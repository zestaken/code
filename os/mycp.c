#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <libgen.h>
#include <dirent.h>

int copyFileToFile(char *srcPath, char *dstPath);
int copyFileToDir(char *srcPath, char *dstPath);
int copyDirToDir(char *srcPath, char *dstPath);


int main(int argc, char *argv[]) {

    //文件复制到文件
    char *srcPath = argv[1];
    char *dstPath = argv[2];




    printf("%s\n", srcPath);
    printf("%s\n", dstPath);

    struct stat srcBuf, dstBuf;
    if(lstat(dstPath, &dstBuf) < 0 || lstat(srcPath, &srcBuf) < 0) {
        printf("lstat error %d\n", errno);
    } else if (S_ISREG(srcBuf.st_mode) && S_ISREG(dstBuf.st_mode)) {
        copyFileToFile(srcPath, dstPath);
    } else if (S_ISREG(srcBuf.st_mode) && S_ISDIR(dstBuf.st_mode)) {
        copyFileToDir(srcPath, dstPath);
    } else if(S_ISDIR(srcBuf.st_mode) && S_ISDIR(dstBuf.st_mode)) {
        copyDirToDir(srcPath,dstPath);
    }
    
    return 0;
}

int copyFileToFile(char *srcPath, char *dstPath) {
    
    //打开文件
    int srcFd, dstFd;

    printf("\ncopy file to file\n");
    printf("srcPath: %s\n", srcPath);
    printf("dstPath: %s\n\n", dstPath);
    srcFd = open(srcPath, O_RDONLY);
    if(errno != 0) {
        printf("srcPath:Failed to open file! Errno: %d\n", errno);
        // return 0;
    }
    dstFd = open(dstPath, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
    if(errno != 0) {
        printf("dstPath:Failed to open file! Errno: %d\n", errno);
        return 0;
    }

    //读取源文件写入目的文件
    char buffer[100];
    while(read(srcFd, buffer, 1) > 0 ) {
        write(dstFd, buffer, 1);
    }

    close(srcFd);
    close(dstFd);

    return 1;
}

int copyFileToDir(char *srcPath, char *dstPath) {
    
        char *deLimiter = "/"; //路径分隔符
        char *srcName = basename(srcPath); //获取源文件名

        //合并目标路径目标文件名
        strcat(dstPath, deLimiter);
        strcat(dstPath, srcName);

        int status = copyFileToFile(srcPath, dstPath);
        if(status == 1) {
            printf("success\n");
            return 1;
        } else {
            printf("Fail\n");
            return 0;
        }
}

int copyDirToDir(char *srcPath, char *dstPath) {

    char *deLimiter = "/"; //路径分隔符
    DIR *srcDir = opendir(srcPath);
    struct dirent *curDir;
    char *tmpPath = srcPath;
    char *path = strtok(tmpPath, deLimiter);
    char *srcPath1;
    char *dstPath1;
    
    //逐级创建目录
    while(path != NULL){
        
        strcat(dstPath, deLimiter);
        strcat(dstPath, path);
        int s = mkdir(dstPath, S_IRWXU | S_IRWXG | S_IRWXO);
        printf("mkdir: %d errno: %d\n", s, errno);
        if(errno == 17) {
            printf("Directory exits !\n");
            return 0;
        }
        path = strtok(NULL, deLimiter);
        printf("path:%s\n", path);
    } 
    printf("mkdir dstPath: %s\n", dstPath);

    while((curDir = readdir(srcDir)) != NULL) {
        printf("curDir: %s; type: %d\n", curDir->d_name, curDir->d_type);
        if(curDir->d_type == 8) {
            dstPath1 = dstPath;
            srcPath1 = srcPath;

            strcat(srcPath1, deLimiter);
            strcat(srcPath1, curDir->d_name);
            //合并目标路径目标文件名
            strcat(dstPath1, deLimiter);
            strcat(dstPath1, srcPath1);
            
            copyFileToFile(srcPath1, dstPath1);
        }
    }

    return 1;
}

