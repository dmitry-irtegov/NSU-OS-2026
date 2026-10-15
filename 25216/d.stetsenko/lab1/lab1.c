#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/resource.h>

extern char **environ;

static void print_ids(void) {
    printf("real uid=%d, effective uid=%d\n", (int)getuid(), (int)geteuid());
    printf("real gid=%d, effective gid=%d\n", (int)getgid(), (int)getegid());
}

static void become_group_leader(void) {
    if (setpgid(0, 0) == -1)
        perror("setpgid");
}

static void print_pids(void) {
    printf("pid=%d, ppid=%d, pgid=%d\n", (int)getpid(), (int)getppid(), (int)getpgrp());
}

static void print_ulimit(void) {
    struct rlimit rl;
    if (getrlimit(RLIMIT_FSIZE, &rl) == 0)
        printf("ulimit=%ld\n", (long)rl.rlim_cur);
    else
        perror("getrlimit");
}

static void set_ulimit(const char *val) {
    struct rlimit rl;
    if (getrlimit(RLIMIT_FSIZE, &rl) == -1) {
        perror("getrlimit");
        return;
    }
    rl.rlim_cur = (rlim_t)atol(val);
    if (setrlimit(RLIMIT_FSIZE, &rl) == -1)
        perror("setrlimit");
}

static void print_core_size(void) {
    struct rlimit rl;
    if (getrlimit(RLIMIT_CORE, &rl) == 0)
        printf("core size=%ld\n", (long)rl.rlim_cur);
    else
        perror("getrlimit");
}

static void set_core_size(const char *val) {
    struct rlimit rl;
    if (getrlimit(RLIMIT_CORE, &rl) == -1) {
        perror("getrlimit");
        return;
    }
    rl.rlim_cur = (rlim_t)atol(val);
    if (setrlimit(RLIMIT_CORE, &rl) == -1)
        perror("setrlimit");
}

static void print_cwd(void) {
    char buf[4096];
    if (getcwd(buf, sizeof(buf)) != NULL)
        printf("cwd=%s\n", buf);
    else
        perror("getcwd");
}

static void print_env(void) {
    for (char **e = environ; *e != NULL; e++)
        printf("%s\n", *e);
}

static void set_env(const char *name_eq_value) {
    putenv(strdup(name_eq_value));
}

int main(int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "ispuU:cC:dvV:")) != -1) {
        switch (opt) {
        case 'i': print_ids(); break;
        case 's': become_group_leader(); break;
        case 'p': print_pids(); break;
        case 'u': print_ulimit(); break;
        case 'U': set_ulimit(optarg); break;
        case 'c': print_core_size(); break;
        case 'C': set_core_size(optarg); break;
        case 'd': print_cwd(); break;
        case 'v': print_env(); break;
        case 'V': set_env(optarg); break;
        case '?':
        default:
            fprintf(stderr, "unknown option: -%c\n", optopt);
            break;
        }
    }
    return 0;
}