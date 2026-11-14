#define _POSIX_C_SOURCE 200809L

#include "shell.h"
#include <stdlib.h>

char *infile, *outfile, *appfile;
struct command cmds[MAXCMDS];
char bkgrnd;

Job all_jobs[MAXCMDS];
Job *jobs_foreground;

pid_t shell_pgid;
struct termios shell_tmodes;
bool shell_interactive;

void init_shell(void) {
    shell_interactive = isatty(STDIN_FILENO);
    shell_pgid = getpgrp();

    if (!shell_interactive)
        return;

    // Убеждаемся, что shell находится на переднем плане
    while (tcgetpgrp(STDIN_FILENO) != getpgrp()) {
        if (kill(-getpgrp(), SIGTTIN) < 0) {
            perror("SIGTTIN");
            exit(EXIT_FAILURE);
        }
    }

    // Shell не должен завершаться от клавиатурных сигналов
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);

    // Создаём отдельную группу для shell
    shell_pgid = getpid();

    if (getpgrp() != shell_pgid) {
        if (setpgid(0, 0) < 0) {
            perror("setpgid shell");
            exit(EXIT_FAILURE);
        }
    }

    // Передаём терминал группе shell
    if (tcsetpgrp(STDIN_FILENO, shell_pgid) < 0) {
        perror("tcsetpgrp shell");
        exit(EXIT_FAILURE);
    }

    // Сохраняем настройки терминала
    if (tcgetattr(STDIN_FILENO, &shell_tmodes) < 0) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }
}

void standart_shell_signals() {
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);
    signal(SIGTTIN, SIG_DFL);
    signal(SIGTTOU, SIG_DFL);
}

int redirect_file(const char *path, int flags, int target) {
    int fd = open(path, flags, 0666);

    if (fd < 0) {
        perror(path);
        return -1;
    }

    if (dup2(fd, target) < 0) {
        perror("dup2");

        if (fd != target)
            close(fd);

        return -1;
    }

    if (fd != target)
        close(fd);

    return 0;
}

void return_terminal(void) {
    if (!shell_interactive)
        return;

    if (tcsetpgrp(STDIN_FILENO, shell_pgid) < 0)
        perror("tcsetpgrp shell");

    if (tcsetattr(STDIN_FILENO, TCSADRAIN, &shell_tmodes) < 0)
        perror("tcsetattr shell");
}

bool is_job_done(Job *job) {
    if (job->nprocesses == 0) {
        return false;
    }
    for (int i = 0; i < job->nprocesses; i++) {
        if (job->state[i] != DONE) {
            return false;
        }
    }
    return true;
}

void check_zombie_processes(void) {
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED)) > 0) {

        Job *job = NULL;

        // Ищем задание и процесс внутри него
        for (int i = 0; i < MAXCMDS && !job; i++) {
            if (!all_jobs[i].used)
                continue;

            for (int j = 0; j < all_jobs[i].nprocesses; j++) {

                if (all_jobs[i].pids[j] == pid) {
                    job = &all_jobs[i];
                    break;
                }
            }
        }

        if (job == NULL)
            continue;

        update_process_state(job, pid, status);

        // Проверяем, завершилось ли всё задание
        if (is_job_done(job)) {
            printf("job %d finished\n", job->id);
            job->used = 0;
        }
    }
}

Job *find_job(const char *arg) {
    Job *result = NULL;

    if (arg == NULL) {
        // Без номера выбираем последнее задание
        for (int i = 0; i < MAXCMDS; i++) {
            if (!all_jobs[i].used)
                continue;

            if (result == NULL || all_jobs[i].id > result->id)
                result = &all_jobs[i];
        }

        return result;
    }

    // Пропускаем символ %
    if (*arg == '%')
        arg++;

    char *end;
    errno = 0;

    long id = strtol(arg, &end, 10);

    if (errno != 0 || arg == end || *end != '\0' || id <= 0)
        return NULL;

    for (int i = 0; i < MAXCMDS; i++) {
        if (all_jobs[i].used && all_jobs[i].id == id)
            return &all_jobs[i];
    }

    return NULL;
}

bool is_job_stopped(Job *job) {
    bool has_stopped = false;

    for (int i = 0; i < job->nprocesses; i++) {
        if (job->state[i] == RUNNING)
            return false;

        if (job->state[i] == STOPPED)
            has_stopped = true;
    }

    return has_stopped;
}

