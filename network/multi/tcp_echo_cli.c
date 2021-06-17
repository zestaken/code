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

#define MAX_CMD_STR 100

//业务逻辑处理函数
int echo_rqt(int sockfd, int pin) {
    //声明缓冲区
    char buf[MAX_CMD_STR + 9];

    //拼接文件名
    char filename1[50] = "td";
    char filename2[20] = ".txt";
    char filepin[5];
    itoa(pin, filepin, 10);
    strcat(filename1, filepin);
    strcat(filename1, filename2);

    //循环读取，每次从stdin读取一行
    while(fgets(buf, MAX_CMD_STR, filename1)) {
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

    //获取socket连接描述符
    if((connectfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket error: ");
        exit(-1);
    }

    for(int i = 1; i < ProcCount; i++) {
        pid_t pid;
        pid = fork();
        if(pid < 0) {

        } else if(pid == 0) {
            //子进程

            //获取pin值
            int pin = i;

            char filename1[20] = "stu_cli_res_";
            char filename2[10] = ".txt";
            char filepin[5];
            itoa(pin, filepin, 10);
            strcat(filename1, filepin);
            strcat(filename1, filename2);
            
            //打开文件
            int fd = open(fd, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
            printf("[cli](%d) %s is created\n", getpid(), filename1);
            //todo 写子进程启动消息到打开的文件中去

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
            printf("[cli] connfd is closed!\n");
        } else if(pid > 0) {
            //父进程
            int pin = 0;
            //连接服务器
            int res = connect(connectfd, (struct sockaddr *)&server, sin_size);
            if(res == -1) {
                printf("[cli] connect error! errno is %d\n", errno);
                return -1;
            }else if(res == 0) {
                //将ip和port转换为主机字节序，打印，以验证转换成功
                inet_ntop(PF_INET, &server.sin_addr.s_addr, ip_h, sin_size);
                printf("[srv] server[%s:%d] is connected!\n", ip_h, ntohs(port_n));

                //业务处理
                echo_rqt(connectfd);
            }

            //关闭连接套接字
            close(connectfd);
            printf("[cli] connfd is closed!\n");
        }
    }



    //关闭连接套接字
    close(connectfd);
    printf("[cli] connfd is closed!\n");

    printf("[cli] client is exiting!\n");
    return 0;
}
