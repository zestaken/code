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
#include <string.h>

#define MAX_CMD_STR 100

//业务逻辑处理函数
int echo_rqt(int sockfd, int pin) {
    //声明缓冲区
    char buf[MAX_CMD_STR + 9];
    char tmp[MAX_CMD_STR + 1];
    struct PDU{
        int pin;
        int len;
        char buf[MAX_CMD_STR + 1];
    };
    struct PDU *pdu = (struct PDU *)malloc(sizeof(struct PDU));
    //拼接文件名
    char filename[20];
    sprintf(filename, "td%d.txt", pin);
    FILE *fp = fopen(filename, "r");

    //循环读取，每次从tdPIN.txt文件中读取一行
    int i = 0;
    while(fgets(pdu->buf, MAX_CMD_STR, fp)) {
        //如果开头字符为exit退出程序
        if(strncmp(pdu->buf, "exit", 4) == 0) {
            return 0;
        }
        
        //获取读取的字符串的长度
        int len_h = strnlen(pdu->buf, MAX_CMD_STR + 1);
        //将最后一位字符由\n改为\0
        (pdu->buf)[len_h - 1] = '\0';
        
        //将len_h和pin转换为网络字节序
        int len_n = htonl(len_h);
        int pin_n = htonl(pin);
        pdu->pin = pin_n;
        pdu->len = len_n;
        // printf("-----------\n");
        // printf("pdu pin: %d\n", pdu->pin);
        // printf("pdu len: %d\n", pdu->len);
        // printf("pdu buf: %s\n", pdu->buf);
        // printf("----------\n");
        // //构建PDU
        // char pin_s[4];
        // char len_s[30];
        // memset(buf, 0, sizeof(buf));
        // memset(pin_s, 0, sizeof(pin_s));
        // memset(len_s, 0, sizeof(len_s));
        // snprintf(pin_s, sizeof(pin_n), "%d", pin_n);
        // snprintf(len_s, sizeof(len_n), "%d", len_n);
        // strncat(buf, pin_s, strlen(pin_s));
        // strncat(buf, len_s, strlen(len_s));
        // strncat(buf, tmp, strlen(tmp) + 1);
        // printf("pin_s%d: %d pin_n: %d pin: %d sizeof pin_n: %d\n", i, atoi(pin_s), pin_n, pin, (int)sizeof(pin_n));      
        // printf("len_s%d: %d len_n: %d len_h: %d sizeof len_n: %d\n", i, atoi(len_s), len_n, len_h, (int)sizeof(len_n));
        // printf("tmp%d: %s\n", i, tmp);
        // printf("buf%d: %s\n", i, buf);

        //发送pdu
        char buf2[MAX_CMD_STR + 9];
        //将结构体转换为字符串
        memcpy(buf2 + 0, &(pdu->pin), sizeof(pdu->pin));
        memcpy(buf2 + sizeof(pdu->pin), &(pdu->len), sizeof(pdu->len));
        memcpy(buf2 + sizeof(pdu->pin) + sizeof(pdu->len), pdu->buf, sizeof(pdu->buf));
        write(sockfd, buf2, 8 + sizeof(pdu->buf));
        i++;

        // 读取服务器数据
        int res = 0, res1 = 0, srv_pin_n = 0, srv_len_n =0 ,srv_pin_h =0, srv_len_h = 0;
        char buf[MAX_CMD_STR + 1];
        read(sockfd, &srv_pin_n, sizeof(srv_pin_h));
        read(sockfd, &srv_len_n, sizeof(srv_len_n));
        srv_len_h = ntohl(srv_len_n);
        srv_pin_h = ntohl(srv_pin_n);
        char *tmp = buf;
        while(res < srv_len_h) {
            res1 = read(sockfd, tmp, len_h);
            tmp += res1;
            res += res1;
        }
        
        printf("##########\n");
        printf("srv_pin_h: %d\n", srv_pin_h);
        printf("srv_len_h: %d\n", srv_len_h);
        printf("[echo_rep](%d) %s\n", getpid(), buf);
        printf("##########\n");
    }
 
    return 0;
}

int main(int argc, char *argv[]) {

    int connectfd;
    int count = 0;

    //检查用户是否正确输入参数
    if(argc != 4) {
        printf("Usage: %s <ip> <port> <count>\n", argv[0]);
        exit(-1);
    }
    count = atoi(argv[3]);

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

    
    for(int i = 1; i < count || i == 1; i++) {
        pid_t pid = 1;

        if(count != 1) {
            pid = fork();
        }

        if(pid < 0) {
            //todo fork出错提示
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
            fprintf(fp, "[cli](%d) child process %d is created!\n", getpid(), pin);
            printf("[cli](%d) child process %d is created!\n", getpid(), pin);


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
                fprintf(fp, "[cli](%d) server[%s:%d] is connected!\n", getpid(), ip_h, ntohs(port_n));
                printf("[cli](%d) server[%s:%d] is connected!\n", getpid(), ip_h, ntohs(port_n));

                //业务处理
                echo_rqt(connectfd, pin);
            }

            //关闭连接套接字
            close(connectfd);
            fprintf(fp, "[cli](%d) connfd is closed!\n", getpid());
            fprintf(fp, "[cli](%d) parent process is going to exit!\n", getpid());
            printf("[cli](%d) connfd is closed!\n", getpid());
            printf("[cli](%d) parent process is going to exit!\n", getpid());

            //关闭文件
            fclose(fp);
            printf("[cli](%d) stu_cli_res_%d.txt is closed!\n", getpid(), pin);

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
            fprintf(fp, "[cli](%d) child process %d is created!\n", getpid(), pin);
            printf("[cli](%d) child process %d is created!\n", getpid(), pin);


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
                fprintf(fp, "[cli](%d) server[%s:%d] is connected!\n", getpid(), ip_h, ntohs(port_n));
                printf("[cli](%d) server[%s:%d] is connected!\n", getpid(), ip_h, ntohs(port_n));

                //业务处理
                echo_rqt(connectfd, pin);
            }

            //关闭连接套接字
            close(connectfd);
            fprintf(fp, "[cli](%d) connfd is closed!\n", getpid());
            fprintf(fp, "[cli](%d) parent process is going to exit!\n", getpid());
            printf("[cli](%d) connfd is closed!\n", getpid());
            printf("[cli](%d) parent process is going to exit!\n", getpid());


            //关闭文件
            fclose(fp);
            printf("[cli](%d) stu_cli_res_%d.txt is closed!\n", getpid(), pin);
            exit(0);
        }
    }

}
