#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int procCount = 1, procMax;
int fd1_td1, fd2_td1, fd1_td2, fd2_td2;
size_t count = 5;
char buffer[5];
pid_t pid;


// int createChild() {
//     if(procCount > procMax) {
//         return 0;
//     }
//     procCount++;
//     pid = fork();
//     if(pid < 0) {
//         printf("error");
//     }else if(pid == 0) {
//         printf("[pid](%d) [ppid](%d): start\n", getpid(), getppid());
//         read(fd1_td1, buffer, count);
//         printf("[pid](%d) [ppid](%d): [read](%s) by [fd1_td1](%d)\n", getpid(), getppid(), buffer, fd1_td1);
//         fd2_td1 = open("td1.txt", O_RDONLY);
//         printf("[pid](%d) [ppid](%d): [fd2_td1](%d) is got\n", getpid(), getppid(), fd2_td1);
//         read(fd2_td1, buffer, count);
//         printf("[pid](%d) [ppid](%d): [read](%s) by [fd2_td1](%d)\n", getpid(), getppid(), buffer, fd2_td1);
//         close(fd2_td1);
//         close(fd1_td1);
//         printf("[pid](%d) [ppid](%d): exit\n", getpid(), getppid());
//         exit(0);
//     }else {
//         printf("[pid](%d) [ppid](%d): startParen\n", getpid(), pid);
//         createChild();
//     }
// }

int main(int argc, char *argv[]) {

    if(strcmp("-c", argv[1]) == 0) {
        if(atoi(argv[2]) > 6) {
            printf("No more than 6 process!\n");
            return 0;
        }else {
            procMax = atoi(argv[2]);
        }
    }
    //启动
    printf("[pid](%d) [ppid](%d): start\n", getpid(), getppid());

    fd1_td1 = open("td1.txt", O_RDONLY);
    printf("[pid](%d) [ppid](%d): [fd1_td1](%d) is got\n",getpid(),getppid(), fd1_td1);

    
    for(int i = 0; i < procMax - 1; i++) {
        pid = fork();
        if(pid < 0) {
            printf("error");
        }else if(pid == 0) {
            printf("[pid](%d) [ppid](%d): start\n", getpid(), getppid());
            read(fd1_td1, buffer, count);
            printf("[pid](%d) [ppid](%d): [read](%s) by [fd1_td1](%d)\n", getpid(), getppid(), buffer, fd1_td1);
            fd2_td1 = open("td1.txt", O_RDONLY);
            printf("[pid](%d) [ppid](%d): [fd2_td1](%d) is got\n", getpid(), getppid(), fd2_td1);
            read(fd2_td1, buffer, count);
            printf("[pid](%d) [ppid](%d): [read](%s) by [fd2_td1](%d)\n", getpid(), getppid(), buffer, fd2_td1);
            close(fd2_td1);
            close(fd1_td1);
            printf("[pid](%d) [ppid](%d): exit\n", getpid(), getppid());
            exit(0);
        }else {
            continue;
        }
    }           
        sleep(2);
        read(fd1_td1, buffer, count);
        printf("[pid](%d) [ppid](%d): [read](%s) by [fd1_td1](%d)\n", getpid(), getppid(), buffer, fd1_td1);
        close(fd1_td1);
        fd1_td2 = open("td2.txt", O_RDONLY);
        printf("[pid](%d) [ppid](%d): [fd1_td2](%d) is got\n", getpid(), getppid(), fd1_td2);
        read(fd1_td2, buffer, count);
        printf("[pid](%d) [ppid](%d): [read](%s) by [fd1_td2](%d)\n", getpid(), getppid(), buffer, fd1_td2);
        close(fd1_td2);
        printf("[pid](%d) [ppid](%d): exit\n", getpid(), getppid());
        exit(0);
}
