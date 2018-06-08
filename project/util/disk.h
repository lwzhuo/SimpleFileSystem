#include<stdio.h>
#ifndef __DISK__
#define __DISK__
#include"../global/var.h"
void createDisk();
int writeToDisk(FILE* DISK,void *ptr,int size,int base,long offset);
int readFromDisk(FILE* DISK,void *buff,int size,int base,long offset);
int getFAT(FATitem * fat,int fat_location);
int changeFAT(FATitem *fat,int fat_location);
void reloadFAT();
void rewriteFAT();
int initFCBBlock(int blocknum,int parentblocknum);
int addFCB(FCB fcb,int blocknum);
int removeFCB(int blocknum,int offset_in_block);
int findFCBInBlockByName(char *name,int blocknum);
int getEmptyFCBOffset(int blocknum);
int getFCB(FCB *fcb,int blocknum,int offset_in_block);
int getEmptyBlockId();
#endif