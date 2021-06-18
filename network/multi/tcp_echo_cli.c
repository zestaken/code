#include <stdio.h>		
#include <stdlib.h>		//exit()函数相关
#include <unistd.h>		//C 和 C++ 程序设计语言中提供对 POSIX 操作系统 API 的访问功能的头文件
#include <sys/types.h>		//Unix/Linux系统的基本系统数据类型的头文件,含有size_t,time_t,pid_t等类型
#include <sys/socket.h>		//套接字基本函数相关
#include <netinet/in.h>		//IP地址和端口相关定义，比如struct sockaddr_in等
#include <arpa/inet.h>		//inet_pton()等函数相关
#include <string.h>		//bzero()函数相关
#include <errno.h>      //错误处理相关

#define MAX_CMD_STR 100

//业务逻辑处理函数
int echo_rqt(int sockfd) {
    //声明缓冲区
    char buf1[MAX_CMD_STR + 1];

    //循环读取，每次从stdin读取一行
    while(fgets(buf1, MAX_CMD_STR, stdin)) {
        //如果开头字符为exit退出程序
        if(strncmp(buf1, "exit", 4) == 0) {
            return 0;
        }
        //获取读取的字符串的长度
        int len_h = strnlen(buf1, MAX_CMD_STR);
        //将最后一位字符由\n改为\0
        buf1[len_h - 1] = '\0';
        
        //将len_h转换为网络字节序，发送给服务器
        int len_n = htonl(len_h);
        write(sockfd, &len_n, sizeof(len_n));
        //随后将按指定长度发送缓存数据
        write(sockfd, buf1, len_h);

         //读取服务器数据
        int len_n1, len_h1, res = 0, res1 = 0;
        char buf2[MAX_CMD_STR + 1];
        read(sockfd, &len_n1, sizeof(len_n1));
        len_h1 = ntohl(len_n1);
        char *tmp = buf2;
        printf("读取服务器数据之前\n");
        printf("len_h1: %d\n", len_h1);
        while(res < len_h1) {
            res1 = read(sockfd, tmp, len_h1);
            tmp = tmp + res1;
            res += res1;
        }
        printf("读取完服务器数据，打印之前\n");
        printf("[echo_rep] %s\n", buf2);
    }
 
    return 0;
}

int main(int argc, char *argv[]) {

    int connectfd;

    //检查用户是否正确输入参数
    if(argc != 3) {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        exit(-1);
    }

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

    //连接服务器
    int res = connect(connectfd, (struct sockaddr *)&server, sin_size);
    if(res == -1) {
        printf("[cli] connect error! errno is %d\n", errno);
        return -1;
    }else if(res == 0) {
        //将ip和port转换为主机字节序，打印，以验证转换成功
        inet_ntop(PF_INET, &server.sin_addr.s_addr, ip_h, sin_size);
        printf("[cli] server[%s:%d] is connected!\n", ip_h, ntohs(port_n));

        //业务处理
        echo_rqt(connectfd);
    }


    //关闭连接套接字
    close(connectfd);
    printf("[cli] connfd is closed!\n");

    printf("[cli] client is exiting!\n");
    return 0;
}
