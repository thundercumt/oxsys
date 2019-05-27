#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/param.h>
#include <pwd.h>
#include <sys/types.h>
#include <grp.h>
#include <time.h>

static void usage() {
    printf("usage: oxls path\n");
}

static void list(const char*);
static void show_entry(dirent *);
static void show_user_name(uid_t);
static void show_group_name(gid_t);
static void show_time(long);

int main(int argc, const char** argv) {
    if (argc < 2) {
        usage();
        exit(1);
    }
    
    char cwd[MAXPATHLEN] = {0};
    if (getcwd(cwd, MAXPATHLEN) == NULL) {
        perror("getcwd failed");
        exit(1);
    }
    list(argv[argc-1]);

    if (chdir(cwd) == -1) {
        perror("restore cwd failed");
    }
    return 0;
}

static void list(const char* path) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("failed to open directory");
        exit(1);
    }

    if (chdir(path) == -1) {
        perror("failed to cd");
        exit(1);
    }

    dirent *pde;
    while ((pde=readdir(dir)) != NULL) {
        show_entry(pde);
    }

    if (closedir(dir)==-1) {
        perror("failed to close directory");
    }
}

static void show_mode(mode_t m) {
    char cmode[11] = {0};
    strncpy(cmode, "----------", 10);

    if (S_ISDIR(m)) cmode[0] = 'd';
    if (S_ISREG(m)) cmode[0] = 'p';
    if (S_ISCHR(m)) cmode[0] = 'c';
    if (S_ISBLK(m)) cmode[0] = 'b';
    if (S_ISLNK(m)) cmode[0] = 'l';

    if (m & S_IRUSR) cmode[1] = 'r';
    if (m & S_IWUSR) cmode[2] = 'w';
    if (m & S_IXUSR) cmode[3] = 'x';

    if (m & S_IRGRP) cmode[4] = 'r';
    if (m & S_IWGRP) cmode[5] = 'w';
    if (m & S_IXGRP) cmode[6] = 'x';

    if (m & S_IROTH) cmode[7] = 'r';
    if (m & S_IWOTH) cmode[8] = 'w';
    if (m & S_IXOTH) cmode[9] = 'x';

    printf("%.*s", 10, cmode);
}

static void show_user_name(uid_t uid) {
    struct passwd *ppwd;
    if ((ppwd = getpwuid(uid)) == NULL) {
        perror("failed to get user");
        return;
    }

    printf("%-8s ", ppwd->pw_name);
}

static void show_group_name(gid_t gid) {
    struct group *pgrp;
    if ((pgrp = getgrgid(gid)) == NULL) {
        perror("failed to get group");
        return;
    }

    printf("%-8s ", pgrp->gr_name);
}

static void show_time(long secs) {
    struct tm *ptm = localtime(&secs);
    printf("%02d-%02d %02d:%02d ", ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min);
}

static void show_entry(dirent *p) {
    struct stat st;
    if (stat(p->d_name, &st) == -1) {
        perror(p->d_name);
        return;
    }

    show_mode(st.st_mode);

    printf("%4d ", st.st_nlink);

    show_user_name(st.st_uid);

    show_group_name(st.st_gid);

    printf("%6lld  ", st.st_size);

    show_time(st.st_ctimespec.tv_sec);

    printf("%.*s", p->d_namlen, p->d_name);
    printf("\n");
}