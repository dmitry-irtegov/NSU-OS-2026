#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>
#include "shell.h"

struct command cmds[MAX_COMMANDS];
int bkgrnd;
char *infile, *outfile, *appfile;
int shell_interactive;
volatile sig_atomic_t input_interrupted;

struct process {
    pid_t pid;
    int stopped;
};

struct job {
    pid_t pgid;
    unsigned long id;
    int nproc;
    struct process processes[MAX_COMMANDS];
    int stopped_notice;
    struct termios modes;
    char name[128];
};

static struct job jobs[MAX_JOBS];
static unsigned long next_job_id = 1;
static pid_t shell_pgid;
static struct termios shell_modes;

static int set_signal(int sig, void (*handler)(int), int flags) {
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = handler;
    action.sa_flags = flags;
    sigemptyset(&action.sa_mask);
    return sigaction(sig, &action, NULL);
}

static void signal_event(int sig) {
    if (sig == SIGINT) {
        input_interrupted = 1;
    }
}

static int init_shell(void) {
    shell_interactive = isatty(STDIN_FILENO);
    if (shell_interactive) {
        if (set_signal(SIGTTIN, SIG_DFL, SA_RESTART) == -1) {
            return -1;
        }
        for (;;) {
            pid_t foreground = tcgetpgrp(STDIN_FILENO);
            if (foreground == -1) {
                return -1;
            }
            shell_pgid = getpgrp();
            if (foreground == shell_pgid) {
                break;
            }
            if (kill(-shell_pgid, SIGTTIN) == -1) {
                return -1;
            }
        }
        const int ignored[] = {SIGQUIT, SIGTSTP, SIGTTIN, SIGTTOU};
        for (size_t i = 0; i < sizeof(ignored) / sizeof(ignored[0]); i++) {
            if (set_signal(ignored[i], SIG_IGN, SA_RESTART) == -1) {
                return -1;
            }
        }
        if (getpgrp() != getpid() && setpgid(0, 0) == -1) {
            return -1;
        }
        shell_pgid = getpgrp();
        if (tcsetpgrp(STDIN_FILENO, shell_pgid) == -1 ||
            tcgetattr(STDIN_FILENO, &shell_modes) == -1) {
            return -1;
        }
    }

    if (set_signal(SIGCHLD, signal_event, SA_RESTART) == -1) {
        return -1;
    }

    return shell_interactive ? set_signal(SIGINT, signal_event, 0) : 0;
}

static int job_done(const struct job *job) {
    for (int i = 0; i < job->nproc; i++) {
        if (job->processes[i].pid != 0) {
            return 0;
        }
    }
    return 1;
}

static int job_stopped(const struct job *job) {
    if (job_done(job)) {
        return 0;
    }
    for (int i = 0; i < job->nproc; i++) {
        if (job->processes[i].pid != 0 && !job->processes[i].stopped) {
            return 0;
        }
    }
    return 1;
}

static void number_job(struct job *job) {
    if (job->id == 0) {
        job->id = next_job_id++;
    }
}

static void job_notice(struct job *job, const char *state) {
    number_job(job);
    fprintf(stderr, "[%lu] %-8s %ld %s\n", job->id, state, (long)job->pgid,
            job->name);
}

static int signal_job(const struct job *job, int sig) {
    if (shell_interactive) {
        return kill(-job->pgid, sig);
    }
    int result = 0;
    for (int i = 0; i < job->nproc; i++) {
        pid_t pid = job->processes[i].pid;
        if (pid != 0 && kill(pid, sig) == -1 && errno != ESRCH) {
            result = -1;
        }
    }
    return result;
}

static void update_process(pid_t pid, int status) {
    for (int i = 0; i < MAX_JOBS; i++) {
        if (jobs[i].pgid == 0) {
            continue;
        }
        for (int k = 0; k < jobs[i].nproc; k++) {
            struct process *process = &jobs[i].processes[k];
            if (process->pid == pid) {
                if (WIFEXITED(status) || WIFSIGNALED(status)) {
                    process->pid = 0;
                } else {
                    process->stopped = WIFSTOPPED(status);
                }
                return;
            }
        }
    }
}

