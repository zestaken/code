#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
    u_int32_t ipv4_h, ipv4_n;
    u_int16_t port_h, port_n;

    char delims[] = ".";
    char * result = NULL;
    int ipv4[4]; 
    int j = 0;

    //将ipv4地址四个数字存入数组中
    result = (char *)strtok(argv[1], delims);
    while (result != NULL) {
        ipv4[j] = atoi(result);
        result = (char *)strtok(NULL, delims);
        j++;
    }

    unsigned char* bytepointer = (unsigned char *)&ipv4_n;
    //将数组中的数字通过从低到高写入ipv4_n中
    for(int i = 0; i < 4; i++) {
        bytepointer[i] = ipv4[i];
    }

    //将port读入并转换为网络字节序
    port_n = atoi(argv[2]);
    port_n = htons(port_n);

    //输出存储网络字节序信息的ipv4_n和port_n
    //1.
    printf("[ipv4_n_cbc_d]");
    for(int i = 0; i < 4; i++) {
        if(i <= 2) {
            printf("%d.", bytepointer[i]);
        }else {
            printf("%d", bytepointer[i]);
        }
    }

    //2.
    printf("\n[ipv4_n_cbc_x]");
    for(int i = 0; i < 4; i++) {
        printf("%02X",*(bytepointer + i));
    }
        
    //3
    printf("\n[ipv4_n_asw_x]");
    printf("%08X",ipv4_n);
    
    //4
    bytepointer = (unsigned char *)&port_n;
    printf("\n[port_n_cbc_x]");
    for(int i = 0; i < sizeof(u_int16_t); i++) {
        printf("%02X", bytepointer[i]);
    }

    //5
    printf("\n[port_n_asw_x]");
    printf("%04X",port_n);

    //6
    printf("\n[port_n_asw_d]");
    printf("%d", port_n);

    ipv4_h = ntohl(ipv4_n);
    port_h = ntohs(port_n);

    //输出存储主机字节序信息的ipv4_h和port_h
    bytepointer = (unsigned char *)&ipv4_h;
    //7.
    printf("\n[ipv4_h_cbc_d]");
    for(int i = 0; i < 4; i++) {
        if(i <= 2) {
            printf("%d.", bytepointer[i]);
        }else{
            printf("%d",bytepointer[i]);
        }

    }

    //8.
    printf("\n[ipv4_h_cbc_x]");
    for(int i = 0; i < 4; i++) {
        printf("%02X",*(bytepointer + i));
    }
        
    //9
    printf("\n[ipv4_h_asw_x]");
    printf("%08X",ipv4_h);
    
    //10
    bytepointer = (unsigned char *)&port_h;
    printf("\n[port_h_cbc_x]");
    for(int i = 0; i < sizeof(u_int16_t); i++) {
        printf("%02X", bytepointer[i]);
    }

    //11
    printf("\n[port_h_asw_x]");
    printf("%04X",port_h);

    //12
    printf("\n[port_h_asw_d]");
    printf("%d\n", port_h);
}
