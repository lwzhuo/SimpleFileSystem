#include<stdio.h>
#include"function/api.h"
#include"./global/var.h"
char sysname[100]="mydisk";
char pwd[16];
FILE * DISK;
BLOCK0 block0;
FATitem FAT1 [FAT_ITEM_NUM];
FATitem FAT2 [FAT_ITEM_NUM];
int main()
{
    startsys();
    printf("show Block0\n");
    showBlock0();
    printf("show FAT\n");
    showFAT();
    printf("show FCB\n");
    showFCB(6,0);
    exitsys();
    return 0;
}