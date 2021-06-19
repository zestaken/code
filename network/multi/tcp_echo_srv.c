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
#include <wait.h>

#define MAX_CMD_STR 100

int sig_to_exit = 0;
int sig_type = 0;

void sig_int(int signo) {
    sig_to_exit = 1;
    sig_type = signo;
    printf("[srv](%d) SIGINT is coming!\n", getpid());
}

void sig_pipe(int signo) {
    sig_type = signo;
    printf("[srv](%d) SIGPIPE is coming!\n", getpid());
}

//处理SIGCHLD信号，避免因为子进程先于父进程结束而产生僵尸进程
void sig_child(int signo) {
    pid_t pid_child;
    sig_type = signo;
    while((pid_child = waitpid(-1, NULL, WNOHANG)) > 0) {
         printf("[srv](%d) server child(%d) terminated!\n", getpid(), pid_child);
    };
}

int echo_rep(int sockfd) {
    struct PDU {
        int pin;
        int len;
        char buf[MAX_CMD_STR + 1];
    };
    int pin = 0;

    while(1) {
        int len_h = 0,len_n = 0,  res = 0, res1 = 0,pin_h = 0, pin_n = 0;
        char *buf;
        struct PDU *pdu;
        //读取客户端发送的字符串
        //读取应读取的字符串长度
        if(read(sockfd, &pin_n, sizeof(pin_n)) == 0) {
                printf("res == 0 exit\n");
                return pin;
        }
        if(read(sockfd, &len_n, sizeof(len_n)) == 0) {
                printf("res == 0 exit\n");
                return pin;
        }
        pin_h = ntohl(pin_n);
        len_h = ntohl(len_n);
        printf("pin_h: %d\n", pin_h);
        printf("len_h: %d\n", len_h);
        buf = (char *)malloc(sizeof(char) * len_h);
        pdu = (struct PDU *)malloc(sizeof(struct PDU));
        memset(buf, 0, sizeof(buf));
        memset(pdu, 0, sizeof(*pdu));
        char *tmp = buf;
        printf("读取客户端数据之前\n");

        // if(len_h == 0) {
        //     printf("len_h == 0 exit\n");
        //     return -1;
        // }

        while(res < len_h) {
            res1 = read(sockfd, tmp, len_h);
            printf("res1: %d\n", res1);
            if(res1 < 0) {
                printf("[srv] read data return %d and errno is %d\n", res1, errno);
                if(errno == EINTR) {
                    if(sig_type == SIGINT) {
                        //  若是中断信号，释放资源退出
                        free(buf);
                        free(pdu);
                        printf("sigint\n");
                        return -1;
                    } 
                    continue;
                }                
                //若是其它错误，释放资源退出
                free(buf);
                free(pdu);
                printf("other errno\n");
                return -1;
            } else if(res1 == 0) {
                printf("res == 0 exit\n");
                free(buf);
                free(pdu);
                return pin;
            }

            res += res1;
            tmp += res1;
        }

        if(sig_to_exit == 1) {
            printf("sig_to_exit \n");
            return -1;
        }

        pdu->pin = pin_n;
        pdu->len = len_n;

        strcpy(pdu->buf, buf);
        printf("[echo_rqt] %s\n", pdu->buf);  
        printf("$$$$$$$$$$$$$\n");
        //发送pdu
        char buf2[MAX_CMD_STR + 9];
        //将结构体转换为字符串
        memcpy(buf2 + 0, &(pdu->pin), sizeof(pdu->pin));
        memcpy(buf2 + sizeof(pdu->pin), &(pdu->len), sizeof(pdu->len));
        memcpy(buf2 + sizeof(pdu->pin) + sizeof(pdu->len), pdu->buf, sizeof(pdu->buf));
        write(sockfd, buf2, 8 + strlen(pdu->buf));
        free(buf);
        free(pdu);
        pin = pin_h;
    }
    return pin;
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
         //安装SIGCHILD处理器
    struct sigaction sigact_child, old_sigact_child;
    sigact_child.sa_handler = sig_child;
    sigemptyset(&sigact_child.sa_mask);
    sigact_child.sa_flags = 0;
    sigact_child.sa_flags |= SA_RESTART;
    sigaction(SIGCHLD, &sigact_child, &old_sigact_child);

    struct sockaddr_in server, client;  //声明socket地址结构
    socklen_t sin_size = (socklen_t)(sizeof(struct sockaddr_in));
    int listenfd, connectfd; //声明监听描述符和连接描述符

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
    
    FILE *fp = fopen("stu_srv_res_p.txt", "a");
    printf("[srv](%d) stu_srv_res_p.txt is opened!\n", getpid());

    printf("[srv](%d) server[%s:%d] is initializing!\n", getpid(), ip_h, ntohs(port_n));
    fprintf(fp, "[srv](%d) server[%s:%d] is initializing!\n", getpid(), ip_h, ntohs(port_n));

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
            perror("accept error");
            continue;
        }
        //打印client的ip和端口
        char cli_ip_h[20];
        inet_ntop(AF_INET, &client.sin_addr.s_addr, cli_ip_h, sin_size);
        int cli_port_h = ntohs(client.sin_port);
        printf("[srv](%d) client[%s:%d] is accepted!\n", getpid(), cli_ip_h, cli_port_h);
        fprintf(fp, "[srv](%d) client[%s:%d] is accepted!\n", getpid(), cli_ip_h, cli_port_h);

        //创建子进程
        pid_t pid = fork();

        if(pid == 0) {
            char filename[30];
            sprintf(filename, "stu_srv_res_%d.txt", getpid());
            FILE *fp1 = fopen(filename, "a");    
            printf("[srv](%d) stu_srv_res_%d.txt is opened!\n", getpid(), getpid());

            printf("[cli](%d) child process is created!\n", getpid());
            fprintf(fp1, "[cli](%d) child process is created!\n", getpid());
            //调用业务处理函数echo_rep()
            int pin = echo_rep(connectfd);

            //文件重命名
            char filename2[20];
            sprintf(filename2, "stu_srv_res_%d.txt", pin);
            rename(filename, filename2);
            //关闭res文件
            printf("[srv](%d) child process is going to exit!\n", getpid());
            fprintf(fp1, "[srv](%d) child process is going to exit!\n", getpid());
            fclose(fp1);
            printf("[srv](%d) %s is closed!\n", getpid(), filename2);
             //关闭连接套接字
            close(connectfd);
            printf("[srv](%d) connfd is closed!\n", getpid());
            int fprintf1 = fprintf(fp1, "[srv](%d) connfd is closed!\n", getpid());
            printf("-------------------%d------------\n", fprintf1);
            
            exit(0);
        }

    }

    //关闭监听套接字
    close(listenfd);
    printf("[srv](%d) listenfd is closed!\n", getpid());
    printf("[srv](%d) parent process is going to exit!\n",getpid());
    fprintf(fp, "[srv](%d) listenfd is closed!\n", getpid());
    fprintf(fp, "[srv](%d) parent process is going to exit!\n",getpid());
    fclose(fp);
    printf("[srv](%d) stu_srv_res_p.txt is closed!\n", getpid());
    return 0;

}