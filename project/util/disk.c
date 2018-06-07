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