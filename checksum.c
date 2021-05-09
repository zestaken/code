#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    u_int32_t sum = 0;

    char *data = argv[1];
    // strcpy(data, argv[1]);
    char temp[4];
    int i = 0;
    int flag = 0;
    while(data[i] != '\0' && flag == 0) {
        for(int j = 0; j < 4; j++) {
            if(data[i + j] != '\0') {
                temp[j] = data[i + j];
            } else {
                for(; j < 4; j++) {
                    temp[j] = '0';     
                    flag = 1;
                }
            }
        }
        if(flag == 0) {
            i = i + 4;
            char *p;
        }
        char *p;
        // u_int16_t test;
        // test = strtol(temp,&p,16);
        // printf("test: %04x\n", test);
        sum += strtol(temp, &p, 16);
        // printf("sum: %05x\n",sum);
        if(sum > 0xFFFF) {
            sum = sum - 0x10000 + 0x0001;
        }
    }
    u_int16_t checksum = ~sum;
    printf("%04x\n", checksum);
}