int collect_jobs(void) {
    for (;;) {
        int status;
        pid_t pid = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED);
        if (pid == -1 && errno == EINTR) {
            continue;
        }
        if (pid <= 0) {
            break;
        }
        update_process(pid, status);
    }

    int notices = 0;
    for (int i = 0; i < MAX_JOBS; i++) {
        struct job *job = &jobs[i];
        if (job->pgid == 0) {
            continue;
        }
        int done = job_done(job);
        int stopped = job_stopped(job);
        if (done || stopped != job->stopped_notice) {
            if (shell_interactive && notices == 0) {
                fputc('\n', stderr);
            }
            job_notice(job, done ? "Done" : stopped ? "Stopped" : "Running");
            notices++;
        }
        job->stopped_notice = stopped;
        if (done) {
            job->pgid = 0;
        }
    }
    return notices;
}

static void restore_terminal(void) {
    if (shell_interactive) {
        if (tcsetpgrp(STDIN_FILENO, shell_pgid) == -1) {
            perror("tcsetpgrp: shell");
        }
        if (tcsetattr(STDIN_FILENO, TCSADRAIN, &shell_modes) == -1) {
            perror("tcsetattr: shell");
        }
    }
}

static int continue_job(struct job *job) {
    if (signal_job(job, SIGCONT) == -1) {
        perror("SIGCONT");
        return -1;
    }
    for (int i = 0; i < job->nproc; i++) {
        job->processes[i].stopped = 0;
    }
    job->stopped_notice = 0;
    return 0;
}

static void cancel_job(struct job *job) {
    for (int i = 0; i < job->nproc; i++) {
        if (job->processes[i].pid != 0) {
            kill(job->processes[i].pid, SIGKILL);
        }
    }
    for (int i = 0; i < job->nproc; i++) {
        pid_t pid = job->processes[i].pid;
        if (pid != 0) {
            while (waitpid(pid, NULL, 0) == -1 && errno == EINTR) {
            }
        }
    }
    job->pgid = 0;
}

static int give_terminal(struct job *job) {
    sigset_t blocked, previous, pending;
    sigemptyset(&blocked);
    sigaddset(&blocked, SIGINT);
    if (sigprocmask(SIG_BLOCK, &blocked, &previous) == -1) {
        return -1;
    }
    int result;
    do {
        result = tcsetpgrp(STDIN_FILENO, job->pgid);
    } while (result == -1 && errno == EINTR);
    int saved_errno = errno;
    if (result == 0) {
        sigpending(&pending);
        if (input_interrupted || sigismember(&pending, SIGINT)) {
            signal_job(job, SIGINT);
        }
    }
    sigprocmask(SIG_SETMASK, &previous, NULL);
    if (result == 0) {
        input_interrupted = 0;
    }
    errno = saved_errno;
    return result;
}

static void foreground_job(struct job *job, int resume, int start_fd) {
    if (shell_interactive) {
        if (tcgetattr(STDIN_FILENO, &shell_modes) == -1 ||
            give_terminal(job) == -1) {
            perror("foreground terminal");
            goto fail;
        }
        if (resume &&
            tcsetattr(STDIN_FILENO, TCSADRAIN, &job->modes) == -1) {
            perror("tcsetattr: job");
            goto fail;
        }
    }
    if (resume && continue_job(job) == -1) {
        goto fail;
    }
    if (start_fd != -1) {
        close(start_fd);
    }

    int signaled = 0;
    while (!job_done(job) && !job_stopped(job)) {
        int status;
        pid_t pid = waitpid(-1, &status, WUNTRACED | WCONTINUED);
        if (pid == -1) {
            if (errno == EINTR) {
                continue;
            }
            perror("waitpid");
            break;
        }
        for (int i = 0; i < job->nproc; i++) {
            if (job->processes[i].pid == pid && WIFSIGNALED(status)) {
                signaled = 1;
            }
        }
        update_process(pid, status);
    }
    if (job_stopped(job) && shell_interactive) {
        if (tcgetattr(STDIN_FILENO, &job->modes) == -1) {
            perror("tcgetattr: job");
        }
    }
    restore_terminal();

    if (job_done(job)) {
        if (shell_interactive && signaled) {
            fputc('\n', stderr);
        }
        job->pgid = 0;
    } else {
        number_job(job);
        if (continue_job(job) == 0) {
            fprintf(stderr, "[%lu] %ld\n", job->id, (long)job->pgid);
        }
    }
    return;

fail:
    if (start_fd != -1) {
        cancel_job(job);
        close(start_fd);
    }
    restore_terminal();
}

static void redirect_file(const char *name, int target, int flags) {
    int fd = open(name, flags, 0666);
    if (fd == -1) {
        perror(name);
        _exit(1);
    }
    if (dup2(fd, target) == -1) {
        perror("dup2");
        _exit(1);
    }
    if (fd != target) {
        close(fd);
    }
}