void mark_job_running(Job *job) {
    for (int i = 0; i < job->nprocesses; i++) {
        if (job->state[i] == STOPPED)
            job->state[i] = RUNNING;
    }
}

int builtin_bg(Job *job) {
    if (job == NULL) {
        fprintf(stderr, "bg: job not found\n");
        return 1;
    }

    if (!is_job_stopped(job)) {
        fprintf(stderr, "bg: job is not stopped\n");
        return 1;
    }

    if (kill(-job->pgid, SIGCONT) < 0) {
        perror("SIGCONT");
        return 1;
    }

    mark_job_running(job);

    printf("[%d] Running %s\n", job->id, job->name);
    return 0;
}

int builtin_fg(Job *job) {
    if (job == NULL) {
        fprintf(stderr, "fg: job not found\n");
        return 1;
    }

    bool stopped = is_job_stopped(job);

    // Передаём терминал группе задания
    if (shell_interactive) {
        if (tcsetpgrp(STDIN_FILENO, job->pgid) < 0) {
            perror("tcsetpgrp fg");
            return 1;
        }

        // Восстанавливаем настройки терминала задания
        if (stopped && job->saved_modes) {
            if (tcsetattr(STDIN_FILENO, TCSADRAIN, &job->tmodes) < 0) {
                perror("tcsetattr fg");
            }
        }
    }

    jobs_foreground = job;

    // Если задание остановлено, продолжаем его
    if (stopped) {
        if (kill(-job->pgid, SIGCONT) < 0) {
            perror("SIGCONT");

            jobs_foreground = NULL;
            return_terminal();
            return 1;
        }

        mark_job_running(job);
    }

    // Ждём завершения или остановки задания
    return wait_foreground(job);
}

static bool run_builtin(int index, int *status) {
    char *name = cmds[index].cmdargs[0];

    if (name == NULL)
        return false;

    if (strcmp(name, "exit") == 0) {
        exit(0);
    }

    if (strcmp(name, "jobs") == 0) {
        for (int i = 0; i < MAXCMDS; i++) {
            Job *job = &all_jobs[i];

            if (!job->used)
                continue;

            printf("[%d] PGID=%ld %s\n",
                   job->id, (long)job->pgid, job->name);
        }

        *status = 0;
        return true;
    }

    if (strcmp(name, "fg") == 0) {
        Job *job = find_job(cmds[index].cmdargs[1]);

        *status = builtin_fg(job);
        return true;
    }

    if (strcmp(name, "bg") == 0) {
        Job *job = find_job(cmds[index].cmdargs[1]);

        *status = builtin_bg(job);
        return true;
    }

    return false;
}

void update_process_state(Job *job, pid_t pid, int status) {
    for (int i = 0; i < job->nprocesses; i++) {
        if (job->pids[i] != pid)
            continue;

        if (WIFEXITED(status) || WIFSIGNALED(status)) {
            job->state[i] = DONE;

            if (i == job->nprocesses - 1) {
                if (WIFEXITED(status)) {
                    job->exit_code = WEXITSTATUS(status);
                } else {
                    job->exit_code = 128 + WTERMSIG(status);
                }

                job->has_exit_code = true;
            }
        } else if (WIFSTOPPED(status)) {
            job->state[i] = STOPPED;
        } else if (WIFCONTINUED(status)) {
            job->state[i] = RUNNING;
        }

        break;
    }
}

int add_group(pid_t pid, pid_t pgid) {
    if (setpgid(pid, pgid) < 0) {
        perror("setpgid");
        return -1;
    }

    return 0;
}

