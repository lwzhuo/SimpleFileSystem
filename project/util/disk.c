#include<stdio.h>
#include<string.h>
#include"disk.h"
#include"../global/global.h"
#include"list.h"
void createDisk(){
    char buff[BLOCK_SIZE]={'0'};
    FILE *f = fopen(sysname,"r+");
    for(int i=0;i<BLOCK_NUMS;i++)
        fwrite(buff,sizeof(buff),1,f);
    fclose(f);
}

int writeToDisk(FILE* DISK,void *ptr,int size,int base,long offset){
    if(base<=0)
        fseek(DISK,offset,SEEK_SET);
    else
        fseek(DISK,base+offset,SEEK_SET);
    fwrite(ptr,size,1,DISK);
    return 0;
}

int readFromDisk(FILE* DISK,void *buff,int size,int base,long offset){
    if(base<=0)
        fseek(DISK,offset,SEEK_SET);
    else
        fseek(DISK,base+offset,SEEK_SET);
    fread(buff,size,1,DISK);
    return 0;
}

int getFAT(FATitem * fat,int fat_location){
    readFromDisk(DISK,fat,FAT_ITEM_SIZE*FAT_ITEM_NUM,fat_location,0);
    return 0;
}

int changeFAT(FATitem *fat,int fat_location){
    writeToDisk(DISK,fat,FAT_ITEM_SIZE*FAT_ITEM_NUM,fat_location,0);
    return 0;
}

void reloadFAT(){
    getFAT(FAT1,FAT1_LOCATON);
    getFAT(FAT2,FAT2_LOCATON);
}

void rewriteFAT(){
    changeFAT(FAT1,FAT1_LOCATON);
    changeFAT(FAT2,FAT2_LOCATON);
}
//初始化一个目录块 blocknum-将要初始化的块号 parentblocknum-父目录块号
int initFCBBlock(int blocknum,int parentblocknum){
//修改FAT
    FAT1[blocknum].item=FCB_BLOCK;
    FAT2[blocknum].item=FCB_BLOCK;
    rewriteFAT();
//加入.和..目录
//.目录
    FCB fcb;
    strcpy(fcb.name,".");
    fcb.type=1;
    fcb.use=USED;
    fcb.creatime=2018;
    fcb.moditime=2018;
    fcb.base=blocknum;//指向当前目录
    fcb.length=1;
    addFCB(fcb,blocknum);
//..目录
    strcpy(fcb.name,"..");
    fcb.type=1;
    fcb.use=USED;
    fcb.creatime=2018;
    fcb.moditime=2018;
    fcb.base=parentblocknum;//指向父目录
    fcb.length=1;
    addFCB(fcb,blocknum);
    return 0;
}

int addFCB(FCB fcb,int blocknum){
    int offset = getEmptyFCBOffset(blocknum);
    if(offset==-1)
        return -1;
    else{
        writeToDisk(DISK,&fcb,sizeof(FCB),blocknum*BLOCK_SIZE,offset*FCB_SIZE);
        return 0;
    }
}
//根据名字在当前目录块寻找对应(已被使用的)FCB
int findFCBInBlockByName(char *name,int blocknum){
    FCB fcb[FCB_ITEM_NUM];
    int offset=-1;
    readFromDisk(DISK,fcb,sizeof(FCB)*FCB_ITEM_NUM,blocknum*BLOCK_SIZE,0);
    for(int i=0;i<FCB_ITEM_NUM;i++){
        if(fcb[i].use==USED){
            if(strcmp(fcb[i].name,name)==0)
            offset=i;
        }
    }
    return offset;
}

int removeFCB(int blocknum,int offset_in_block){
    FCB fcb;
    readFromDisk(DISK,&fcb,sizeof(FCB),blocknum*BLOCK_SIZE,offset_in_block*BLOCK_SIZE);
    fcb.use=0;
    writeToDisk(DISK,&fcb,sizeof(FCB),blocknum*BLOCK_SIZE,offset_in_block*BLOCK_SIZE);
    return 0;
}

int getEmptyFCBOffset(int blocknum){
    FCB fcblist[FCB_ITEM_NUM];
    readFromDisk(DISK,fcblist,sizeof(FCB)*FCB_ITEM_NUM,blocknum*BLOCK_SIZE,0);
    int i;
    for(i=0;i<FCB_ITEM_NUM;i++)
        if(fcblist[i].use==FREE)
            return i;
    if(i==FCB_ITEM_NUM)
        return -1;
}

int getFCB(FCB *fcb,int blocknum,int offset_in_block){
    readFromDisk(DISK,fcb,sizeof(FCB),blocknum*BLOCK_SIZE,offset_in_block*FCB_SIZE);
    return 0;
}

int getFCBList(int blocknum,FCBList FLstruct,lslink *fcblisthead){
    FCB fcblist[FCB_ITEM_NUM];
    readFromDisk(DISK,&fcblist,sizeof(FCB)*FCB_ITEM_NUM,blocknum*BLOCK_SIZE,0);
    list_init(fcblisthead,&FLstruct);
    for(int i=0;i<FCB_ITEM_NUM;i++){
        if(fcblist[i].use==USED){
            FCBList *temp = get_node(FCBList);
            temp->fcb_entry = fcblist[i];
            list_insert(fcblisthead,&(temp->link),temp);
        }  
    }
}

int getEmptyBlockId(){
    int flag=0;
    for(int i=0;i<FAT_ITEM_NUM;i++){
        if(FAT1[i].item==FREE&&flag==0){
            flag = i;
            break;
        }   
    }         
    if(flag==0)
        return -1;
    else
        return flag;
}
