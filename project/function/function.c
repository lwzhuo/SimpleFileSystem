#include<errno.h>
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<unistd.h>
#include"../util/disk.h"
#include"../util/time.h"
#include"../global/global.h"
#include"../global/define.h"
void format(){//文件系统格式化
//引导块 BLOCK0
    strcpy(block0.identify,"MYDISK");
    strcpy(block0.info,"blocksize:1024B\nblocknum:1024 Disksize:1MB");
    block0.root = ROOT_FCB_LOCATION;
    block0.startblock = DATA_INIT_BLOCK;
    block0.rootFCB = ROOT_FCB_LOCATION;
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
    struct tm *t=getTimeStruct();
    FCB rootFCB;
    strcpy(rootFCB.name,"/");
    rootFCB.type = 1;       //类型-目录
    rootFCB.use = USED;        //已使用
    rootFCB.time = getTime(t);
    rootFCB.date = getDate(t);
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

int showFAT(int start,int end){
    getFAT(FAT1,FAT1_LOCATON);
    if(start>end||start<0||end>FAT_ITEM_NUM){
        printf("fat:invalid num\n");
        return -1;
    }
    for(int i=start,j=1;i<end;i++,j++){
         printf("item %d:%d",i,FAT1[i].item);
         if(j%10==0)
            printf("\n");
        else
            printf("\t");
    }
    printf("\n");
    return 0;
}

void showFCB(int blocknum,int num_in_block){
    FCB fcb;
    readFromDisk(DISK,&fcb,sizeof(fcb),blocknum*BLOCK_SIZE,num_in_block*FCB_SIZE);
    unsigned short time = fcb.time;
    unsigned short date = fcb.date;
    printf("*****block %d offset %d*****\n",blocknum,num_in_block);
    printf("name %s\ntype %d\nused %d\ncreate time %02d:%02d:%02d\n\
create date %4d/%02d/%02d\nblocknum %d\nlength %d\n",
fcb.name,fcb.type,fcb.use,
getHour(date,time),getMinute(time),getSecond(time),
getYear(date),getMonth(date),getDay(date),fcb.base,fcb.length);
}

void showPresentFCB(){
    unsigned short time = presentFCB.time;
    unsigned short date = presentFCB.date;
    printf("name %s\ntype %d\nused %d\ncreate time %02d:%02d:%02d\n\
create date %4d/%02d/%02d\nblocknum %d\nlength %d\n",
presentFCB.name,presentFCB.type,presentFCB.use,
getHour(date,time),getMinute(time),getSecond(time),
getYear(date),getMonth(date),getDay(date),
presentFCB.base,presentFCB.length);
}

void showBlockData(int blocknum){
    char buff[BLOCK_SIZE];
    readFromDisk(DISK,buff,BLOCK_SIZE,blocknum*BLOCK_SIZE,0);
    fputs(buff,stdout);
    printf("\nlen:%d bytes\n",(int)strlen(buff));
}

char *getPwd(){
    return pwd;
}

void showBlockChain(int blocknum){
    blockchain *blc;
    lslink *temp;
    int n=0;
    blc = getBlockChain(blocknum);
    list_for_each(temp,&(blc->link)){
        n++;
        blockchain *b = list_entry(temp,struct blockchain,link);
        if(b->link.next!=&(blc->link))
            printf("%d-",b->blocknum);
        else
            printf("%d",b->blocknum);
    }
    printf("\n");
    printf("%d blocks in total\n",n);
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
    struct tm *t=getTimeStruct();
    strcpy(fcb.name,dirname);
    fcb.type=1;
    fcb.use=USED;
    fcb.time=getTime(t);
    fcb.date=getDate(t);
    fcb.base=blocknum;
    fcb.length=1;
    initFCBBlock(blocknum,presentFCB.base);
    addFCB(fcb,presentFCB.base);//在当前目录表加入这个目录项
    rewriteFAT();
}

int my_rmdir(char *dirname){
//检查是否有这个目录
    int offset;
    FCB fcb;//将要删除的FCB
    if((offset=findFCBInBlockByName(dirname,presentFCB.base))<0){
        printf("rmdir: cannot remove '%s': No such file or directory\n",dirname);
        return -1;
    }else{
// 清空这个FCB指向的盘块
        getFCB(&fcb,presentFCB.base,offset);
    //判断是否为目录类型
        if(fcb.type!=1){
            printf("rmdir: cannot remove '%s': Is a file, please use rm\n",dirname);
            return -1;
        }
    //判断是否删除的是 .或 ..目录
        if(strcmp(fcb.name,".")==0||strcmp(fcb.name,"..")==0){
            printf("rmdir: refusing to remove '.' or '..'\n");
            return -1;
        }
    //判断这个目录是否为空
        for(int i=0;i<FCB_ITEM_NUM;i++){
            FCB f;
            getFCB(&f,fcb.base,i);
            if(f.use==USED){
                if(strcmp(f.name,".")!=0&&strcmp(f.name,"..")!=0){
                    printf("rmdir: cannot remove '%s': %s not empty\n",dirname,dirname);
                    return -1;
                }
            }
        }
    //修改FAT
        FAT1[fcb.base].item=FREE;
        FAT2[fcb.base].item=FREE;
        rewriteFAT();
    //删除这个FCB
        removeFCB(presentFCB.base,offset);
        return 0;
    }
}

void my_ls(){
    int blocknum = presentFCB.base;//获得当前fcb所在的盘块号
    lslink *FCBlisthead,*temp;
    FCBList FL,*Fnode;
    FCBlisthead = &(FL.link);
    unsigned short date;
    unsigned short time;
    printf("directory %s\n",pwd);
    printf("%-12s %-10s %-20s %-6s\n","name","type","time","length(bytes)");
    getFCBList(blocknum,FL,FCBlisthead);
    list_for_each(temp,FCBlisthead){
        Fnode = list_entry(temp,FCBList,link);
        date = Fnode->fcb_entry.date;
        time = Fnode->fcb_entry.time;
        printf("%-12s %-10s %4d/%02d/%02d %02d:%02d:%02d  %-6d\n",Fnode->fcb_entry.name,
        type[Fnode->fcb_entry.type],getYear(date),getMonth(date),getDay(date),
        getHour(date,time),getMinute(time),getSecond(time),Fnode->fcb_entry.length);
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
                struct tm *t=getTimeStruct();
                FCB fcb;
                strcpy(fcb.name,filename);
                fcb.type=0;
                fcb.use=USED;
                fcb.time=getTime(t);
                fcb.date=getDate(t);
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
        FCB fcb;
        getFCB(&fcb,presentFCB.base,offset);
    //判断文件类型
        if(fcb.type==1){
            printf("rm: cannot remove '%s': Is a directory\n",filename);
        return -1;
        }
    //修改FAT
        blockchain *blc;
        lslink *temp;
        blc = getBlockChain(fcb.base);
        list_for_each(temp,&(blc->link)){
            blockchain *b = list_entry(temp,struct blockchain,link);
            FAT1[b->blocknum].item=FREE;
            FAT2[b->blocknum].item=FREE;
        }
        FAT1[fcb.base].item=FREE;
        FAT2[fcb.base].item=FREE;
        rewriteFAT();
    //删除FCB
        removeFCB(presentFCB.base,offset);
        int fd = findfdByNameAndDir(filename,pwd);
        if(fd>=0&&uopenlist[fd].topenfile==USED)
            uopenlist[fd].topenfile=FREE;
        return 0;
    }
}

int my_open(char *filename){
    int fd;
    fd = findfdByNameAndDir(filename,pwd);
    if(fd>=0&&uopenlist[fd].topenfile==USED){//判断是否已经打开
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
            uopenlist[fd].count = BLOCK_SIZE*fcb.base+0;
            uopenlist[fd].fcbstate = 0;
            uopenlist[fd].topenfile = USED;
            uopenlist[fd].blocknum = presentFCB.base;
            uopenlist[fd].offset_in_block = findFCBInBlockByName(filename,presentFCB.base);
            printf("filename:%s fd:%d\n",filename,fd);
            return 0;
        }
    }
}

