#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[]) {

    //文件复制到文件
    char *srcPath = argv[1];
    char *dstPath = argv[2];

    //打开文件
    int srcFd, dstFd;
    srcFd = open(srcPath, O_RDONLY);
    dstFd = open(dstPath, O_RDWR | O_CREAT | O_TRUNC, S_IWUSR);
    
    fprintf(stderr, "Value of errno: %d\n", errno);
    fprintf(stderr, "Error opening file: %s\n", strerror(errno));

    //读取源文件写入目的文件
    char *buffer[100];
    while(read(srcFd, buffer, 100) != -1) {
        write(dstFd, buffer, 100);
    }

    return 0;
}