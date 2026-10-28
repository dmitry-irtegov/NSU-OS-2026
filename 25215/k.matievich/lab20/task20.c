#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <fnmatch.h>
#include <limits.h>
#include <sys/stat.h>


void fsearch(int* cnt, const char* pat, char* curpath, char* curdir) {

    DIR* dir = opendir(curdir);
    if (dir == NULL) {
        return;
    }
    struct dirent* curfile = readdir(dir);

    while(curfile != NULL) {

        if (strcmp(curfile->d_name, ".") == 0 || strcmp(curfile->d_name, "..") == 0) {
            curfile = readdir(dir);
            continue;
        }

        char ncurpath[PATH_MAX];
        if (strlen(curpath) > 0) {
            snprintf(ncurpath, sizeof(ncurpath), "%s/%s", curpath, curfile->d_name);
        } else {
            snprintf(ncurpath, sizeof(ncurpath), "%s", curfile->d_name);
        }

        if (fnmatch(pat, ncurpath, FNM_PATHNAME) == 0) {
            write(1, ncurpath, strlen(ncurpath));
            write(1, "\n", 1);
            (*cnt)++;
        }

        struct stat st;
        char ncurdir[PATH_MAX];
        snprintf(ncurdir, sizeof(ncurdir), "%s/%s", curdir, curfile->d_name);

        stat(ncurdir, &st);
        if (S_ISDIR(st.st_mode)) {
            fsearch(cnt, pat, ncurpath, ncurdir);
        }


        curfile = readdir(dir);
    }
    closedir(dir);

    return;
}


int main() {
    write(1, "Введите шаблон имени файла:\n", strlen("Введите шаблон имени файла:\n"));
    char buf[BUFSIZ];
    ssize_t bytesread = read(0, buf, BUFSIZ);
    
    if (buf[bytesread - 1] == '\n') {
        buf[bytesread - 1] = '\0';
    } else {
        buf[bytesread] = '\0';
    }

    char curdir[PATH_MAX];
    getcwd(curdir, PATH_MAX);
    int cnt = 0;

    fsearch(&cnt, buf, "", curdir);

    if (cnt == 0) {
        write(1, buf, strlen(buf));
        write(1, "\n", 1);
    }
    
    return 0;
}