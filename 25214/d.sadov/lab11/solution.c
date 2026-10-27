#include <unistd.h>

extern char **environ;

void execvpe(const char *file, char *const argv[], char *const envp[]) {
    char **saved_environ = environ;
    environ = (char **)envp;

   execvp(file, argv); 

   int saved_errno = errno;
   environ = saved_environ;
   errno = saved_errno;
    
   return -1;
}

int main() {
    
}
