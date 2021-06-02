#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

char *bufin1 = "Child process 1 is sending a message!";
char *bufin2 = "Child process 2 is sending a message!";

int main() {

    int fd[2]; //文件描述符数组
    pid_t childPid1, childPid2;

    if(pipe(fd) == -1) {
        perror("Failed to create a pipe");
        return 1;
    }

    //父进程创建第一个子进程
    childPid1 = fork();
    if (childPid1 == -1) {
        perror("Failed to fork");
        return 1;
    } else if (childPid1 > 0) {
        //父进程等待子进程结束
        waitpid(childPid1,NULL,0);

        childPid2 = fork();
        if(childPid2 == -1) {
            perror("Failed to fork");
            return 1;
        } else if (childPid2 > 0) {
            //父进程等待第二个子进程结束
            waitpid(childPid2, NULL, 0);
	    printf("parent pid[%d] ppid[%d]\n", getpid(), getppid());

            //关闭管道写文件描述符
            close(fd[1]);

            int bufsize = (size_t)(strlen(bufin1) + 1);
            char bufout1[bufsize];
            char bufout2[bufsize];
	    memset((void*)bufout1,0,sizeof(bufout1));
	    memset((void*)bufout2, 0, sizeof(bufout2));
	    int num1 = 0;
	    int num2 = 0;
	    
            num1 = read(fd[0], bufout1, (size_t)strlen(bufin1));
            num2 = read(fd[0], bufout2, bufsize);

            printf("childmes1: %s num1: %d\n", bufout1, num1);
            printf("childmes2: %s num2: %d\n", bufout2, num2);

	    exit(0);
        } else {
		//第二个子进程
            //关闭管道读文件
            close(fd[0]);
            //将字符串通过管道写入缓冲区
            write(fd[1], bufin2, (size_t)strlen(bufin2));
	    printf("child2 pid[%d] ppid[%d]\n", getpid(), getppid());
            exit(0);
        }
    } else {
        //第一个子进程
        //关闭管道读文件
        close(fd[0]);
        //将字符串通过管道写入缓冲区
        write(fd[1], bufin1, (size_t)strlen(bufin1));
	printf("child1 pid[%d] ppid[%d]\n", getpid(), getppid());
        exit(0);
    }

}
