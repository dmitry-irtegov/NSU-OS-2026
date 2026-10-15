#include <stdio.h>
#include <unistd.h>
#include <ulimit.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/resource.h>

extern char** environ;

int is_number(char* str) {
    int len = strlen(str);

    for (int i = 0; i < len; i++) {
        if (!isdigit(str[i])){
            printf("Size is incorrect\n");
            return 0;
            break;
        }
    }
    
    return 1;
}

int main(int argc, char *argv[], char *envp[]) {
    char options[] = "ispuU:cC:dvV:";
    int c, invalid = 0;

    while ((c = getopt(argc, argv, options)) != EOF) {
        printf("----------\n");
        switch (c) {
            case 'i': {
                printf("User UID: %d.\n", getuid());
                printf("User EUID: %d.\n", geteuid());
                printf("Group UID: %d.\n", getgid());
                printf("Group EUID: %d.\n", getegid());
                break;
            }
            case 's': {
                printf("Process %d becomes lider.\n", getpgrp());
                setpgid(0, 0);
                break;
            }
            case 'p': {
                printf("Process ID: %d.\n", getpid());
                printf("Parent process ID: %d.\n", getppid());
                printf("Group process ID: %d.\n", getpgrp());
                break;
            }
            case 'u': {
                printf("User limit size: %ld.\n", ulimit(UL_GETFSIZE));
                break;
            }
            case 'U': {
                if (!is_number(optarg)) break;

                long new_size = ulimit(UL_SETFSIZE, atol(optarg));
                if (new_size == -1) {
                    printf("ERROR! ulimit: Permission denied.\n");
                }
                printf("User limit updated! New size: %ld units.\n", new_size);
                break;
            }
            case 'c': {
                struct rlimit limit;
                if (getrlimit(RLIMIT_CORE, &limit) == 0) {
                    if (limit.rlim_cur == RLIM_INFINITY) {
                        printf("Core-file limit is unlimited.\n");
                    } else {
                        printf("Core-file limit: %ld bytes.\n", limit.rlim_cur);
                    }
                } else {
                    printf("ERROR! Fail to get core-file limit.\n");
                }
                break;
            }
            case 'C': {
                if (!is_number(optarg)) break;

                struct rlimit limit;
                if (getrlimit(RLIMIT_CORE, &limit) != 0){
                    printf("ERROR! Fail to get core-file limit.\n");
                    break;
                }
                
                limit.rlim_cur = strtol(optarg, (char **)NULL, 10);

                if (limit.rlim_cur > limit.rlim_max){
                    limit.rlim_max = limit.rlim_cur;
                }
                
                if (setrlimit(RLIMIT_CORE, &limit) != 0){
                    printf("ERROR! setrlimit: Permission denied.\n");
                    break;
                }
                printf("Core-file limit updated! New size: %ld bytes.\n", limit.rlim_cur);
                break;
            }
            case 'd': {
                printf("Current work directory: %s\n", getcwd(NULL, 150));
                break;
            }
            case 'v': {
                printf("Environment variables: \n");
                for (char** var = environ; *var != NULL; var++){
                    printf("%s\n", *var);
                }
                break;
            }
            case 'V': {
                putenv(optarg);
                printf("Variable added: %s.\n", optarg);
                break;
            }
            case '?': {
                printf("invalid option is -- %c.\n", optopt);
                invalid++;
            }
        }
    }

    return 0;
}