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

    //获取命令行参数
    char *srcPath = argv[1];
    char *dstPath = argv[2];

    printf("%s\n", srcPath);
    printf("%s\n", dstPath);

    //根据目录名来检测用户输入的是目录还是文件
    struct stat srcBuf, dstBuf;
        //如果两个路径所指的文件或目录都存在
    if(lstat(dstPath, &dstBuf) >= 0 && lstat(srcPath, &srcBuf) >= 0) {
        

        if (S_ISREG(srcBuf.st_mode) && S_ISREG(dstBuf.st_mode)) {
            //从文件复制到文件
            copyFileToFile(srcPath, dstPath);
        } else if (S_ISREG(srcBuf.st_mode) && S_ISDIR(dstBuf.st_mode)) {
            //从文件复制到目录
            copyFileToDir(srcPath, dstPath);
        } else if(S_ISDIR(srcBuf.st_mode) && S_ISDIR(dstBuf.st_mode)) {
            //从目录复制到目录
            copyDirToDir(srcPath, dstPath);
        }
    } else if(lstat(dstPath, &dstBuf) < 0 && errno == 2 ) { 

        //如果源路径所指的文件存在
        if(lstat(srcPath, &srcBuf) >= 0) {
            copyFileToFile(srcPath, dstPath);
        } else {
            printf("source is not exsiting!\n");
        }
    }
    
    return 0;
}

//文件复制到文件
int copyFileToFile(char *srcPath, char *dstPath) {
    
    //打开文件描述符
    int srcFd, dstFd;

    //打开源文件
    srcFd = open(srcPath, O_RDONLY);
    if(errno != 0) {
        printf("srcPath:%s Failed to open file! Errno: %d\n", srcPath, errno);
        return 0;
    }
    //打开目的文件，如果不存在则创建
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

    //关闭文件描述符
    close(srcFd);
    close(dstFd);

    return 1;
}

//文件复制到目录
int copyFileToDir(char *srcPath, char *dstPath) {
    
        char *deLimiter = "/"; //路径分隔符
        char *srcName = basename(srcPath); //获取源文件名
        char dstPath1[strlen(dstPath) + 1];
        strcpy(dstPath1, dstPath);

        //合并目标路径目标文件名
        strcat(dstPath1, deLimiter);
        strcat(dstPath1, srcName);

        //调用从文件复制到文件的方法创建目的新文件
        int status = copyFileToFile(srcPath, dstPath1);
        if(status == 1) {
            printf("success\n");
            return 1;
        } else {
            printf("Fail\n");
            return 0;
        }
}

//目录复制到目录
int copyDirToDir(char *srcPath, char *dstPath) {

    char dstPath1[strlen(dstPath) + 1];
    char *deLimiter = "/"; //路径分隔符
    char tmpPath[strlen(srcPath) + 1];
    char *path1;
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
    //创建目录
    int s = mkdir(dstPath1, S_IRWXU | S_IRWXG | S_IRWXO);
    printf("mkdir: %d errno: %d\n", s, errno);
    if(errno == 17) {
        printf("Directory exits !\n");
         return 0;
     }

    DIR *srcDir = opendir(srcPath);
    while((curDir = readdir(srcDir)) != NULL) {
        printf("curDir: %s; type: %d\n", curDir->d_name, curDir->d_type);

        
        if(curDir->d_type == 8) { //如果扫描到的是文件
    		char srcPath1[strlen(srcPath) + 1];
            strcpy(srcPath1, srcPath);
            strcat(srcPath1, deLimiter);
            strcat(srcPath1, curDir->d_name);
            //调用从文件复制到目录方法实现复制
            copyFileToDir(srcPath1, dstPath1);
        } else if(strcmp(curDir->d_name, ".") != 0 && strcmp(curDir->d_name, "..") != 0 
                    && curDir->d_type == 4) { //如果扫描到的是目录
            char srcPath2[strlen(srcPath) + 1];
            strcpy(srcPath2, srcPath);
            strcat(srcPath2, deLimiter);
            strcat(srcPath2, curDir->d_name);
            //递归调用本方法
            copyDirToDir(srcPath2, dstPath1);
            printf("子目录复制完成返回\n");
        }
    }

    return 1;
}

