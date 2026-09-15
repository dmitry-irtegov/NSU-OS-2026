#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <sys/resource.h>

#define MAX_OPTS 256
#define MAX_ARG 256

struct rlimit rl;

int main(int argc, char *argv[]) {
   int opt;
   char opts[MAX_OPTS][MAX_ARG];
   int cnt = 0;
   while ((opt = getopt(argc, argv, ":ispuU:cC:dvV:")) != -1) {
      if (opt == ':') {
        fprintf(stderr, "Option requires an argument\n");
        exit(EXIT_FAILURE);
     }
      opts[cnt][0] = (char)opt;
      opts[cnt][1] = '\0';
      if (optarg != NULL) {
            strncpy(opts[cnt] + 1, optarg, MAX_ARG - 2);
            opts[cnt][MAX_ARG - 1] = '\0';
      }
      cnt++;
   }
   for (int i = cnt - 1; i >= 0; i--) {
      char o = opts[i][0];
      char *arg = opts[i] + 1;
      switch (o) {
         case 'i':
            //-i  Печатает реальные и эффективные идентификаторы пользователя и группы.
            printf("Real user ID: %d\n", (int)getuid());
            printf("Effective user ID: %d\n", (int)geteuid());
            printf("Real group ID: %d\n", (int)getgid());
            printf("Effective group ID: %d\n", (int)getegid());
            break;
         case 's':
            // -s  Процесс становится лидером группы. 
            if (setpgid(0, 0) == -1) {
               perror("setpgid");
               break;
            } else {
               printf("The process becomes the group leader: %d\n", (int)getpgrp());
            }
            break;
         case 'p':
            // -p  Печатает идентификаторы процесса, процесса-родителя и группы процессов.
            printf("Prints the process identifiers: %d\n", (int)getpid());
            printf("Prints the parent process: %d\n", (int)getppid());
            printf("Prints the process group: %d\n", (int)getpgrp());
            break;
         case 'u':
            // -u  Печатает значение ulimit
            if (getrlimit(RLIMIT_FSIZE, &rl) == -1){
               perror("getrlimit");
               break;
            } else {
               if (rl.rlim_cur == RLIM_INFINITY){
                  printf("Unlimited ulimit value\n");
               } else {
                  printf("Ulimit value: %lu\n", (unsigned long)rl.rlim_cur);
               }
            }
            break;
         case 'U':
            // -Unew_ulimit  Изменяет значение ulimit.
            if (getrlimit(RLIMIT_FSIZE, &rl) == -1) {
               perror("getrlimit");
               break;
            } else {
               char *end = NULL;
               long t = strtol(arg, &end, 10);
               if (end == arg || *end != '\0' || t < 0) {
                  fprintf(stderr, "Invalid value\n");
                  break;
               }
               rl.rlim_cur = (rlim_t)t;
               if (setrlimit(RLIMIT_FSIZE, &rl) == -1){
                  perror("setrlimit");
                  break;
               } else {
                  printf("Changed ulimit value: %lu\n", (unsigned long)rl.rlim_cur);
               }
            }
            break;
         case 'c':
            // -c  Печатает размер в байтах core-файла, который может быть создан.
            if (getrlimit(RLIMIT_CORE, &rl) == -1){
               perror("getrlimit");
               break;
            } else {
               if (rl.rlim_cur == RLIM_INFINITY){
                  printf("Unlimited core file size\n");
               } else {
                  printf("Core file size: %lu\n", (unsigned long)rl.rlim_cur);
               }
            }
            break;
         case 'C':
            // Csize  Изменяет размер core-файла
            if (getrlimit(RLIMIT_CORE, &rl) == -1){
               perror("getrlimit");
               break;
            } else {
               char *end = NULL;
               long t = strtol(arg, &end, 10);
               if (end == arg || *end != '\0' || t < 0) {
                  fprintf(stderr, "Invalid value\n");
                  break;
               }
               rl.rlim_cur = (rlim_t)t;
               if (setrlimit(RLIMIT_CORE, &rl) == -1){
                  perror("setrlimit");
               } else {
                  printf("New core file size: %lu\n", (unsigned long)rl.rlim_cur);
               }
            }
            break;
         case 'd': {
            // -d  Печатает текущую рабочую директорию
            char buf[PATH_MAX];
            if (getcwd(buf, sizeof(buf)) == NULL){
               perror("getcwd");
            } else {
               printf("Current working directory: %s\n", buf);
            }
            break;
         }
         case 'v': {
            // -v  Распечатывает переменные среды и их значения
            extern char **environ;
            for (char **e = environ; *e != NULL; e++){
               printf("%s\n", *e);
            }            
            break;
         }
         case 'V':
            // -Vname=value  Вносит новую переменную в среду или 
            // изменяет значение существующей переменной.
            if (putenv(arg) != 0){
               perror("putenv");
               break;
            } else {
               printf("Updated environment: %s\n", arg);
            }
            break;
         default:
            fprintf(stderr, "Unknown option\n");
            exit(EXIT_FAILURE);
         }
   }
   if (optind < argc && argv[optind][0] != '-') {
      printf("name argument = %s\n", argv[optind]);
   }
   exit(EXIT_SUCCESS);
}