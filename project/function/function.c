#include<fcntl.h>
#include<sys/stat.h>
#include<errno.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"../global/var.h"

extern FILE * DISK;
extern char sysname[100];
int startsys(){//初始化文件系统
    int fd;
//以创建新文件的方式打开
    if((fd=open(sysname,O_CREAT|O_EXCL,S_IRWXU))<0){
        printf("open:%d  %s\n",errno,strerror(errno));
    //打开失败 检测失败原因
        if(errno==EEXIST){//文件已存在
            fd = open(sysname,O_RDWR,S_IRWXU);//直接打开这个文件
            if(fd<0)
                printf("open error\n");
        }
        else//其他原因
            exit(-1);
    }
//打开成功(创建新文件)
    if((DISK = fdopen(fd,"r+"))==NULL){
        printf("fdopen error\n");
        printf("%d  %s\n",errno,strerror(errno));
        exit(-1);
    }
    fputs("this is my disk!\n",DISK);
    fclose(DISK);

}
void format(){//文件系统格式化

}