int my_close(int fd){
    if(fd>=MAX_FD_NUM||fd<0){
        printf("close: invalid fd\n");
        return -1;
    }else{
        if(uopenlist[fd].topenfile==FREE){//判断是否已经关闭
            printf("close: cannot close fd ‘%d’: fd %d is already close\n",fd,fd);
            return -1;
        }
        if(uopenlist[fd].fcbstate==1)//fcb被修改了
            changeFCB(uopenlist[fd].fcb,uopenlist[fd].blocknum,uopenlist[fd].offset_in_block);
        uopenlist[fd].topenfile=FREE;//清空文件打开表项
        return 0;
    }
}

//wstyle w-截断写 a-追加写 c-覆盖写
int my_write(int fd,int *sumlen,char wstyle){
    if(fd>=MAX_FD_NUM||fd<0){//判断fd合法性
        printf("close: invalid fd\n");
        return -1;
    }else{
        if(uopenlist[fd].topenfile==FREE){//判断是否已经关闭
            printf("write: cannot write to fd ‘%d’: fd %d is already close\n",fd,fd);
            return -1;
        }else{
            if(uopenlist[fd].fcb.type==1){//判断如果是目录
                printf("write: cannot write to fd ‘%d’: fd %d is a directory\n",fd,fd);
                return -1;
            }
            char str[BLOCK_SIZE],buff[BLOCK_SIZE];
            int blocknum,nextblocknum;
            int len,bloffset;//len-一次读取的长度 bloffset-文件指针块内偏移量
            *sumlen=0;//sumlen-总长(所有输入长度之和)
            memset(str,0,BLOCK_SIZE);
            memset(buff,0,BLOCK_SIZE);
        //截断写 将文件长度截断成0写
            if(wstyle=='w'){
                bloffset = 0;
                blocknum = uopenlist[fd].fcb.base;
            //做截断处理
                if(FAT1[blocknum].item!=END_OF_FILE){
                    blockchain *blc = getBlockChain(blocknum);
                    lslink *temp;
                    list_for_each(temp,&(blc->link)){
                        blockchain *b = list_entry(temp,struct blockchain,link);
                    //清空对应FAT块
                        FAT1[b->blocknum].item=FREE;
                        FAT2[b->blocknum].item=FREE;
                        uopenlist[fd].fcb.length=0;
                    }
                }
            //循环读取直到EOF 每次最多读取一个盘块大小的内容 多余部分留在缓冲区作为下次读取
                while(fgets(str,BLOCK_SIZE,stdin)!=NULL){
                    len = strlen(str);//记录实际读取到的长度
                    //printf("len %d\n",len);
                    if(bloffset+len<BLOCK_SIZE){
                //文件指针块内偏移量和将要输入的长度之和小于一个盘块--不停地输入到缓冲区中
                        *sumlen+=len;
                        bloffset+=len;
                        strcat(buff,str);
                    }else{
                //长度大于一个盘块
                        int lastlen=BLOCK_SIZE-bloffset-1;//要留出一位给\0作为结尾标志!!!!!!重要
                        int leavelen=len-lastlen;//计算剩下的长度
                        strncat(buff,str,lastlen);//填满上一个块
                        writeToDisk(DISK,buff,BLOCK_SIZE,blocknum*BLOCK_SIZE,0);//先把之前整个盘块的内容存放起来
                        memset(buff,'\0',BLOCK_SIZE);//重新初始化buff 清空原有数据 准备下一次输入
                    //修改总长度和块内指针位置
                        *sumlen+=len;
                        bloffset = leavelen;
                    //将剩下部分输入缓冲区
                        strcat(buff,str+lastlen);
                    //获得一个新的盘块
                        nextblocknum = getEmptyBlockId();
                    //判断空间是否充足  
                        if(nextblocknum<0){
                            printf("write: cannot write to fd ‘%d’: lack of space\n",fd);
                            return -1;
                        }
                    //标记使用状态(重要！！！)
                        FAT1[nextblocknum].item=END_OF_FILE;
                        FAT2[nextblocknum].item=END_OF_FILE;
                    //修改FAT
                        FAT1[blocknum].item=nextblocknum;
                        FAT2[blocknum].item=nextblocknum;
                        blocknum = nextblocknum;//修改当前块号
                    }
                }
                writeToDisk(DISK,buff,BLOCK_SIZE,blocknum*BLOCK_SIZE,0);
                uopenlist[fd].count=BLOCK_SIZE*blocknum+bloffset;
                uopenlist[fd].fcb.length = *sumlen;
            //修改对应FCB
                changeFCB(uopenlist[fd].fcb,uopenlist[fd].blocknum,uopenlist[fd].offset_in_block);
            //设置文件结尾
                FAT1[blocknum].item=END_OF_FILE;
                FAT2[blocknum].item=END_OF_FILE;
                rewriteFAT();
                return 0;
            }else if(wstyle=='a'){
        //追加写
                int currentlen=uopenlist[fd].fcb.length;//计算当前(写前)长度
            //计算块号
                blocknum=uopenlist[fd].fcb.base;
                while(FAT1[blocknum].item!=END_OF_FILE){
                    blocknum=FAT1[blocknum].item;
                }
                int oldblocknum = blocknum;//记录写前最后一个块号
            //计算块内偏移量
                bloffset=currentlen%BLOCK_SIZE;
                //循环读取直到EOF 每次最多读取一个盘块大小的内容 多余部分留在缓冲区作为下次读取
                while(fgets(str,BLOCK_SIZE,stdin)!=NULL){
                    len = strlen(str);//记录实际读取到的长度
                    if(bloffset+len<BLOCK_SIZE){
                    //文件指针块内偏移量和将要输入的长度之和小于一个盘块--不停地输入到缓冲区中
                        *sumlen+=len;
                        bloffset+=len;
                        strcat(buff,str);
                    }else{
                    //长度大于一个盘块
                        int lastlen=BLOCK_SIZE-bloffset-1;//要留出一位给\0作为结尾标志!!!!!!重要
                        int leavelen=len-lastlen;//计算剩下的长度
                        strncat(buff,str,lastlen);//填满上一个块
                        if(blocknum==oldblocknum)//和w的区别 这里要分类处理 讨论是不是要填满原有的块
                            writeToDisk(DISK,buff,strlen(buff),blocknum*BLOCK_SIZE,BLOCK_SIZE-strlen(buff)-1);//填满上一次没有写满盘块
                        else
                            writeToDisk(DISK,buff,BLOCK_SIZE,blocknum*BLOCK_SIZE,0);//往一个全新的盘块写
                        memset(buff,'\0',BLOCK_SIZE);//重新初始化buff 清空原有数据 准备下一次输入
                    //修改总长度和块内指针位置
                        *sumlen+=len;
                        bloffset = leavelen;
                    //将剩下部分输入缓冲区
                        strcat(buff,str+lastlen);
                    //获得一个新的盘块
                        nextblocknum = getEmptyBlockId();
                    //判断空间是否充足  
                        if(nextblocknum<0){
                            printf("write: cannot write to fd ‘%d’: lack of space\n",fd);
                            return -1;
                        }
                    //标记使用状态(重要！！！)
                        FAT1[nextblocknum].item=END_OF_FILE;
                        FAT2[nextblocknum].item=END_OF_FILE;
                    //修改FAT
                        FAT1[blocknum].item=nextblocknum;
                        FAT2[blocknum].item=nextblocknum;
                        blocknum = nextblocknum;//修改当前块号
                    }
                }
                if(blocknum==oldblocknum)//和w的区别 这里要分类处理 讨论是不是要填满原有的第一个块
                    writeToDisk(DISK,buff,strlen(buff),blocknum*BLOCK_SIZE,bloffset-*sumlen);//填满上一次没有写满盘块
                else
                    writeToDisk(DISK,buff,BLOCK_SIZE,blocknum*BLOCK_SIZE,0);//往一个全新的盘块写
                //writeToDisk(DISK,buff,strlen(buff),blocknum*BLOCK_SIZE,bloffset-strlen(buff));
                uopenlist[fd].count=BLOCK_SIZE*blocknum+bloffset;
                uopenlist[fd].fcb.length = currentlen+*sumlen;
            //修改对应FCB
                changeFCB(uopenlist[fd].fcb,uopenlist[fd].blocknum,uopenlist[fd].offset_in_block);
            //文件结尾
                FAT1[blocknum].item=END_OF_FILE;
                FAT2[blocknum].item=END_OF_FILE;
                rewriteFAT();
                return 0; 
            }else if(wstyle=='c'){
        //覆盖写 在当前指针后继续写
                int currentlen=uopenlist[fd].fcb.length;//计算当前(写前)长度
                bloffset=uopenlist[fd].count%BLOCK_SIZE;//计算块内偏移量
                blocknum=uopenlist[fd].count/BLOCK_SIZE;//计算块号
                int oldblocknum = blocknum;//记录写前最后一个块号
                //循环读取直到EOF 每次最多读取一个盘块大小的内容 多余部分留在缓冲区作为下次读取
                while(fgets(str,BLOCK_SIZE,stdin)!=NULL){
                    len = strlen(str);//记录实际读取到的长度
                    if(bloffset+len<BLOCK_SIZE){
                    //文件指针块内偏移量和将要输入的长度之和小于一个盘块--不停地输入到缓冲区中
                        *sumlen+=len;
                        bloffset+=len;
                        strcat(buff,str);
                    }else{
                    //长度大于一个盘块
                        int lastlen=BLOCK_SIZE-bloffset-1;//要留出一位给\0作为结尾标志!!!!!!重要
                        int leavelen=len-lastlen;//计算剩下的长度
                        strncat(buff,str,lastlen);//填满上一个块
                        if(blocknum==oldblocknum)//和w的区别 这里要分类处理 讨论是不是要填满原有的第一个块
                            writeToDisk(DISK,buff,strlen(buff),blocknum*BLOCK_SIZE,bloffset);//填满上一次没有写满盘块
                        else
                            writeToDisk(DISK,buff,BLOCK_SIZE,blocknum*BLOCK_SIZE,0);//往一个全新的盘块写
                        //writeToDisk(DISK,buff,strlen(buff),blocknum*BLOCK_SIZE,bloffset);//填满上一个盘块
                        memset(buff,'\0',BLOCK_SIZE);//重新初始化buff 清空原有数据 准备下一次输入
                    //修改总长度和块内指针位置
                        *sumlen+=len;
                        bloffset = leavelen;
                    //将剩下部分输入缓冲区
                        strcat(buff,str+lastlen);
                    //获得一个新的盘块
                        nextblocknum = getEmptyBlockId();
                    //判断空间是否充足  
                        if(nextblocknum<0){
                            printf("write: cannot write to fd ‘%d’: lack of space\n",fd);
                            return -1;
                        }
                    //标记使用状态(重要！！！)
                        FAT1[nextblocknum].item=END_OF_FILE;
                        FAT2[nextblocknum].item=END_OF_FILE;
                    //修改FAT
                        FAT1[blocknum].item=nextblocknum;
                        FAT2[blocknum].item=nextblocknum;
                        blocknum = nextblocknum;//修改当前块号
                    }
                }
                if(blocknum==oldblocknum)//和w的区别 这里要分类处理 讨论是不是要填满原有的第一个块
                    writeToDisk(DISK,buff,strlen(buff),blocknum*BLOCK_SIZE,bloffset-strlen(buff));//填满上一次没有写满盘块
                else
                    writeToDisk(DISK,buff,BLOCK_SIZE,blocknum*BLOCK_SIZE,0);//往一个全新的盘块写
                uopenlist[fd].count=BLOCK_SIZE*blocknum+bloffset;
                uopenlist[fd].fcb.length = currentlen+*sumlen;
            //修改对应FCB
                changeFCB(uopenlist[fd].fcb,uopenlist[fd].blocknum,uopenlist[fd].offset_in_block);
            //文件结尾
                FAT1[blocknum].item=END_OF_FILE;
                FAT2[blocknum].item=END_OF_FILE;
                rewriteFAT();
                return 0;
            }else{
                printf("write: invalid write type ‘%c’\n",wstyle);
                return -1;
            }
        } 
    }
}

