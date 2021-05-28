#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

static void sigHandler(int sig) {
    if(sig == SIGINT) {
        printf("[pid](%d): [sig](SIGINT) is captured\n", getpid());
        printf("[pid](%d): exit\n", getpid());
        exit(0);
    }

    if(sig == SIGUSR1) {
        printf("[pid](%d): [sig](SIGUSR1) is catured\n", getpid());
    }

    if(sig == SIGUSR2) {
        printf("[pid](%d): [sig](SIGUSR2) is captured\n", getpid());
    }
}


int main() {

    printf("[pid](%d): start\n", getpid());
    // if(signal(SIGINT, sigHandler) == SIG_ERR) {
    //     printf("error\n");
    // }

    // if(signal(SIGUSR1, sigHandler) == SIG_ERR) {
    //     printf("error\n");
    // }

    // if(signal(SIGUSR2, sigHandler) == SIG_ERR) {
    //     printf("error\n");
    // }

    //初始化
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    sigemptyset(&act.sa_mask);
    act.sa_handler = sigHandler;

    //安装信号处理器
    if(sigaction(SIGINT, &act, NULL) == -1) {
        printf("error\n");
    }

    if(sigaction(SIGUSR1, &act, NULL) == -1) {
        printf("error\n");
    }

    if(sigaction(SIGUSR2, &act, NULL) == -1) {
        printf("error\n");
    }

    //等待信号
    while(1){
        pause();
    }
}
