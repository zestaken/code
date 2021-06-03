#include <stdio.h>		
#include <stdlib.h>		//exit()函数相关
#include <unistd.h>		//C 和 C++ 程序设计语言中提供对 POSIX 操作系统 API 的访问功能的头文件
#include <sys/types.h>		//Unix/Linux系统的基本系统数据类型的头文件,含有size_t,time_t,pid_t等类型
#include <sys/socket.h>		//套接字基本函数相关
#include <netinet/in.h>		//IP地址和端口相关定义，比如struct sockaddr_in等
#include <arpa/inet.h>		//inet_pton()等函数相关
#include <string.h>		//bzero()函数相关

int main(int argc, char *argv[]) {

    int clientfd; //客户端套接字
    int numbytes; //客户端接收到的套接字
    char buf[100]; //缓冲区

    //存放服务器端的地址信息
    struct sockaddr_in server_addr;
    socklen_t sin_size = (socklen_t)sizeof(struct sockaddr_in);

    //输入的参数不对
    if(argc != 3) {

        printf("args error\n");
        exit(0);
    }

    //获取客户端连接的套接字
    if( (clientfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket error.");
        exit(1);
    }

    //初始化server_addr
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET; //设置协议簇
    //从argv[1]将输入的IP转换为网络字节序存入server_addr结构体中
    if(inet_pton(AF_INET, argv[1], &server_addr.sin_addr) == 0) {
        perror("Sever IP address error!");
        exit(1);
    }
    //将转换为网络字节序的端口存入server_addr
    server_addr.sin_port = htons(atoi(argv[2]));

    //连接服务器
    if(connect(clientfd, (struct sockaddr *)&server_addr, sin_size) == -1) {
        perror("connect failed!");
        exit(1);
    }

    if((numbytes = recv(clientfd, buf, 100, 0)) == -1) {
        perror("recv error");
        exit(1);
    }
    
    buf[numbytes] = '\0';
    printf("Message from server: %s\n", buf);

    //关闭连接套接字
    close(clientfd);
}