static void close_pipes(int pipes[][2], int count) {
    for (int i = 0; i < count; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
}

static int wait_ready(int fd, int count) {
    char byte;
    while (count > 0) {
        if (input_interrupted) {
            return -1;
        }
        ssize_t n = read(fd, &byte, 1);
        if (n == 1) {
            count--;
        } else if (n == 0) {
            errno = EIO;
            return -1;
        } else if (errno != EINTR) {
            return -1;
        }
    }
    return input_interrupted ? -1 : 0;
}

static void launch_pipeline(int ncmd) {
    collect_jobs();
    struct job *job = NULL;
    for (int i = 0; i < MAX_JOBS; i++) {
        if (jobs[i].pgid == 0) {
            job = &jobs[i];
            break;
        }
    }
    if (job == NULL) {
        fprintf(stderr, "Слишком много задач\n");
        return;
    }
    memset(job, 0, sizeof(*job));
    job->modes = shell_modes;
    snprintf(job->name, sizeof(job->name), "%s", cmds[0].cmdargs[0]);

    int pipes[MAX_COMMANDS - 1][2];
    int pipe_count = 0;
    int start[2];
    if (pipe(start) == -1) {
        perror("pipe");
        return;
    }
    int ready[2];
    if (pipe(ready) == -1) {
        perror("pipe");
        close(start[0]);
        close(start[1]);
        return;
    }
    for (int i = 0; i < ncmd - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            goto fail;
        }
        pipe_count++;
    }

    fflush(NULL);
    for (int i = 0; i < ncmd; i++) {
        if (input_interrupted) {
            goto fail;
        }
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            goto fail;
        }
        if (pid == 0) {
            close(start[1]);
            close(ready[0]);
            if (shell_interactive && setpgid(0, job->pgid) == -1) {
                perror("setpgid");
                _exit(1);
            }
            const int signals[] = {SIGINT,  SIGQUIT, SIGTSTP, SIGTTIN,
                                   SIGTTOU, SIGCHLD, SIGCONT, SIGPIPE};
            for (size_t k = 0; k < sizeof(signals) / sizeof(signals[0]); k++) {

                if (set_signal(signals[k], SIG_DFL, SA_RESTART) == -1) {
                    perror("sigaction");
                    _exit(1);
                }
            }
            if (bkgrnd && !shell_interactive) {
                if (set_signal(SIGINT, SIG_IGN, SA_RESTART) == -1 ||
                    set_signal(SIGQUIT, SIG_IGN, SA_RESTART) == -1) {
                    perror("sigaction");
                    _exit(1);
                }
            }

            if (write(ready[1], "x", 1) == -1) {
                perror("ready pipe");
                _exit(1);
            }
            close(ready[1]);

            char byte;
            ssize_t count;
            do {
                count = read(start[0], &byte, 1);
            } while (count == -1 && errno == EINTR);
            if (count == -1) {
                perror("start pipe");
                _exit(1);
            }
            close(start[0]);

            if ((i > 0 && dup2(pipes[i - 1][0], STDIN_FILENO) == -1) ||
                (i < ncmd - 1 && dup2(pipes[i][1], STDOUT_FILENO) == -1)) {
                perror("dup2: pipe");
                _exit(1);
            }
            close_pipes(pipes, pipe_count);

            if (i == 0) {
                if (infile != NULL) {
                    redirect_file(infile, STDIN_FILENO, O_RDONLY);
                } else if (bkgrnd && !shell_interactive) {
                    redirect_file("/dev/null", STDIN_FILENO, O_RDONLY);
                }
            }
            if (i == ncmd - 1) {
                if (outfile != NULL) {
                    redirect_file(outfile, STDOUT_FILENO,
                                  O_WRONLY | O_CREAT | O_TRUNC);
                }
                if (appfile != NULL) {
                    redirect_file(appfile, STDOUT_FILENO,
                                  O_WRONLY | O_CREAT | O_APPEND);
                }
            }
            execvp(cmds[i].cmdargs[0], cmds[i].cmdargs);
            perror(cmds[i].cmdargs[0]);
            _exit(127);
        }

        if (job->pgid == 0) {
            job->pgid = pid;
        }
        job->processes[job->nproc++].pid = pid;
        if (shell_interactive && setpgid(pid, job->pgid) == -1) {
            perror("setpgid");
            goto fail;
        }
    }
    close_pipes(pipes, pipe_count);
    close(start[0]);
    close(ready[1]);
    int ok = wait_ready(ready[0], ncmd);
    close(ready[0]);
    if (ok == -1) {
        if (!input_interrupted) {
            perror("child startup");
        }
        cancel_job(job);
        close(start[1]);
        return;
    }
    if (bkgrnd) {
        number_job(job);
        printf("[%lu] %ld\n", job->id, (long)job->pgid);
        fflush(stdout);
        close(start[1]);
    } else {
        foreground_job(job, 0, start[1]);
    }
    return;

