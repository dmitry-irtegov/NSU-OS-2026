#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <ulimit.h>
#include <errno.h>
#include <sys/resource.h>
#include <string.h>

extern char **environ;
extern int optopt;
extern char *optarg;

typedef struct option {
    char letter;
    char* argument;
    char invalid_option;
} option;

int main(int argc, char *argv[]) {
    char options[] = ":ispuU:cC:dvV:";
    int c;

    option all_options[100];
    int cnt = 0;

    while ((c = getopt(argc, argv, options)) != EOF) {
        if (cnt >= 100) {
            fprintf(stderr, "Too many options\n");
            return 1;
        }
        if (c == '?' || c == ':') {
            all_options[cnt].letter = c;
            all_options[cnt].argument = NULL;
            all_options[cnt].invalid_option = optopt;
            cnt++;
        } else if (isupper(c)) {
            all_options[cnt].letter = c;
            all_options[cnt].argument = optarg;
            all_options[cnt].invalid_option = '\0';
            cnt++;
        } else {
            all_options[cnt].letter = c;
            all_options[cnt].argument = NULL;
            all_options[cnt].invalid_option = '\0';
            cnt++;
        }
    }

    for (int index = cnt - 1; index >= 0; index--) {
        int c = all_options[index].letter;
        switch (c) {
            case 'i':
                printf("User's real ID: %lu\n", (unsigned long) getuid());
                printf("User's effective ID: %lu\n", (unsigned long) geteuid());
                printf("Group's real ID: %lu\n", (unsigned long) getgid());
                printf("Group's effective ID: %lu\n", (unsigned long) getegid());
                break;
            case 's':
                if (setpgid(getpid(), getpid()) == -1) {
                    perror("setpgid");
                }
                break;
            case 'p':
                printf("Process's ID: %lu\n", (unsigned long) getpid());
                printf("Process parent's ID: %lu\n", (unsigned long) getppid());
                printf("Process group's ID: %lu\n", (unsigned long) getpgrp());
                break;
            case 'u':
                long cur_ulimit = ulimit(UL_GETFSIZE);
                if (cur_ulimit == -1) {
                    perror("ulimit_get");
                } else {
                    printf("Current value of ulimit: %ld\n", cur_ulimit);
                }
                break;
            case 'U':
                char* endptr_u;
                errno = 0;
                long new_ulimit = strtol(all_options[index].argument, &endptr_u, 10);
                if (errno != 0) {
                    perror("strtol");
                } else if (*endptr_u != '\0') {
                    printf("Invalid argument: %s is not a number\n", all_options[index].argument);
                } else {
                    if (ulimit(UL_SETFSIZE, new_ulimit) == -1) {
                        perror("ulimit_set");
                    }
                }
                break;
            case 'c':
                struct rlimit cur_rlimit; 
                if (getrlimit(RLIMIT_CORE, &cur_rlimit) == -1) {
                    perror("getrlimit");
                } else {
                    printf("Current core file size limit: %lu\n", (unsigned long) cur_rlimit.rlim_cur);
                }
                break;
            case 'C':
                char* endptr_c;
                errno = 0;
                long new_size = strtol(all_options[index].argument, &endptr_c, 10);
                if (errno != 0) {
                    perror("strtol");
                } else if (*endptr_c != '\0') {
                    printf("Invalid argument: %s is not a number\n", all_options[index].argument);
                } else {
                    struct rlimit cur_rlimit;
                    if (getrlimit(RLIMIT_CORE, &cur_rlimit) == -1) {
                        perror("getrlimit");
                    } else {
                        cur_rlimit.rlim_cur = new_size;
                        if (setrlimit(RLIMIT_CORE, &cur_rlimit) == -1) {
                            perror("setrlimit");
                        }
                    }
                }
                break;
            case 'd':
                char current_path[256];
                if (getcwd(current_path, sizeof(current_path)) != NULL) {
                    printf("Current working directory: %s\n", current_path);
                } else {
                    perror("getcwd");
                }
                break;
            case 'v':
                int ind_env = 0;
                while (environ[ind_env] != NULL) {
                    printf("%s\n", environ[ind_env]);
                    ind_env++;
                }
                break;
            case 'V':
                char *equal = strchr(all_options[index].argument, '=');
                if (equal != NULL) {
                    *equal = '\0';
                    char* name = all_options[index].argument;
                    char* value = equal + 1;
                    if (setenv(name, value, 1) == -1) {
                        perror("setenv");
                    }
                } else {
                    printf("option V must have an argument in the form name=value\n");
                }
                break;
            case '?':
                printf("invalid option is %c\n", all_options[index].invalid_option);
                break;
            case ':':
                printf("option %c must have a mandatory argument.\n", all_options[index].invalid_option);
                break;
            default:
                break;
        }
    }
}