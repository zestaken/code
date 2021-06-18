#include <stdio.h>		
#include <stdlib.h>		//exit()函数相关
#include <unistd.h>		//C 和 C++ 程序设计语言中提供对 POSIX 操作系统 API 的访问功能的头文件
#include <sys/types.h>		//Unix/Linux系统的基本系统数据类型的头文件,含有size_t,time_t,pid_t等类型
#include <sys/socket.h>		//套接字基本函数相关
#include <netinet/in.h>		//IP地址和端口相关定义，比如struct sockaddr_in等
#include <arpa/inet.h>		//inet_pton()等函数相关
#include <string.h>		//bzero()函数相关
#include <signal.h>    //信号处理相关
#include <errno.h> //错误处理相关

#define MAX_CMD_STR 100

int sig_to_exit = 0;
int sig_type = 0;

void sig_int(int signo) {
    printf("[srv] SIGINT is coming!\n");
    sig_to_exit = 1;
    sig_type = SIGINT;
}

void sig_pipe(int signo) {
    printf("[srv] SIGPIPE is coming!\n");
    sig_type = SIGPIPE;
}

void echo_rep(int sockfd) {


    while(1) {
        int len_h = 0, len_n = 0, res = 0, res1 = 0;
        char *buf;
    //读取客户端发送的字符串
        //读取应读取的字符串长度
        read(sockfd, &len_n, sizeof(len_n));
        len_h = ntohl(len_n);
        printf("len_h: %d\n", len_h);
        buf = (char *)malloc(sizeof(char) * len_h);
        char *tmp = buf;
        printf("读取客户端数据之前\n");
        if(len_h == 0) {
            return;
        }
        while(res < len_h) {
            res1 = read(sockfd, tmp, len_h);
            if(res1 < 0) {
                printf("[srv] read data return %d and errno is %d\n", res1, errno);
                if(errno == EINTR) {
                    if(sig_type == SIGINT) {
                        //  若是中断信号，释放资源退出
                        free(buf);
                        return;
                    } 
                    continue;
                }                
                //若是其它错误，释放资源退出
                free(buf);
                return;
            } else if(res1 == 0) {
                return;
            }

            res += res1;
            printf("res: %d\n", res);
            printf("tmp: %s\n", tmp);
            tmp += res1;
            printf("res1: %d\n", res1);
        }
        if(sig_to_exit == 1) {
            return;
        }
        printf("[echo_rqt] %s\n", buf);  
        write(sockfd, &len_n, sizeof(len_n));
        write(sockfd, buf, len_h);
    }
}


int main(int argc, char *argv[]) {

    //安装信号处理器
        //安装SIGPIPE处理器
    struct sigaction sigact_pipe, old_sigact_pipe;
    sigact_pipe.sa_handler = sig_pipe;
    sigemptyset(&sigact_pipe.sa_mask);
    sigact_pipe.sa_flags = 0;
    sigact_pipe.sa_flags |= SA_RESTART;
    sigaction(SIGPIPE, &sigact_pipe, &old_sigact_pipe);
        //安装SIGINT处理器
    struct sigaction sigact_int, old_sigact_int;
    sigact_int.sa_handler = sig_int;
    sigemptyset(&sigact_int.sa_mask);
    sigact_int.sa_flags = 0;
    sigaction(SIGINT, &sigact_int, &old_sigact_int);

    struct sockaddr_in server, client;  //声明socket地址结构
    socklen_t sin_size = (socklen_t)(sizeof(struct sockaddr_in));
    int listenfd, connectfd; //声明监听描述符和连接描述符

    //检查用户是否正确输入参数
    if(argc != 3) {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        exit(-1);
    }

    //初始化socket地址
    char ip_h[20];
    int port_n;
    bzero(&server, sizeof(server));
    server.sin_family = PF_INET;
    port_n = htons(atoi(argv[2])); //初始化port,将从命令行获得的参数转换为网络字节序
    server.sin_port = port_n;
    if(inet_pton(PF_INET, argv[1], &server.sin_addr.s_addr) == 0) { //从argv[1]将输入的IP转换为网络字节序存入sockaddr_in结构体中
        perror("Sever IP address error: ");
        exit(1);
    }

    //将ip和port转换为主机字节序，打印，以验证转换成功
    inet_ntop(PF_INET, &server.sin_addr.s_addr, ip_h, sin_size);
    
    printf("[srv] server[%s:%d] is initializing!\n", ip_h, ntohs(port_n));

    //获取socket监听描述符
    if((listenfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(-1);
    }

    //绑定服务器地址和端口
    if((bind(listenfd, (struct sockaddr *)&server, sin_size)) == -1) {
        perror("bind error");
        exit(-1);
    }

    //开启服务监听
    if((listen(listenfd, 5)) == -1) {
        perror("listen error");
        exit(-1);
    }

    //开启while主循环，直至sig_to_exit指示程序退出
    while(!sig_to_exit) {
        if((connectfd = accept(listenfd, (struct sockaddr *)&client, &sin_size)) == - 1) {
            printf("accept\n");
            perror("accept error");
            continue;
        }
        
        //打印client的ip和端口
        char cli_ip_h[20];
        inet_ntop(AF_INET, &client.sin_addr.s_addr, cli_ip_h, sin_size);
        int cli_port_h = ntohs(client.sin_port);
        printf("[srv] client[%s:%d] is accepted!\n", cli_ip_h, cli_port_h);

        //调用业务处理函数echo_rep()
        echo_rep(connectfd);

        //关闭连接套接字
        close(connectfd);
        printf("[srv] connfd is closed!\n");
    }

    //关闭监听套接字
    close(listenfd);
    printf("[srv] listenfd is closed!\n");
    printf("[srv] server is exiting\n");
    return 0;
}