#include<stdio.h>
#ifndef __DISK__
#define __DISK__
#include"../global/var.h"
void createDisk();
int writeToDisk(FILE* DISK,void *ptr,int size,int base,long offset);
int readFromDisk(FILE* DISK,void *buff,int size,int base,long offset);
int getFAT(FATitem * FAT,int fat_location);
int changeFAT(FATitem *FAT,int fat_location);
int getEmptyBlockId();
int initFCBBlock(int base);
int addFCB(FCB fcb,int blocknum,int offset_in_block);
#endif