int my_read(int fd,int *sumlen){
    if(fd>=MAX_FD_NUM||fd<0){
        printf("read: invalid fd\n");
        return -1;
    }else{
        if(uopenlist[fd].topenfile==FREE){//判断是否已经关闭
            printf("read: cannot read to fd ‘%d’: fd %d is already close\n",fd,fd);
            return -1;
        }else{
            if(uopenlist[fd].fcb.type==1){//判断如果是目录
                printf("read: cannot read to fd ‘%d’: fd %d is a directory\n",fd,fd);
                return -1;
            }
            char buff[BLOCK_SIZE];
            int blocknum = uopenlist[fd].fcb.base;
            while(blocknum!=END_OF_FILE){
                readFromDisk(DISK,buff,BLOCK_SIZE,blocknum*BLOCK_SIZE,0);
                *sumlen += strlen(buff);
                fputs(buff,stdout);
                blocknum=FAT1[blocknum].item;
            }
            return 0;
        }
    }
}

int my_in(int fd,char *filename,int *sumlen){
    if(fd>=MAX_FD_NUM||fd<0){//判断fd合法性
        printf("close: invalid fd\n");
        return -1;
    }else{
        if(uopenlist[fd].topenfile==FREE){//判断是否已经关闭
            printf("write: cannot write to fd ‘%d’: fd %d is already close\n",fd,fd);
            return -1;
        }else{
            if(uopenlist[fd].fcb.type==1){//判断如果是目录
                printf("write: cannot write to fd ‘%d’: fd %d is a directory\n",fd,fd);
                return -1;
            }
            char buff[BLOCK_SIZE];
            int blocknum,nextblocknum;
            int len,bloffset;//len-一次读取的长度 bloffset-文件指针块内偏移量
            FILE *f = fopen(filename,"rb");
            if(f==NULL){
                printf("open file %s failure\n",filename);
                return -1;
            }
            *sumlen=0;//sumlen-总长(所有输入长度之和)
            memset(buff,0,BLOCK_SIZE);
        //截断写 将文件长度截断成0写
            blocknum = uopenlist[fd].fcb.base;
        //做截断处理
            if(FAT1[blocknum].item!=END_OF_FILE){
                blockchain *blc = getBlockChain(blocknum);
                lslink *temp;
                list_for_each(temp,&(blc->link)){
                    blockchain *b = list_entry(temp,struct blockchain,link);
                //清空对应FAT块
                    FAT1[b->blocknum].item=FREE;
                    FAT2[b->blocknum].item=FREE;
                    uopenlist[fd].fcb.length=0;
                }
            }
        //循环读取直到EOF 每次最多读取一个盘块大小的内容 多余部分留在缓冲区作为下次读取
            while(!feof(f)){
                len=fread(buff,1,BLOCK_SIZE,f);
                if(len<BLOCK_SIZE){
            //文件指针块内偏移量和将要输入的长度之和小于一个盘块--不停地输入到缓冲区中
                    *sumlen+=len;
                    writeToDisk(DISK,buff,BLOCK_SIZE,blocknum*BLOCK_SIZE,0);
                }else{
            //长度大于一个盘块
                    writeToDisk(DISK,buff,BLOCK_SIZE,blocknum*BLOCK_SIZE,0);//先把之前整个盘块的内容存放起来
                    memset(buff,'\0',BLOCK_SIZE);//重新初始化buff 清空原有数据 准备下一次输入
                //修改总长度
                    *sumlen+=len;
                //获得一个新的盘块
                    nextblocknum = getEmptyBlockId();
                //判断空间是否充足  
                    if(nextblocknum<0){
                        printf("write: cannot write to fd ‘%d’: lack of space\n",fd);
                        return -1;
                    }
                //标记使用状态(重要！！！)
                    FAT1[nextblocknum].item=END_OF_FILE;
                    FAT2[nextblocknum].item=END_OF_FILE;
                //修改FAT
                    FAT1[blocknum].item=nextblocknum;
                    FAT2[blocknum].item=nextblocknum;
                    blocknum = nextblocknum;//修改当前块号
                }
            }
            uopenlist[fd].count=BLOCK_SIZE*blocknum+len;
            uopenlist[fd].fcb.length = *sumlen;
        //修改对应FCB
            changeFCB(uopenlist[fd].fcb,uopenlist[fd].blocknum,uopenlist[fd].offset_in_block);
        //设置文件结尾
            FAT1[blocknum].item=END_OF_FILE;
            FAT2[blocknum].item=END_OF_FILE;
            rewriteFAT();
            fclose(f);
            return 0;
        }
    }
}

