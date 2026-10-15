#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/resource.h>

extern char** environ; 

typedef struct option_t {
    int name;
    char* arg;
} option;

int main(int argc, char* argv[]) {
    int size = 0;
    for(int i = 1; i < argc; i++) {
        size += strlen(argv[i]);
    }
    if (size == 0) {
        return 0;
    }
    option* opt_arr = calloc(size, sizeof(option));
    int c;
    int opt_number = 0;
    while((c = getopt(argc, argv, "ispuU:cC:dvV:")) != -1) {
        opt_arr[opt_number].name = c;
        if (optarg != NULL) {
            opt_arr[opt_number].arg = malloc((strlen(optarg) + 1) * sizeof(char));
            strcpy(opt_arr[opt_number].arg, optarg);
        }
        opt_number++;
    }
    for(int i = opt_number - 1; i >= 0; i--) {
        switch(opt_arr[i].name) {
            case 'i': {
                uid_t ruid = getuid();
                printf("Real UID: %u\n", ruid);
                uid_t euid = geteuid();
                printf("Effective UID: %u\n", euid);
                gid_t gid = getgid();
                printf("Real GID: %u\n", gid);
                gid_t egid = getegid();
                printf("Effective GID: %u\n", egid);
                break;
            }
            case 's': {
                int res = setpgid(0, 0);
                if (res != 0) {
                    perror("Failed to change PGID");
                }
                break;
            }
            case 'p': {
                pid_t pid = getpid();
                printf("PID: %d\n", pid);
                pid_t ppid = getppid();
                printf("Parent PID: %d\n", ppid);
                pid_t pgid = getpgrp();
                printf("PGID: %d\n", pgid);
                break;
            }
            case 'u': {
                struct rlimit rlp;
                int res = getrlimit(RLIMIT_FSIZE, &rlp);
                if (res != 0) {
                    perror("Can't get RLIMIT_FSIZE");
                }
                else {
                    printf("RLIMIT_FSIZE: %lu\n", rlp.rlim_cur);
                }
                break;
            }
            case 'U': {
                struct rlimit rlp;
                int res = getrlimit(RLIMIT_FSIZE, &rlp);
                if (res != 0) {
                    perror("Can't get RLIMIT_FSIZE");
                    break;
                }
                rlp.rlim_cur = atol(opt_arr[i].arg);
                res = setrlimit(RLIMIT_FSIZE, &rlp);
                if (res != 0) {
                    perror("Failed to change soft limit");
                }
                else {
                    printf("New soft limit: %lu\n", rlp.rlim_cur);
                }
                break;
            }
            case 'c': {
                struct rlimit rlp;
                int res = getrlimit(RLIMIT_CORE, &rlp);
                if (res != 0) {
                    perror("Can't get RLIMIT_CORE");
                }
                else {
                    printf("RLIMIT_CORE: %lu\n", rlp.rlim_cur);
                }
                break;
            }
            case 'C': {
                struct rlimit rlp;
                int res = getrlimit(RLIMIT_CORE, &rlp);
                if (res != 0) {
                    perror("Can't get RLIMIT_CORE");
                    break;
                }
                rlp.rlim_cur = atol(opt_arr[i].arg);
                res = setrlimit(RLIMIT_CORE, &rlp);
                if (res != 0) {
                    perror("Failed to change soft limit");
                }
                else {
                    printf("New soft limit: %lu\n", rlp.rlim_cur);
                }
                break;
            }
            case 'd': {
                char buf[1024];
                char* res = getcwd(buf, sizeof(buf));
                if (res == NULL) {
                    perror("Failed to get the current working directory");
                }
                else {
                    printf("Current working directory %s\n", buf);
                }
                break;
            }
            case 'v': {
                char** p;
                for(p = environ; *p != NULL; p++) {
                    printf("%s\n", *p);
                }
                break;
            }
            case 'V': {
                char* p = strchr(opt_arr[i].arg, '=');
                if (p == NULL) {
                    printf("Incorrect argument in option 'V'\n");
                    break;
                }
                size_t env_var_size = (size_t)(p - opt_arr[i].arg) + 1;
                size_t val_size = strlen(p + 1) + 1;
                char* env_var = malloc(env_var_size * sizeof(char));
                char* val = malloc(val_size * sizeof(char));
                int index = 0;
                for(char* k = opt_arr[i].arg; k < p; k++) {
                    env_var[index++] = *k; 
                }
                env_var[index] = '\0';
                index = 0;
                for(char* k = p + 1; *k != '\0'; k++) {
                    val[index++] = *k; 
                }
                val[index] = '\0';
                int res = setenv(env_var, val, 1);
                if (res != 0) {
                    perror("Failed to change environment variable");
                }
                else {
                    printf("%s=%s\n", env_var, getenv(env_var));
                }
                free(env_var);
                free(val);
                break;
            }
            case '?': {
                printf("Invalid option\n");
                break;
            }
        }
    }
    for(int i = 0; i < opt_number; i++) {
        free(opt_arr[i].arg);
    }
    free(opt_arr);
    return 0;
}
