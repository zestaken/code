#include <stdio.h>		
#include <stdlib.h>		//exit()函数相关
#include <unistd.h>		//C 和 C++ 程序设计语言中提供对 POSIX 操作系统 API 的访问功能的头文件
#include <sys/types.h>		//Unix/Linux系统的基本系统数据类型的头文件,含有size_t,time_t,pid_t等类型
#include <sys/socket.h>		//套接字基本函数相关
#include <netinet/in.h>		//IP地址和端口相关定义，比如struct sockaddr_in等
#include <arpa/inet.h>		//inet_pton()等函数相关
#include <string.h>		//bzero()函数相关

int main(int argc, char *argv[]) {

    int clientfd, numbytes;
    char buf[100];

    struct sockaddr_in server_addr;
    if(argc != 3) {

        printf("args error\n");
        exit(0);
    }

    if( (clientfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket error.");
        exit(-1);
    }

    bzero(&server_addr, sizeof(server_addr));
    
}