int wait_foreground(Job *job) {
    int status;
    bool stopped_with_tstp = false;
    int stop_signal = 0;

    while (!is_job_done(job) && !is_job_stopped(job)) {
        pid_t pid = waitpid(-job->pgid, &status, WUNTRACED | WCONTINUED);

        if (pid < 0) {
            if (errno == EINTR)
                continue;

            if (errno == ECHILD) {
                for (int i = 0; i < job->nprocesses; i++)
                    job->state[i] = DONE;
            } else {
                perror("waitpid");
            }

            break;
        }

        if (WIFSTOPPED(status)) {
            stop_signal = WSTOPSIG(status);

            if (stop_signal == SIGTSTP)
                stopped_with_tstp = true;
        }

        update_process_state(job, pid, status);
    }

    // Сохраняем настройки терминала остановленного задания
    if (shell_interactive && !is_job_done(job)) {
        if (tcgetattr(STDIN_FILENO, &job->tmodes) == 0)
            job->saved_modes = 1;
    }

    // Возвращаем терминал shell
    return_terminal();
    jobs_foreground = NULL;

    // Все процессы завершились
    if (is_job_done(job)) {
        job->used = 0;

        return job->has_exit_code ? job->exit_code : 1;
    }

    // Все оставшиеся процессы остановлены
    if (is_job_stopped(job)) {

        // По заданию Ctrl+Z переводит процесс в фон
        if (stopped_with_tstp) {
            fprintf(stderr, "[%d] Background PID: %ld\n", job->id,
                    (long)job->pgid);

            if (kill(-job->pgid, SIGCONT) < 0) {
                perror("SIGCONT");
            } else {
                mark_job_running(job);
            }
        } else {
            printf("[%d] Stopped %s\n", job->id, job->name);
        }

        return 128 + (stopped_with_tstp
                          ? SIGTSTP
                          : (stop_signal ? stop_signal : SIGSTOP));
    }
    return 1;
}

int launch_pipeline(int first, int last, int ncmds) {
    int ind = -1;

    // Ищем свободное место в таблице заданий
    for (int i = 0; i < MAXCMDS; i++) {
        if (!all_jobs[i].used) {
            ind = i;
            break;
        }
    }

    if (ind == -1) {
        fprintf(stderr, "Maximum number of jobs reached\n");
        return 1;
    }

    // Создаём новое задание
    Job *job = &all_jobs[ind];
    memset(job, 0, sizeof(*job));
    job->used = 1;
    job->id = ind + 1;
    snprintf(job->name, sizeof(job->name), "%s", cmds[first].cmdargs[0]);

    // Канал синхронизации запуска процессов
    int start_pipe[2];

    if (pipe(start_pipe) < 0) {
        perror("pipe");
        job->used = 0;
        return 1;
    }

    int previous_read = -1;
    bool failed = false;
    fflush(NULL);

    // Запускаем все команды конвейера
    for (int i = first; i <= last; i++) {
        int next_pipe[2] = {-1, -1};

        // Создаём канал, если команда не последняя
        if (i < last) {
            if (pipe(next_pipe) < 0) {
                perror("pipe");
                failed = true;
                break;
            }
        }

        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");

            if (next_pipe[0] != -1) {
                close(next_pipe[0]);
                close(next_pipe[1]);
            }

            failed = true;
            break;
        }

        if (pid == 0) {
            // Дочерний процесс

            close(start_pipe[1]);

            // Первый процесс создаёт группу,
            // остальные присоединяются к ней
            pid_t pgid = job->pgid ? job->pgid : getpid();

            if (setpgid(0, pgid) < 0) {
                perror("setpgid child");
                _exit(1);
            }

            standart_shell_signals();

            // Ожидаем, пока родитель настроит терминал
            char c;
            ssize_t result;

            do {
                result = read(start_pipe[0], &c, 1);
            } while (result < 0 && errno == EINTR);

            if (result < 0) {
                perror("read start_pipe");
                _exit(1);
            }

            close(start_pipe[0]);

            // Получаем ввод из предыдущего канала
            if (previous_read != -1) {
                if (dup2(previous_read, STDIN_FILENO) < 0) {
                    perror("dup2 stdin");
                    _exit(1);
                }
            }

            // Отправляем вывод в следующий канал
            if (next_pipe[1] != -1) {
                if (dup2(next_pipe[1], STDOUT_FILENO) < 0) {
                    perror("dup2 stdout");
                    _exit(1);
                }
            }

            // Закрываем лишние дескрипторы
            if (previous_read != -1 && previous_read != STDIN_FILENO)
                close(previous_read);

            if (next_pipe[0] != -1)
                close(next_pipe[0]);

            if (next_pipe[1] != -1 && next_pipe[1] != STDOUT_FILENO)
                close(next_pipe[1]);

            // Перенаправление ввода первой команды
            if (i == 0 && infile != NULL) {
                if (redirect_file(infile, O_RDONLY, STDIN_FILENO) < 0)
                    _exit(1);
            }

            // Перенаправление вывода последней команды
            if (i == ncmds - 1) {
                if (outfile != NULL) {
                    if (redirect_file(outfile, O_WRONLY | O_CREAT | O_TRUNC,
                                      STDOUT_FILENO) < 0)
                        _exit(1);
                } else if (appfile != NULL) {
                    if (redirect_file(appfile, O_WRONLY | O_CREAT | O_APPEND,
                                      STDOUT_FILENO) < 0)
                        _exit(1);
                }
            }

            // Запускаем программу
            execvp(cmds[i].cmdargs[0], cmds[i].cmdargs);

            perror(cmds[i].cmdargs[0]);
            _exit(127);
        }

        // Родительский процесс

        // PID первой команды становится PGID группы
        if (job->pgid == 0)
            job->pgid = pid;

        // Регистрируем процесс в задании
        int k = job->nprocesses;

        job->pids[k] = pid;
        job->state[k] = RUNNING;
        job->nprocesses++;

        // Помещаем процесс в общую группу
        if (setpgid(pid, job->pgid) < 0 && errno != EACCES) {
            perror("setpgid parent");
            failed = true;
        }

        // Родителю предыдущий канал больше не нужен
        if (previous_read != -1)
            close(previous_read);

        // Записывающий конец нового канала тоже не нужен
        if (next_pipe[1] != -1)
            close(next_pipe[1]);

        // Сохраняем читающий конец для следующей команды
        previous_read = next_pipe[0];

        if (failed)
            break;
    }

    if (previous_read != -1)
        close(previous_read);

    close(start_pipe[0]);

    // Передаём терминал foreground-заданию
    if (!failed && !bkgrnd && shell_interactive) {
        if (tcsetpgrp(STDIN_FILENO, job->pgid) < 0) {
            perror("tcsetpgrp job");
            failed = true;
        }
    }

    // Если конвейер создать не удалось
    if (failed) {
        for (int i = 0; i < job->nprocesses; i++)
            kill(job->pids[i], SIGTERM);

        if (job->pgid > 0)
            kill(-job->pgid, SIGTERM);

        close(start_pipe[1]);

        for (int i = 0; i < job->nprocesses; i++) {
            while (waitpid(job->pids[i], NULL, 0) < 0 && errno == EINTR) {
            }
        }

        return_terminal();

        job->used = 0;
        return 1;
    }

    // Разрешаем дочерним процессам продолжить работу
    close(start_pipe[1]);

    if (bkgrnd) {
        printf("[%d] PID=%ld\n", job->id, (long)job->pgid);
        fflush(stdout);

        return 0;
    }

    jobs_foreground = job;

    return wait_foreground(job);
}

