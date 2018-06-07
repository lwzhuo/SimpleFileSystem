#include<stdio.h>
#include"disk.h"
#include"../global/global.h"
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

int getFAT(FATitem * FAT,int fat_location){
    readFromDisk(DISK,FAT,FAT_ITEM_SIZE*FAT_ITEM_NUM,fat_location,0);
    return 0;
}

int changeFAT(FATitem *FAT,int fat_location){
    writeToDisk(DISK,FAT,FAT_ITEM_SIZE*FAT_ITEM_NUM,fat_location,0);
    return 0;
}

int getEmptyBlockId(int flag){
    if(!(flag==DATA_BLOCK||flag==FCB_BLOCK))
        return -1;
    int fcbflag=0,dataflag=0;
    for(int i=0;i<FAT_ITEM_NUM;i++){
        if(FAT1[i].item==UNUSED_BLOCK&&dataflag==0)
            dataflag = i;
        if(FAT1[i].item==FCB_BLOCK_NFULL&&fcbflag==0)
            fcbflag = i;
        if(dataflag!=0&&fcbflag!=0)
            break;
    }         
    if(flag==DATA_BLOCK)
        return dataflag; 
    else if(flag==FCB_BLOCK)
        return fcbflag;
}
