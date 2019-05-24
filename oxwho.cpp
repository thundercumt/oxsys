#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <utmpx.h>

static void show_info(struct utmpx*);
static void show_time(struct timeval*);

int main() {
    struct utmpx *p = getutxent();

    while(p) {
        show_info(p);
        p = getutxent();
    }

    endutxent();
    return 0;
}

void show_info(struct utmpx *p) {
    if (p->ut_type != USER_PROCESS) return;

    #ifdef DEBUG
    printf("% - 10d", p->ut_type);
    printf(" ");
    #endif
    printf("%-8.9s", p->ut_user);
    printf(" ");
    printf("%-8.9s", p->ut_line);
    printf(" ");
    show_time(&p->ut_tv);
}

void show_time(struct timeval *ptv) {
    struct tm* ptm = localtime(&ptv->tv_sec);
    char str_tm[40];
    long millies;
    strftime(str_tm, sizeof(str_tm), "%Y-%m-%d %H:%M:%S", ptm);
    millies = ptv->tv_usec / 1000;
    printf("%s.%03ld\n", str_tm, millies);
}