int my_out(int fd,char *filename,int *sumlen){
    if(fd>=MAX_FD_NUM||fd<0){
        printf("read: invalid fd\n");
        return -1;
    }else{
        if(uopenlist[fd].topenfile==FREE){//判断是否已经关闭
            printf("read: cannot read to fd ‘%d’: fd %d is already close\n",fd,fd);
            return -1;
        }else{
            if(uopenlist[fd].fcb.type==1){//判断如果是目录
                printf("read: cannot read to fd ‘%d’: fd %d is a directory\n",fd,fd);
                return -1;
            }
            FILE *f = fopen(filename,"wb");
            char buff[BLOCK_SIZE];
            int blocknum = uopenlist[fd].fcb.base;
            int length = uopenlist[fd].fcb.length;
            while(FAT1[blocknum].item!=END_OF_FILE){
                readFromDisk(DISK,buff,BLOCK_SIZE,blocknum*BLOCK_SIZE,0);
                *sumlen += BLOCK_SIZE;
                fwrite(buff,BLOCK_SIZE,1,f);
                blocknum=FAT1[blocknum].item;
            }
            readFromDisk(DISK,buff,BLOCK_SIZE,blocknum*BLOCK_SIZE,0);
            *sumlen += length%BLOCK_SIZE;
                fwrite(buff,length%BLOCK_SIZE,1,f);
            fclose(f);
            return 0;
        }
    }
}
void showfdList(){
    int num=0;
    for(int i=0;i<MAX_FD_NUM;i++){
        if(uopenlist[i].topenfile==USED){
            num++;
            printf("%-2d fd:%d name:%s dir:%s fileptr:%d [FCB in block:%d FCB offset:%d]\n",num,i,
            uopenlist[i].fcb.name,uopenlist[i].dir,uopenlist[i].count,
            uopenlist[i].blocknum,uopenlist[i].offset_in_block);
        }
    }
}

void exitsys(){//退出文件系统
    fclose(DISK);
}
