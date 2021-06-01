#include <stdio.h>		
#include <stdlib.h>		//exit()函数相关
#include <unistd.h>		//C 和 C++ 程序设计语言中提供对 POSIX 操作系统 API 的访问功能的头文件
#include <sys/types.h>		//Unix/Linux系统的基本系统数据类型的头文件,含有size_t,time_t,pid_t等类型
#include <sys/socket.h>		//套接字基本函数相关
#include <netinet/in.h>		//IP地址和端口相关定义，比如struct sockaddr_in等
#include <arpa/inet.h>		//inet_pton()等函数相关
#include <string.h>		//bzero()函数相关

int main(int argc, char *argv[]) {
    
    //定义监听套接字和连接套接字
    int listenfd, connectfd;
    struct sockaddr_in server, client;
    size_t sin_size = (size_t)(sizeof(struct sockaddr_in));

    if( (listenfd = socket(PF_INET, SOCK_STREAM,0) == -1) {
        perror("Create socket failed!");
        exit(-1);
    }

    bzero(&server, sizeof(server));
    server.sin_family = PF_INET;
    server.sin_port = 12345;
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if( (bind(listenfd, (struct sockaddr_in *)&server, sin_size)) == -1) {
        perror("Bind error.");
        exit(-1);
    }

    if( (listen(listenfd, BACKLOG)) == -1) {
        perror("Listen error.");
        exit(-1);
    }
    
    while(1) {
        if( (connectfd = accept(listenfd, (struct sockaddr_in *)&client, sin_size)) == -1) {
            perror("Accept error.");
            exit(-1);
        }
        printf("You get a connectiong from %s\n", inet_ntoa(client.sin_addr));
        send(connectfd, "Welcom to server.", 22, 0);
        close(connectfd);
    }
    
    close(listenfd);

}
