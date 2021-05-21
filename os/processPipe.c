#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

char *bufin1 = "Child process 1 is sending a message!";
char *bufin2 = "Child process 2 is sending a message!";


int main() {


    int fd[2]; //文件描述符数组
    pid_t childPid;

    if(pipe(fd) == -1) {
        perror("Falied to create a pipe");
        return 1;
    }

    childPid = fork();
    if(childPid == -1) {
        perror("Failed to fork");
        return 1;
    } else if(childPid) {
        //父进程等待子进程结束
        sleep(1);
        //关闭管道写文件描述符
        close(fd[1]);
        char bufout[100];
        read(fd[0], bufout, (size_t)strlen(bufin1));
    } else {
        //要写入管道的字符串
        char *bufin = "Child process 1 is sending a message!";
        //关闭管道读文件
        close(fd[0]);
        //将字符串通过管道写入缓冲区
        write(fd[1], bufin1, (size_t)strlen(bufin1));
    }

    childPid = fork();
    if(childPid == -1) {
        perror("Failed to fork");
        return 1;
    } else if(childPid) {
        //父进程等待子进程结束
        sleep(1);
        //关闭管道写文件描述符
        close(fd[1]);
        char bufout[100];
        read(fd[0], bufout, (size_t)strlen(bufin2));
    } else {
        //要写入管道的字符串
        //关闭管道读文件
        close(fd[0]);
        //将字符串通过管道写入缓冲区
        write(fd[1], bufin2, (size_t)strlen(bufin2));
    }

    exit(0);


}
