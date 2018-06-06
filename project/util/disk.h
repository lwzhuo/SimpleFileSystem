#include<stdio.h>
#ifndef __DISK__
#define __DISK__
#include"../global/var.h"
void createDisk();
int writeToDisk(FILE* DISK,void *ptr,int size,int base,long offset);
int readFromDisk(FILE* DISK,void *buff,int size,int base,long offset);
int getFAT(FILE * DISK,FATitem * fatarray);
#endif