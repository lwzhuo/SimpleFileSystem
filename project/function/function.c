#include<errno.h>
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<unistd.h>
#include"./api.h"
#include"../global/var.h"
#include"../util/disk.h"
void startsys(){//初始化文件系统
    int fd;
    int hasDisk = 1;
//以创建新文件的方式打开
    if((fd=open(sysname,O_CREAT|O_EXCL,S_IRWXU))<0){
    //打开失败(通常是已经存在这个文件) 检测失败原因
        if(errno==EEXIST){//文件已存在
            if((fd = open(sysname,O_RDWR,S_IRWXU))<0){//直接打开这个文件
                printf("open:%d  %s\n",errno,strerror(errno));//打开失败
                exit(-1);
            }
        }
        else//其他原因
            exit(-1);
    }else{//如果采取新建一个文件，那么新建一个磁盘文件并分配空间
        close(fd);
        printf("没有找到磁盘文件,开始创建磁盘文件\n");
        createDisk();
        if((fd = open(sysname,O_RDWR,S_IRWXU))<0){//直接打开这个文件
            printf("open:%d  %s\n",errno,strerror(errno));//打开失败
            exit(-1);
        }
    }
       
//变成标准IO流
    if((DISK = fdopen(fd,"r+"))==NULL){
        printf("fdopen:%d  %s\n",errno,strerror(errno));
        exit(-1);
    }
//转换成功
    char buff[10];
    if(fread(&block0,sizeof(BLOCK0),1,DISK)==1)//取出第一个磁盘块(引导块)
//判断是否格式化            
    if(strcmp(block0.identify,"MYDISK")!=0){
        printf("磁盘尚未格式化，开始格式化\n");
        format();
    }         
}


void format(){//文件系统格式化
//引导块
    strcpy(block0.identify,"MYDISK");
    strcpy(block0.info,"blocksize:1024B\nblocknum:1024 Disksize:1MB");
    block0.root = 3;
    block0.startblock = 3*BLOCK_SIZE;

    fseek(DISK,0,SEEK_SET);
    fwrite(&block0,sizeof(BLOCK0),1,DISK);
//两个FAT块
    int *fat;
    fat = (int *)malloc(sizeof(int)*BLOCK_NUMS);
    memset(fat,0,sizeof(int));
    fseek(DISK,1*BLOCK_SIZE,SEEK_SET);
    fwrite(fat,sizeof(int),BLOCK_NUMS,DISK);
    fseek(DISK,2*BLOCK_SIZE,SEEK_SET);
    fwrite(fat,sizeof(int),BLOCK_NUMS,DISK);
    free(fat);
//根目录区
}

void showBlock0(){
    BLOCK0 b0;
    fseek(DISK,0,SEEK_SET);
    fread(&b0,sizeof(BLOCK0),1,DISK);
    printf("identify:%s\ninfo:%s\nrootblock:%d\ndatablock:%d\n",b0.identify,b0.info,b0.root,b0.startblock);
}

void exitsys(){//退出文件系统
    fclose(DISK);
}
