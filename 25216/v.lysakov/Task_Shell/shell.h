#define MAXARGS 256
#define MAXCMDS 50

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

enum chain_operator { CHAIN_NONE, CHAIN_SEQ, CHAIN_AND, CHAIN_OR };

extern enum chain_operator chain_after[MAXCMDS];

struct command {
    char *cmdargs[MAXARGS];
    char cmdflag;
};

/*  cmdflag's  */
#define OUTPIP 01
#define INPIP 02

extern struct command cmds[];
extern char *infile, *outfile, *appfile;
extern char bkgrnd;

enum process_state { RUNNING, STOPPED, DONE };
typedef struct {
    int used;
    int id;

    pid_t pgid;

    int nprocesses;
    pid_t pids[MAXCMDS];
    enum process_state state[MAXCMDS];

    struct termios tmodes;
    int saved_modes;

    char name[128];

    int exit_code;
    bool has_exit_code;
} Job;

int parseline(char *);
int promptline(char *, char *, int);

void init_shell(void);
void standart_shell_signals();
void return_terminal(void);
bool is_job_done(Job *job);
void check_zombie_processes(void);
Job *find_job(const char *arg);
bool is_job_stopped(Job *job);
void mark_job_running(Job *job);
int builtin_bg(Job *job);
int builtin_fg(Job *job);
int add_group(pid_t pid, pid_t pgid);
int wait_foreground(Job *job);
int execute_command(int ncmds);
int redirect_file(const char *path, int flags, int target);
int launch_pipeline(int first, int last, int ncmds);
void update_process_state(Job *job, pid_t pid, int status);