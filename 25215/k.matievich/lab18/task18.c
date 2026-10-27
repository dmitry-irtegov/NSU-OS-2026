#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <libgen.h>
#include <time.h> 

typedef struct myfileinfo {
    char* perms;
    char* ownername;
    char* groupname;
    char* size;
    char* modtime;
    char* filename;
    unsigned long nlink;
    char type;
} myfileinfo;


myfileinfo* getinfo(struct stat st, char* filepath) {

    myfileinfo* finfo = (myfileinfo*)malloc(sizeof(myfileinfo));
    finfo->perms = (char*)malloc(10*sizeof(char));
    finfo->groupname = (char*)malloc(256*sizeof(char));
    finfo->ownername = (char*)malloc(256*sizeof(char));
    finfo->filename = (char*)malloc(256*sizeof(char));
    finfo->modtime = (char*)malloc(32*sizeof(char));
    finfo->size = (char*)malloc(32*sizeof(char));

    finfo->type = '?';
    if (S_ISREG(st.st_mode)) {
        finfo->type = '-';
    } else if (S_ISDIR(st.st_mode)) {
        finfo->type = 'd';
    }   

    finfo->perms[0] = (st.st_mode & S_IRUSR) ? 'r' : '-';
    finfo->perms[1] = (st.st_mode & S_IWUSR) ? 'w' : '-';
    finfo->perms[2] = (st.st_mode & S_IXUSR) ? 'x' : '-';
    
    finfo->perms[3] = (st.st_mode & S_IRGRP) ? 'r' : '-';
    finfo->perms[4] = (st.st_mode & S_IWGRP) ? 'w' : '-';
    finfo->perms[5] = (st.st_mode & S_IXGRP) ? 'x' : '-';
    
    finfo->perms[6] = (st.st_mode & S_IROTH) ? 'r' : '-';
    finfo->perms[7] = (st.st_mode & S_IWOTH) ? 'w' : '-';
    finfo->perms[8] = (st.st_mode & S_IXOTH) ? 'x' : '-';
    finfo->perms[9] = '\0';

    finfo->nlink = st.st_nlink;

    struct passwd* pw = getpwuid(st.st_uid);
    if (pw) {
        strncpy(finfo->ownername, pw->pw_name, 255);
    } else {
        snprintf(finfo->ownername, 256, "%d", st.st_uid);
    }
    struct group* gr = getgrgid(st.st_gid);
    if (gr) {
        strncpy(finfo->groupname, gr->gr_name, 255);
    } else {
        snprintf(finfo->groupname, 256, "%d", st.st_gid);
    }

    char* path_copy = strdup(filepath);
    char* bname = basename(path_copy);
    strncpy(finfo->filename, bname, 255);
    free(path_copy);

    char size[32] = "";
    if (finfo->type == '-') {
        snprintf(size, sizeof(size), "%lu", (unsigned long)st.st_size);
    }
    strcpy(finfo->size, size);
    


    struct tm *sp;
    time_t modtime = st.st_mtime;
    sp = localtime(&modtime);
    snprintf(finfo->modtime, 32, "%02d/%02d/%02d", sp->tm_mday, sp->tm_mon + 1, sp->tm_year + 1900);

    return finfo;
}

int main(int argc, char *argv[]) {
    
    
    if (argc == 1) {
        struct stat st;
        char curdir[PATH_MAX];
        getcwd(curdir, PATH_MAX);
        lstat(curdir, &st);
        myfileinfo* finfo = getinfo(st, curdir);

        printf("%c%s %lu %s %s %s %s %s\n", finfo->type, finfo->perms, finfo->nlink, finfo->ownername, finfo->groupname, "", finfo->modtime, finfo->filename);

        free(finfo->filename);
        free(finfo->groupname);
        free(finfo->ownername);
        free(finfo->perms);
        free(finfo->size);
        free(finfo->modtime);
        free(finfo);
        return 0;
    }

    int cnt = 0;
    int maxuname = 0;
    int maxgname = 0;
    int maxsize = 0;

    myfileinfo** finfos = (myfileinfo**)malloc((argc-1)*sizeof(myfileinfo*));
    for (int i = 1; i < argc; i++) {
        struct stat st;
        if (lstat(argv[i], &st) == -1) {
            write(2, "task18: cannot accsess '", strlen("task18: cannot accsess '"));
            write(2, argv[i], strlen(argv[i]));
            write(2, "': No such file or directory\n", strlen("': No such file or directory\n"));
        } else {
            finfos[cnt++] = getinfo(st, argv[i]);

            int uname = strlen(finfos[cnt-1]->ownername);
            int gname = strlen(finfos[cnt-1]->groupname);

            if (uname > maxuname) maxuname = uname;
            if (gname > maxgname) maxgname = gname;
            if (strlen(finfos[cnt-1]->size) > maxsize) maxsize = strlen(finfos[cnt-1]->size);
        }

    }

    for (int i = 0; i < cnt; i++) {

        printf("%c%s %3lu %-*s %-*s %-*s %10s %s\n",
               finfos[i]->type,
               finfos[i]->perms,
               finfos[i]->nlink,
               maxuname, finfos[i]->ownername,
               maxgname, finfos[i]->groupname,
               maxsize, finfos[i]->size,
               finfos[i]->modtime,
               finfos[i]->filename);
    }




    for (int i = 0; i < cnt; i++) {
        free(finfos[i]->filename);
        free(finfos[i]->groupname);
        free(finfos[i]->ownername);
        free(finfos[i]->perms);
        free(finfos[i]->modtime);
        free(finfos[i]->size);
        free(finfos[i]);
    }
    free(finfos);
    return 0;
}
