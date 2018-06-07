#include<errno.h>
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<unistd.h>
#include"../util/disk.h"
#include"../global/global.h"
void format(){//文件系统格式化
//引导块
    strcpy(block0.identify,"MYDISK");
    strcpy(block0.info,"blocksize:1024B\nblocknum:1024 Disksize:1MB");
    block0.root = 3;
    block0.startblock = 3*BLOCK_SIZE;

    fseek(DISK,0,SEEK_SET);
    fwrite(&block0,sizeof(BLOCK0),1,DISK);
//两个FAT块 每个占2个磁盘块 每个FAT表项2B大小
    FATitem *fat,fi;
    fat = (FATitem *)malloc(sizeof(FATitem)*BLOCK_NUMS*2);
    memset(fat,0,sizeof(FATitem));
    fseek(DISK,1*BLOCK_SIZE,SEEK_SET);
    fwrite(fat,sizeof(FATitem),BLOCK_NUMS*2,DISK);
    free(fat);
//根目录区 存放在第6个盘块(5号盘块)起始位
    DirItem_disk dd;
    dd.inode = BLOCK_SIZE*5+FCB_SIZE*0;//FCB位置
    strcpy(dd.name,".");
    writeToDisk(DISK,&dd,sizeof(DirItem_disk),BLOCK_SIZE*5,0);
//根目录FCB 存放在第7个盘块(6号盘块)起始位
    FCB rootFCB;
    rootFCB.type = 1;       //类型-目录
    rootFCB.full = 0;       //未存满
    rootFCB.creatime = 0;
    rootFCB.moditime = 0;
    rootFCB.base = 6;       //起始盘块号
    rootFCB.length = 1;     //长度
    writeToDisk(DISK,&rootFCB,sizeof(rootFCB),BLOCK_SIZE*6,0);
//修改对应FAT表
    fi.item = -1;
    for(int i=0;i<=6;i++){
        writeToDisk(DISK,&fi,sizeof(FATitem),FAT1_BLOCK_LOCATON,i*FAT_ITEM_SIZE);
        writeToDisk(DISK,&fi,sizeof(FATitem),FAT2_BLOCK_LOCATON,i*FAT_ITEM_SIZE);
    }
    fi.item = 1;
    writeToDisk(DISK,&fi,sizeof(FATitem),FAT1_BLOCK_LOCATON,6*FAT_ITEM_SIZE);
    writeToDisk(DISK,&fi,sizeof(FATitem),FAT2_BLOCK_LOCATON,6*FAT_ITEM_SIZE);
}

void startsys(){//初始化文件系统
    int fd;
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
    char buff[10];
    if(fread(&block0,sizeof(BLOCK0),1,DISK)==1)//取出第一个磁盘块(引导块)
//判断是否格式化            
    if(strcmp(block0.identify,"MYDISK")!=0){
        printf("磁盘尚未格式化，开始格式化\n");
        format();
    }
//进行其他初始化操作
    strcpy(pwd,"/");//设置当前目录
    return;         
}

void showBlock0(){
    BLOCK0 b0;
    fseek(DISK,0,SEEK_SET);
    fread(&b0,sizeof(BLOCK0),1,DISK);
    printf("identify:%s\ninfo:%s\nrootblock:%d\ndatablock:%d\n",b0.identify,b0.info,b0.root,b0.startblock);
}

void showFAT(){
    getFAT(DISK,FAT1);
    for(int i=0;i<FAT_ITEM_NUM;i++)
        printf("item %d:%d\n",i,FAT1[i].item);
}

void showFCB(int blocknum,int num_in_block){
    FCB fcb;
    readFromDisk(DISK,&fcb,sizeof(fcb),blocknum*BLOCK_SIZE,num_in_block*FCB_SIZE);
    printf("type %d\nfull %d\ncreate time %d\n\
modify time %d\nblocknum %d\nlength %d\n",
fcb.type,fcb.full,fcb.creatime,fcb.moditime,fcb.base,fcb.length);
}

void my_mkdir(char *dirname){
    
}
void exitsys(){//退出文件系统
    fclose(DISK);
}