int execute_command(int ncmds) {
    int status = 0;

    enum chain_operator previous = CHAIN_SEQ;

    for (int first = 0; first < ncmds;) {
        int last = first;

        // Определяем границы конвейера
        while (last + 1 < ncmds &&
               (cmds[last].cmdflag & OUTPIP)) {
            last++;
        }

        bool should_run =
            previous == CHAIN_NONE ||
            previous == CHAIN_SEQ ||
            (previous == CHAIN_AND && status == 0) ||
            (previous == CHAIN_OR && status != 0);

        if (should_run) {
            int builtin_status = 0;

            // Встроенные команды выполняются самим shell
            if (first == last &&
                run_builtin(first, &builtin_status)) {
                status = builtin_status;
            } else {
                status = launch_pipeline(first, last, ncmds);
            }
        }

        // Запоминаем оператор после текущего конвейера
        previous = chain_after[last];

        first = last + 1;
    }

    return status;
}

int main(int argc, char *argv[]) {
    (void)argc;

    char line[1024];
    char prompt[50];

    snprintf(prompt, sizeof(prompt), "[%s] ", argv[0]);

    init_shell();

    while (1) {
        check_zombie_processes();

        int nread = promptline(prompt, line, sizeof(line));

        if (nread <= 0)
            break;

        check_zombie_processes();

        int ncmds = parseline(line);

        if (ncmds <= 0)
            continue;

        execute_command(ncmds);
    }

    return 0;
}