#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>



int conc_amnt = 0;
pid_t pid;
void pa_handle_sigint(int sig) {
    printf("[pid](%d): SIGINT is handled in pa_handle_sigint()\n", getpid());
    printf("[pid](%d): [func](exit) with [conc_amnt](%d)\n", getpid(), conc_amnt);
    exit(0);
}

void pa_handle_sigchld(int sig) {
    pid_t pid;
    printf("[pid](%d): SIGCHLD is handled in pa_handle_sigchld\n", getpid());
    while((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
        printf("[pid](%d): [cpid](%d) is waited in pa_handle_sigchld\n", getpid(), pid);
        conc_amnt--;
    }
}

int main(int argc, char *argv[]) {

    if(strcmp("-c", argv[1]) == 0) {
        if(atoi(argv[2]) > 6) {
            printf("No more than 6 process!\n");
            return 0;
        }else {
            conc_amnt = atoi(argv[2]);
        }
    }

    pid_t pids[conc_amnt]; 

    //初始化
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    sigemptyset(&act.sa_mask);
    act.sa_handler = pa_handle_sigint;

    //安装信号处理器
    if(sigaction(SIGINT, &act, NULL) == -1) {
        printf("error\n");
    }

    act.sa_handler = pa_handle_sigchld;
    if(sigaction(SIGCHLD, &act, NULL) == -1) {
        printf("error\n");
    }

    printf("[pid](%d): start\n", getpid());

    for(int i = 0; i < conc_amnt - 1; i++) {
        pid = fork();
        if(pid < 0) {
            printf("error");
        }else if(pid == 0) {
            printf("[pid](%d): start\n", getpid());
            while(1) {
                pause();
            }

        }else {
            pids[i] = pid;
            continue;
        }
    }           

    sleep(1);
    //向子进程发送中断信号
    for(int i = 0; i < conc_amnt - 1; i++) {
        kill(pids[i], 2);
    } 

    //等待信号
    while(conc_amnt > 1) {
        printf("[pid](%d): [func](pause) with [conc_amnt](%d)\n", getpid(), conc_amnt);
        pause();
    }

    pause();
}