fail:
    cancel_job(job);
    close_pipes(pipes, pipe_count);
    close(start[0]);
    close(start[1]);
    close(ready[0]);
    close(ready[1]);
}

static struct job *select_job(const char *text) {
    unsigned long value = 0;
    int by_id = text != NULL && *text == '%';
    if (text != NULL) {
        if (by_id) {
            text++;
        }
        char *end;
        errno = 0;
        value = strtoul(text, &end, 10);
        if (*text < '0' || *text > '9' || *end || value == 0 || errno) {
            return NULL;
        }
    }
    struct job *found = NULL;
    for (int i = 0; i < MAX_JOBS; i++) {
        struct job *job = &jobs[i];
        if (job->pgid == 0) {
            continue;
        }
        if (value == 0) {
            if (found == NULL || job->id > found->id) {
                found = job;
            }
        } else if (by_id ? job->id == value : (unsigned long)job->pgid == value) {
            return job;
        } else if (!by_id && job->id == value) {
            found = job;
        }
    }
    return found;
}

static int builtin(void) {
    const char *name = cmds[0].cmdargs[0];
    int list = strcmp(name, "jobs") == 0;
    int foreground = strcmp(name, "fg") == 0;
    int background = strcmp(name, "bg") == 0;
    int quit = strcmp(name, "exit") == 0;
    if (!list && !foreground && !background && !quit) {
        return 0;
    }
    if (bkgrnd || infile || outfile || appfile) {
        fprintf(stderr, "%s: перенаправления и & здесь не поддерживаются\n",
                name);
        return 1;
    }
    if (cmds[0].argc > ((list || quit) ? 1 : 2)) {
        fprintf(stderr, "%s: слишком много аргументов\n", name);
        return 1;
    }
    if (quit) {
        return 2;
    }
    collect_jobs();
    if (list) {
        for (int i = 0; i < MAX_JOBS; i++) {
            if (jobs[i].pgid != 0) {
                printf("[%lu] %-8s %ld %s\n", jobs[i].id,
                       job_stopped(&jobs[i]) ? "Stopped" : "Running",
                       (long)jobs[i].pgid, jobs[i].name);
            }
        }
        fflush(stdout);
        return 1;
    }
    if (!shell_interactive) {
        fprintf(stderr, "%s: требуется интерактивный терминал\n", name);
        return 1;
    }
    struct job *job = select_job(cmds[0].cmdargs[1]);
    if (job == NULL) {
        fprintf(stderr, "%s: задача не найдена\n", name);
        return 1;
    }
    if (foreground) {
        foreground_job(job, 1, -1);
    } else if (continue_job(job) == 0) {
        job_notice(job, "Running");
    }
    return 1;
}

static void shutdown_shell(void) {
    if (shell_interactive) {
        for (int i = 0; i < MAX_JOBS; i++) {
            if (jobs[i].pgid != 0) {
                signal_job(&jobs[i], SIGHUP);
                signal_job(&jobs[i], SIGCONT);
            }
        }
        restore_terminal();
    }
    set_signal(SIGCHLD, SIG_DFL, SA_RESTART);
    if (shell_interactive) {
        set_signal(SIGINT, SIG_IGN, SA_RESTART);
    }
}

int main(void) {
    char line[MAX_LINE];
    int input_error = 0;
    int quit = 0;
    if (init_shell() == -1) {
        perror("shell initialization");
        return 1;
    }
    while (!quit) {
        collect_jobs();
        int result = prompline(line, sizeof(line));
        if (result == 0 || result == -2) {
            input_error = result == -2;
            break;
        }
        if (result < 0) {
            continue;
        }
        char *p = line;
        while (*p && !quit && !input_interrupted) {
            int ncmd = parseline(&p);
            if (ncmd == -1) {
                break;
            }
            if (ncmd == 0) {
                continue;
            }
            int handled = ncmd == 1 ? builtin() : 0;
            if (handled == 2) {
                quit = 1;
            } else if (handled == 0) {
                launch_pipeline(ncmd);
            }
        }
    }
    shutdown_shell();
    return input_error;
}
