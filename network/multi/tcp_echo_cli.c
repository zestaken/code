#include <stdio.h>		
#include <stdlib.h>		//exit()函数相关
#include <unistd.h>		//C 和 C++ 程序设计语言中提供对 POSIX 操作系统 API 的访问功能的头文件
#include <sys/types.h>		//Unix/Linux系统的基本系统数据类型的头文件,含有size_t,time_t,pid_t等类型
#include <sys/socket.h>		//套接字基本函数相关
#include <netinet/in.h>		//IP地址和端口相关定义，比如struct sockaddr_in等
#include <arpa/inet.h>		//inet_pton()等函数相关
#include <string.h>		//bzero()函数相关
#include <errno.h>      //错误处理相关
#include <fcntl.h> //文件读写相关
#include <stdarg.h>

#define MAX_CMD_STR 100
#define bprintf(fp, format, ...) \
    if(fp == NULL){printf(format, ##_VA_ARGS_);} \
    else{printf(format, ##_VA_ARGS_); \
            fprintf(fp, format, ##_VA_ARGS_);fflush(fp);}

//业务逻辑处理函数
int echo_rqt(int sockfd, int pin) {
    //声明缓冲区
    char buf[MAX_CMD_STR + 9];

    //拼接文件名
    char filename[20];
    sprintf(filename, "stu_cli_res_%d.txt", pin);

    //循环读取，每次从stdin读取一行
    while(fgets(buf, MAX_CMD_STR, filename)) {
        //如果开头字符为exit退出程序
        if(strncmp(buf, "exit", 4) == 0) {
            return 0;
        }
        //获取读取的字符串的长度
        int len = strnlen(buf, MAX_CMD_STR);
        //将最后一位字符由\n改为\0
        buf[len - 1] = '\0';
        int len_h = len + 8;
        
        //将len_h转换为网络字节序，发送给服务器
        int len_n = htons(len_h);
        int pin_n = htons(pin);
        //先发送pin和长度
        write(sockfd, &pin, sizeof(pin_n));
        write(sockfd, &len_n, sizeof(len_n));
        //随后将按指定长度发送缓存数据
        write(sockfd, buf, len_h);

        //读取服务器数据
        int res = 0;
        while(res < len_h) {
            res += read(sockfd, buf, len_h);
        }

    }

    return 0;
}

int main(int argc, char *argv[]) {

    int connectfd;
    int ProcCount = 0;

    //检查用户是否正确输入参数
    if(argc != 4) {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        exit(-1);
    }

    //获取进程数
    ProcCount = atoi(argv[3]);

    //初始化服务器socket地址
    char ip_h[20];
    int port_n;
    struct sockaddr_in server;
    socklen_t sin_size = (socklen_t)(sizeof(struct sockaddr_in));
    bzero(&server, sizeof(server));
    server.sin_family = PF_INET;
    port_n = htons(atoi(argv[2]));
    server.sin_port = port_n;
    if(inet_pton(PF_INET, argv[1], &server.sin_addr.s_addr) == 0) {
        perror("Sever IP address error: ");
        exit(-1);
    }

    for(int i = 1; i < ProcCount; i++) {
        pid_t pid;
        pid = fork();
        if(pid < 0) {
            //fork出错提示
        } else if(pid == 0) {
            //子进程

            //获取pin值
            int pin = i;
            char filename[20];
            sprintf(filename, "stu_cli_res_%d.txt", pin);
            
            //打开文件,如果不存在则创建
            FILE *fp = fopen(filename, "a");
            printf("[cli](%d) %s is created\n", getpid(), filename);
            //写子进程启动消息到打开的文件中去
            // char message[30];
            bprintf(fp, "[cli](%d) child process %d is created!", getpid(), pin);
            // sprintf(message, "[cli](%d) child process %d is created!",  getpid(), pin);
            // write(fp, message, sizeof("[cli](%d) child process %d is created!") + 1);

            //获取socket连接描述符
            if((connectfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
                perror("socket error: ");
                exit(-1);
            }
            //连接服务器
            int res = connect(connectfd, (struct sockaddr *)&server, sin_size);
            if(res == -1) {
                printf("[cli](%d) connect error! errno is %d\n", getpid(), errno);
                return -1;
            }else if(res == 0) {
                //将ip和port转换为主机字节序，打印，以验证转换成功
                inet_ntop(PF_INET, &server.sin_addr.s_addr, ip_h, sin_size);
                printf("[cli](%d) server[%s:%d] is connected!\n", getpid(), ip_h, ntohs(port_n));

                //业务处理
                echo_rqt(connectfd, pin);
            }

            //关闭连接套接字
            close(connectfd);
            // char message3[30];
            // char message4[45];
            // sprintf(message3, "[cli](%d) connfd is closed!", getpid());
            // sprintf(message4, "[cli](%d) parent process is going to exit!", getpid());
            // int len3 = strlen(message3);
            // int len4 = strlen(message4);
            // write(fp, message3, len3);
            // write(fp, message4, len4);
            bprintf(fp, "[cli](%d) connfd is closed!", getpid());
            bprintf(fp, "[cli](%d) parent process is going to exit!", getpid());

            //关闭文件
            fclose(fp);
            printf("[cli](%d) stu_cli_res_%d.txt is closed!", getpid(), pin);

            exit(0);
        } else if(pid > 0) {
            //父进程
            int pin = 0;      

            char filename[20];
            sprintf(filename, "stu_cli_res_%d.txt", pin);
            //打开文件,如果不存在则创建
            FILE *fp = fopen(filename, "a");
            printf("[cli](%d) %s is created\n", getpid(), filename);
            //写子进程启动消息到打开的文件中去
            // char message[40];
            // sprintf(message, "[cli](%d) child process %d is created!",  getpid(), pin);
            // int len = strlen(message);
            // write(fp, message, len);
            bprintf(fp, "[cli](%d) child process %d is created!", getpid(), pin);

            //获取socket连接描述符
            if((connectfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
                perror("socket error: ");
                exit(-1);
            }
            //连接服务器
            int res = connect(connectfd, (struct sockaddr *)&server, sin_size);
            if(res == -1) {
                printf("[cli] connect error! errno is %d\n", errno);
                return -1;
            }else if(res == 0) {
                //将ip和port转换为主机字节序，写入到对应文件
                inet_ntop(PF_INET, &server.sin_addr.s_addr, ip_h, sin_size);
                // char message2[40];
                // sprintf(message2,"[cli](%d) server[%s:%d] is connected!", getpid(), ip_h, ntohs(port_n));
                // int len2 = strlen(message2);
                // write(fp, message2, len2);
                bprintf(fp, "[cli](%d) server[%s:%d] is connected!", getpid(), ip_h, ntohs(port_n));

                //业务处理
                echo_rqt(connectfd, pin);
            }

            //关闭连接套接字
            close(connectfd);
            // char message3[30];
            // char message4[45];
            // sprintf(message3, "[cli](%d) connfd is closed!", getpid());
            // sprintf(message4, "[cli](%d) parent process is going to exit!", getpid());
            // int len3 = strlen(message3);
            // int len4 = strlen(message4);
            // write(fp, message3, len3);
            // write(fp, message4, len4);
            bprintf(fp, "[cli](%d) connfd is closed!", getpid());
            bprintf(fp, "[cli](%d) parent process is going to exit!", getpid());

            //关闭文件
            fclose(fp);
            printf("[cli](%d) stu_cli_res_%d.txt is closed!", getpid(), pin);
            exit(0);
        }
    }

}
