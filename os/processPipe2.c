#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

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
        //父进程等待第一个子进程结束
        waitpid(childPid, NULL, 0);

        childPid = fork();
        if(childPid == -1) {
            perror("Failed to fork");
            return 1;
        } else if(childPid) {
            //父进程等待第二个子进程结束
            waitpid(childPid, NULL, 0);
            close(fd[1]);
            int bufsize2 = 2 * (1 + strlen("Child process 2 is sending a message!"));
            char bufout2[bufsize2];

            read(fd[0], bufout2, bufsize2);
            printf("%s\n", bufout2);
        } else {
            //第二个子进程
            char *bufin2 = "Child process 2 is sending a message!";
            //要写入管道的字符串
            //关闭管道读文件
            close(fd[0]);
            //将字符串通过管道写入缓冲区
            write(fd[1], bufin2, (size_t)(1 + strlen(bufin2)));
            exit(0);
        }
    } else {
        //第一个子进程
        //要写入管道的字符串
        char *bufin1 = "Child process 1 is sending a message! ";
        //关闭管道读文件
        close(fd[0]);
        //将字符串通过管道写入缓冲区
        write(fd[1], bufin1, (size_t)strlen(bufin1));
        exit(0);
    }

}