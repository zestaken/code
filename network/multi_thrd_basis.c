#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <unistd.h>

//子线程1执行函数
void child1(void *str) {
    char *m = (char *)str;
    printf("[spid](%ld) [tgid](%d) [ptid](%ld) start\n", syscall(SYS_gettid), getpid(), (long)pthread_self());
    printf("[spid](%ld) [read](%s)\n", syscall(SYS_gettid), m);
    printf("[spid](%ld) [tgid](%d) [ptid](%ld) [func](pthread_exit)\n", syscall(SYS_gettid), getpid(), (long)pthread_self());
    pthread_exit(0);
}

//子线程2及以后执行函数
void child2(void *str) {
    char *m = (char *)str;
    pthread_detach(pthread_self());
    printf("[spid](%ld) [tgid](%d) [ptid](%ld) start\n", syscall(SYS_gettid), getpid(), (long)pthread_self());
    printf("[spid](%ld) [read](%s)\n", syscall(SYS_gettid), m);
    printf("[spid](%ld) [tgid](%d) [ptid](%ld) [func](pthread_exit)\n", syscall(SYS_gettid), getpid(), (long)pthread_self());
    pthread_exit(0);
}

int main(int argc, char *argv[]) {

    int conc_amnt;
    FILE *fp;
    pthread_t childs[conc_amnt - 1];
    char strs[conc_amnt][66]; //存储从文件中读取出的字符串的二维数组

    //获取命令行参数
    if(strcmp(argv[1], "-c") == 0) {
        if(atoi(argv[2]) > 6) {
            printf("No more than 6 threads!\n");
            return 0;
        }else {
            conc_amnt = atoi(argv[2]);
        }
    } else {
        printf("args are wrong\n");
        return 0;
    }

    printf("[spid](%ld) [tgid](%d) [ptid](%ld) start\n", syscall(SYS_gettid), getpid(), (long)pthread_self());

    //读取文件内容，存入字符串数组
    fp = fopen("td.txt", "r");
    for(int i = 0; i < conc_amnt - 1; i++) {
        fgets(strs[i], 61, fp);
        strs[i][(int)strlen(strs[i]) - 1] = '\0'; //去掉末尾换行符
     
        printf("[spid](%ld) [read](%s)\n", syscall(SYS_gettid), strs[i]);
    }

    //创建子线程
    for(int i = 0; i < conc_amnt - 1; i++) {
        if(i == 0) {
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
            pthread_create(&childs[i], &attr, (void*)child1, strs[i]);
            pthread_attr_destroy(&attr);
        } else {
            pthread_create(&childs[i], NULL, (void*)child2, strs[i]);
        }
    }

    //等待子线程执行完毕
    sleep(1);
    //关闭文件描述符
    fclose(fp);
    printf("[spid](%ld) [tgid](%d) [ptid](%ld) [func](exit)\n", syscall(SYS_gettid), getpid(), (long)pthread_self());
    exit(0);

}