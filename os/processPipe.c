#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() {

    int fd[2]; //文件描述符数组
    pid_t childPid1, childPid2;

    if(pipe(fd) == -1) {
        perror("Falied to create a pipe");
        return 1;
    }

    childPid1 = fork();
    if(childPid1 == -1) {
        perror("Failed to fork");
        return 1;
    } else if(childPid1 > 0) {
        //父进程等待子进程结束
        waitpid(childPid1,NULL,0);
        //关闭管道写文件描述符
        close(fd[1]);
        char bufout1[100];
        read(fd[0], bufout1, (size_t)strlen(bufin1));
        printf("%s\n", bufout1);

        childPid2 = fork();
        if(childPid2 == -1) {
            perror("Failed to fork");
            return 1;
        } else if(childPid2 > 0) {
            //父进程等待第二个子进程结束
            waitpid(childPid2, NULL, 0);
            //关闭管道写文件描述符
            close(fd[1]);
            int bufsize = (size_t)(strlen(bufin2) + 1);
            char bufout2[bufsize];
            read(fd[0], bufout2, bufsize);
            read(fd[0], bufout2, bufsize);
            printf("%s\n", bufout2);
            exit(0);
        } else {
            //要写入管道的字符串
            //关闭管道读文件
            close(fd[0]);
            //将字符串通过管道写入缓冲区
            write(fd[1], bufin2, (size_t)strlen(bufin2));
            exit(0);
        }
    } else {
        //第一个子进程
        //关闭管道读文件
        close(fd[0]);
        //将字符串通过管道写入缓冲区
        write(fd[1], bufin1, (size_t)strlen(bufin1));
        exit(0);
    }
}
