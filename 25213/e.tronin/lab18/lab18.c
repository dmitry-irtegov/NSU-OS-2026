#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>

void print_file(const char *path) {
    struct stat st;

    if (lstat(path, &st) == -1) {
        perror(path);
        return;
    }

    char type;
    if(S_ISDIR(st.st_mode)) {
        type = 'd';
    }
    else if (S_ISREG(st.st_mode)) {
        type = '-';
    }
    else {
        type = '?';
    }

    char permission[10];
    permission[0] = (st.st_mode & S_IRUSR) ? 'r' : '-';
    permission[1] = (st.st_mode & S_IWUSR) ? 'w' : '-';
    permission[2] = (st.st_mode & S_IXUSR) ? 'x' : '-';

    permission[3] = (st.st_mode & S_IRGRP) ? 'r' : '-';
    permission[4] = (st.st_mode & S_IWGRP) ? 'w' : '-';
    permission[5] = (st.st_mode & S_IXGRP) ? 'x' : '-';

    permission[6] = (st.st_mode & S_IROTH) ? 'r' : '-';
    permission[7] = (st.st_mode & S_IWOTH) ? 'w' : '-';
    permission[8] = (st.st_mode & S_IXOTH) ? 'x' : '-';

    permission[9] = '\0';

    struct passwd *pw = getpwuid(st.st_uid);
    struct group *gr = getgrgid(st.st_gid);

    const char *owner = pw ? pw->pw_name : "?";
    const char *group = gr ? gr->gr_name : "?";

    char size[32];
    if(S_ISREG(st.st_mode)) {
        snprintf(size, sizeof(size), "%ld", (long)st.st_size);
    }
    else {
        size[0] = '\0';
    }


    char date[13];
    struct tm *tm_info = localtime(&st.st_mtime);
    if (tm_info != NULL) {
        strftime(date, sizeof(date), "%b %e %H:%M", tm_info);
    }
    else {
        strcpy(date, "?");
    }

    const char *filename = strrchr(path, '/');
    if (filename != NULL) {
        filename++;
    }
    else {
        filename = path;
    }

    printf("%c%-9s %3lu %-10s %-10s %8s %-15s %s\n",
               type,
               permission,
               (unsigned long)st.st_nlink,
               owner,
               group,
               size,
               date,
               filename);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s file...\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        print_file(argv[i]);
    }

    return 0;
}
