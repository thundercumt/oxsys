#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

static void usage() {
    printf("usage: oxtail file\n");
}

static void tail(const char*);

int main(int argc, const char** argv) {
    if (argc < 2) {
        usage();
        exit(1);
    }
    tail(argv[argc-1]);
    return 0;
}

static bool keep_on = true;

static void on_interupt(int s) {
    if (SIGINT == s) {
        keep_on = false;
        #ifdef DEBUG
        printf("received SIGINT\n");
        #endif
    }
}

void tail(const char* path) {
    #define SZ 256
    char buf[SZ] = {0};
    FILE *fp = fopen(path, "r");

    if (!fp) {
        perror("failed to open file");
        return;
    }

    char *p;

    if (fseek(fp, 0L, SEEK_END) == -1) {
        perror("failed to seek to end of file");
        return;
    }

    signal(SIGINT, on_interupt);

    while(keep_on) {
        int c = fgetc(fp);
        if (c != EOF) {
            ungetc(c, fp);
            p = fgets(buf, SZ, fp);
            if (p) {
                printf("%s", buf);
                fflush(stdout);
            }
        }
        else {
            if (feof(fp)) {
                clearerr(fp);
            }
            sleep(1);
        }
    }
    #ifdef DEBUG
    printf("should not loop %d\n", keep_on);
    #endif
    fclose(fp);
}