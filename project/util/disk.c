#include<stdio.h>
#include"../global/global.h"
#include"../util/disk.h"
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