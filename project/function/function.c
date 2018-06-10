#include<errno.h>
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<unistd.h>
#include"../util/disk.h"
#include"../global/global.h"
#include"../global/define.h"
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
//根目录区 存放在5号盘块
    FCB rootFCB;
    strcpy(rootFCB.name,"/");
    rootFCB.type = 1;       //类型-目录
    rootFCB.use = USED;        //已使用
    rootFCB.time = 0;
    rootFCB.date = 0;
    rootFCB.base = 6;       //起始盘块号
    rootFCB.length = 1;     //长度
    FAT1[5].item=FCB_BLOCK;
    FAT2[5].item=FCB_BLOCK;
    addFCB(rootFCB,5);
//根目录FCB 存放在6号盘块
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
//初始化文件打开表
    for(int i=0;i<MAX_FD_NUM;i++)
        uopenlist[i].topenfile=FREE;//全部设置成0
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
create date %d\nblocknum %d\nlength %d\n",
fcb.name,fcb.type,fcb.use,fcb.time,fcb.date,fcb.base,fcb.length);
}

char *getPwd(){
    return pwd;
}

int my_mkdir(char *dirname){
//判断文件名长度
    if(strlen(dirname)>FILE_NAME_LEN){
        printf("mkdir: cannot create directory ‘%s’: directory name must less than %d bytes\n",dirname,FILE_NAME_LEN);
        return -1;
    }
//检查是否重名
    if(findFCBInBlockByName(dirname,presentFCB.base)>0){
        printf("mkdir: cannot create directory ‘%s’: File exists\n",dirname);
        return -1;
    } 
//获得当前目录表中空余位置，以便存放FCB
    int offset = getEmptyFCBOffset(presentFCB.base);
    if(offset<0){
        printf("mkdir:no empty space to make directory\n");
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
    fcb.time=2018;
    fcb.date=2018;
    fcb.base=blocknum;
    fcb.length=1;
    initFCBBlock(blocknum,presentFCB.base);
    addFCB(fcb,presentFCB.base);//在当前目录表加入这个目录项
//修改FAT
    FAT1[blocknum].item=END_OF_FILE;
    FAT2[blocknum].item=END_OF_FILE;
    rewriteFAT();
}

// void my_rmdir(char *dirname){
// //检查是否有这个目录
//     int offset;
//     FCB fcb;
//     if((offset=findFCBInBlockByName(dirname,presentFCBblocknum))<0){
//         printf("rm: cannot remove '%s': No such file or directory",dirname);
//         return -1;
//     }else{
//     // 清空这个FCB指向的盘块
//         getFCB(&fcb,presentFCB,offset);
//         FAT1[fcb.base].item=FREE;
//         FAT2[fcb.base].item=FREE;
//         rewriteFAT();
//     //删除这个FCB
//         removeFCB(presentFCBblocknum,offset);
    
//     }
// }

void my_ls(){
    int blocknum = presentFCB.base;//获得当前fcb所在的盘块号
    lslink *FCBlisthead,*temp;
    FCBList FL,*Fnode;
    FCBlisthead = &(FL.link);

    printf("directory %s\n",pwd);
    getFCBList(blocknum,FL,FCBlisthead);
    list_for_each(temp,FCBlisthead){
        Fnode = list_entry(temp,FCBList,link);
        printf("%-12s type:%d\n",Fnode->fcb_entry.name,Fnode->fcb_entry.type);
    }
}

int my_cd(char *dirname){
    int offset;
    if((offset=findFCBInBlockByName(dirname,presentFCB.base))<0){
        printf("cd: %s: No such file or directory\n",dirname);
        return -1;
    }else{
        FCB fcb;
        getFCB(&fcb,presentFCB.base,offset);
        if(fcb.type==0){
            printf("cd: %s: Not a directory\n",dirname);
            return -1;
        }
        presentFCBblocknum = presentFCB.base;//修改当前fcb所在的盘块号
        presentFCB=fcb;//修改当前fcb值
        if(strcmp(dirname,".")==0)//当前目录
            ;
        else if(strcmp(dirname,"..")==0){//上一级目录
            if(strcmp(pwd,"/")!=0){//不是根目录情况
                char *a = strchr(pwd,'/');//从左往右第一次出现/的位置
                char *b = strrchr(pwd,'/');//从右往左第一次出现/的位置
                if(a!=b)//判断是否只有一个/字符 不相等则有多个
                    *b='\0';
                else
                    *(b+1)='\0';
            }
        }
        else{//下一级目录
            if(strcmp(pwd,"/")!=0)
                strcat(pwd,"/");
            strcat(pwd,dirname);
        }
        return 0;
    }    
}

int my_create(char *filename){
    //判断文件名长度
    if(strlen(filename)>FILE_NAME_LEN){
        printf("create: cannot create file ‘%s’: file name must less than %d bytes\n",filename,FILE_NAME_LEN);
        return -1;
    }
    int offset = findFCBInBlockByName(filename,presentFCB.base);
    if(offset>0){//判断是否已经存在此文件
        printf("create: cannot create file ‘%s’: File exists\n",filename);
        return -1;
    }else{
        offset = getEmptyFCBOffset(presentFCB.base);//判断FCB块是否有剩余空间加入FCB
        if(offset<0){
            printf("create: cannot create file ‘%s’: %s Lack of space\n",pwd,filename);
            return -1;
        }else{
            int blocknum = getEmptyBlockId();
            if(blocknum<0){
                printf("create: cannot create file ‘%s’: %s Lack of space\n",sysname,filename);
                return -1;
            }else{
            //构建FCB
                FCB fcb;
                strcpy(fcb.name,filename);
                fcb.type=0;
                fcb.use=USED;
                fcb.time=2018;
                fcb.date=2018;
                fcb.base=blocknum;
                fcb.length=1;
                addFCB(fcb,presentFCB.base);
            //修改FAT
                FAT1[blocknum].item=END_OF_FILE;
                FAT2[blocknum].item=END_OF_FILE;
                rewriteFAT();
                return 0;
            }
        }
    }
    return 0;
}

int my_rm(char *filename){
    int offset = findFCBInBlockByName(filename,presentFCB.base);
    if(offset<0){//判断是否已经存在此文件
        printf("rm: cannot remove '%s': No such file\n",filename);
        return -1;
    }else{
    //修改FAT
        FCB fcb;
        getFCB(&fcb,presentFCB.base,offset);
        FAT1[fcb.base].item=FREE;
        FAT2[fcb.base].item=FREE;
        rewriteFAT();
    //删除FCB
        removeFCB(presentFCB.base,offset);
        return 0;
    }
}

int my_open(char *filename){
    int fd;
    fd = findfdByNameAndDir(filename,pwd);
    if(fd>0){//判断是否已经打开
        printf("open: cannot open file ‘%s’: %s is already open\n",filename,filename);
        return -1;
    }
    if((fd=getEmptyfd())<0){//查看是否有空的fd
        printf("open: cannot open file ‘%s’: Lack of empty fd\n",filename);
        return -1;
    }else{
        int offset = findFCBInBlockByName(filename,presentFCB.base);//获得fcb位置
        if(offset<0){
            printf("open: %s: No such file or directory\n",filename);
            return -1;
        }else{
        //构造打开表项
            FCB fcb;
            getFCB(&fcb,presentFCB.base,offset);
            uopenlist[fd].fcb = fcb;
            strcpy(uopenlist[fd].dir,pwd);
            uopenlist[fd].count = 0;
            uopenlist[fd].fcbstate = 0;
            uopenlist[fd].topenfile = 1;
            return 0;
        }
    }
}

void showfdList(){
    int num=0;
    for(int i=0;i<MAX_FD_NUM;i++){
        if(uopenlist[i].topenfile==USED){
            num++;
            printf("%d %s %s %d\n",num,uopenlist[i].fcb.name,uopenlist[i].dir,uopenlist[i].count);
        }
    }
}

void exitsys(){//退出文件系统
    fclose(DISK);
}
