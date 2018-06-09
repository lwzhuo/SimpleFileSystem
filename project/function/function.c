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
//引导块 BLOCK0
    strcpy(block0.identify,"MYDISK");
    strcpy(block0.info,"blocksize:1024B\nblocknum:1024 Disksize:1MB");
    block0.root = 5;
    block0.startblock = 6;
    block0.rootFCB = 6;
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
    FCB rootFCB;
    strcpy(rootFCB.name,"/");
    rootFCB.type = 1;       //类型-目录
    rootFCB.use = USED;        //已使用
    rootFCB.creatime = 0;
    rootFCB.moditime = 0;
    rootFCB.base = 6;       //起始盘块号
    rootFCB.length = 1;     //长度
    FAT1[5].item=FCB_BLOCK;
    FAT2[5].item=FCB_BLOCK;
    addFCB(rootFCB,5);
//根目录FCB 存放在第7个盘块(6号盘块)起始位
    initFCBBlock(6,6);
//修改对应FAT表
    fi.item = END_OF_FILE;
    for(int i=0;i<5;i++)
        FAT1[i].item=USED;
    rewriteFAT();
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
    presentFCBblocknum = 5;//设置当前的FCB盘块号
    getFCB(&presentFCB,5,0);//将根目录设置成当前内存中的FCB
    getFAT(FAT1,FAT1_LOCATON);//加载FAT1
    getFAT(FAT2,FAT2_LOCATON);//加载FAT2
    return;         
}

void showBlock0(){
    BLOCK0 b0;
    fseek(DISK,0,SEEK_SET);
    fread(&b0,sizeof(BLOCK0),1,DISK);
    printf("identify:%s\ninfo:%s\nrootblock:%d\ndatablock:%d\nrootFCB:%d\n",
    b0.identify,b0.info,b0.root,b0.startblock,b0.rootFCB);
}

void showFAT(){
    getFAT(FAT1,FAT1_LOCATON);
    for(int i=0,j=1;i<FAT_ITEM_NUM;i++,j++){
         printf("item %d:%d",i,FAT1[i].item);
         if(j%10==0)
            printf("\n");
        else
            printf("\t");
    }
    printf("\n");
}

void showFCB(int blocknum,int num_in_block){
    FCB fcb;
    readFromDisk(DISK,&fcb,sizeof(fcb),blocknum*BLOCK_SIZE,num_in_block*FCB_SIZE);
    printf("*****block %d offset %d*****\n",blocknum,num_in_block);
    printf("%s type %d\nused %d\ncreate time %d\n\
modify time %d\nblocknum %d\nlength %d\n",
fcb.name,fcb.type,fcb.use,fcb.creatime,fcb.moditime,fcb.base,fcb.length);
}

char *getPwd(){
    return pwd;
}

int my_mkdir(char *dirname){
//检查是否重名
    if(findFCBInBlockByName(dirname,presentFCB.base)>0){
        printf("mkdir: cannot create directory ‘%s’: File exists\n",dirname);
        return -1;
    } 
//获得当前目录表中空余位置，以便存放FCB
    int offset = getEmptyFCBOffset(presentFCB.base);
    if(offset<0){
        printf("mkdir:no empty space to make dirctory\n");
        return -1;
    }
//获得空的盘块以便存储新的目录表
    int blocknum = getEmptyBlockId();
    FCB fcb;
    if(blocknum<0){
        printf("mkdir:no empty block\n");
        return -1;
    }
//构造FCB
    strcpy(fcb.name,dirname);
    fcb.type=1;
    fcb.use=USED;
    fcb.creatime=2018;
    fcb.moditime=2018;
    fcb.base=blocknum;
    fcb.length=1;
    initFCBBlock(blocknum,presentFCBblocknum);
    addFCB(fcb,presentFCB.base);
//修改FAT
    FAT1[blocknum].item=END_OF_FILE;
    FAT2[blocknum].item=END_OF_FILE;
    rewriteFAT();
}



void my_ls(){
    int blocknum = presentFCB.base;
    lslink *FCBlisthead,*temp;
    FCBList FL,*Fnode;
    FCBlisthead = &(FL.link);

    getFCBList(blocknum,FL,FCBlisthead);
    list_for_each(temp,FCBlisthead){
        Fnode = list_entry(temp,FCBList,link);
        printf("name %s type:%d\n",Fnode->fcb_entry.name,Fnode->fcb_entry.type);
    }
}

void exitsys(){//退出文件系统
    fclose(DISK);
}
