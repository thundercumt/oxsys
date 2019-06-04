#include <unistd.h>
#include <stdio.h>
#include <sys/param.h>

void usage() {
    printf("usage: oxcd path\n");
}

void pwd() {
    char buf[MAXPATHLEN] = {0};
    char *ptr = NULL;
    if ((ptr = getcwd(buf, MAXPATHLEN)) == NULL) {
        perror("getcwd");
        return;
    }
    printf("%s\n", ptr);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        usage();
        return 1;
    }

    if (chdir(argv[1]) == -1) {
        perror(argv[1]);
        return 1;
    }

    #ifdef DEBUG
    pwd();
    #endif

    return 0;
}