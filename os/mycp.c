#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <libgen.h>
#include <dirent.h>
#include <stdlib.h>

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
    if(lstat(dstPath, &dstBuf) >= 0 && lstat(srcPath, &srcBuf) >= 0) {
        
        if (S_ISREG(srcBuf.st_mode) && S_ISREG(dstBuf.st_mode)) {
            copyFileToFile(srcPath, dstPath);
        } else if (S_ISREG(srcBuf.st_mode) && S_ISDIR(dstBuf.st_mode)) {
            copyFileToDir(srcPath, dstPath);
        } else if(S_ISDIR(srcBuf.st_mode) && S_ISDIR(dstBuf.st_mode)) {
            copyDirToDir(srcPath, dstPath);
        }
    } else if(lstat(dstPath, &dstBuf) < 0 && errno == 2 ) {
        if(lstat(srcPath, &srcBuf) >= 0) {
            copyFileToFile(srcPath, dstPath);
        } else {
            printf("source is not exsiting!\n");
        }
    }
    
    return 0;
}

int copyFileToFile(char *srcPath, char *dstPath) {
    
    //打开文件
    int srcFd, dstFd;

    srcFd = open(srcPath, O_RDONLY);
    if(errno != 0) {
        printf("srcPath:%s Failed to open file! Errno: %d\n", srcPath, errno);
        // return 0;
    }
    dstFd = open(dstPath, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
    if(errno != 0) {
        printf("dstPath:%s Failed to open file! Errno: %d\n",dstPath, errno);
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
        char dstPath1[strlen(dstPath) + 1];
        strcpy(dstPath1, dstPath);

        //合并目标路径目标文件名
        strcat(dstPath1, deLimiter);
        strcat(dstPath1, srcName);

        int status = copyFileToFile(srcPath, dstPath1);
        if(status == 1) {
            printf("success\n");
            return 1;
        } else {
            printf("Fail\n");
            return 0;
        }
}

int copyDirToDir(char *srcPath, char *dstPath) {

    char dstPath1[strlen(dstPath) + 1];
    printf("Enter copydir\n");
    char *deLimiter = "/"; //路径分隔符
    char tmpPath[strlen(srcPath) + 1];
    char *path1;
    printf("after char *!\n");
    strcpy(tmpPath, srcPath);
    strcpy(dstPath1, dstPath);
    struct dirent *curDir = (struct dirent *)malloc(sizeof(struct dirent));
    char *path = strtok(tmpPath, deLimiter);

    //获取目录名
    while(path != NULL){
	    path1 = path;
        path = strtok(NULL, deLimiter);
        printf("path:%s\n", path);
    } 

    strcat(dstPath1, deLimiter);
    strcat(dstPath1, path1);
    int s = mkdir(dstPath1, S_IRWXU | S_IRWXG | S_IRWXO);
    printf("mkdir: %d errno: %d\n", s, errno);
    if(errno == 17) {
        printf("Directory exits !\n");
         return 0;
     }
    printf("mkdir dstPath: %s\n", dstPath1);


    DIR *srcDir = opendir(srcPath);
    while((curDir = readdir(srcDir)) != NULL) {
        printf("curDir: %s; type: %d\n", curDir->d_name, curDir->d_type);
        if(curDir->d_type == 8) {
    		char srcPath1[strlen(srcPath) + 1];
            strcpy(srcPath1, srcPath);
            printf("1-srcPath1: %s\n", srcPath1);
            strcat(srcPath1, deLimiter);
            strcat(srcPath1, curDir->d_name);
            printf("2-srcPath1: %s\n", srcPath1);
            copyFileToDir(srcPath1, dstPath1);
        } else if(strcmp(curDir->d_name, ".") != 0 && strcmp(curDir->d_name, "..") != 0 && curDir->d_type == 4) {
            char srcPath2[strlen(srcPath) + 1];
            strcpy(srcPath2, srcPath);
            printf("1-srcPath2: %s\n", srcPath2);
            strcat(srcPath2, deLimiter);
            strcat(srcPath2, curDir->d_name);

            copyDirToDir(srcPath2, dstPath1);
            printf("递归返回！");
        }
    }

    return 1;
}

