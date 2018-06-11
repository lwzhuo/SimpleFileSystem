#include<stdio.h>
#include"function/api.h"
#include"global/var.h"
#include"util/disk.h"
#include"shell/shell.h"
char sysname[20]="mydisk";
char pwd[80];
FILE * DISK;
BLOCK0 block0;
FATitem FAT1[FAT_ITEM_NUM];
FATitem FAT2[FAT_ITEM_NUM];
FCB presentFCB;
useropen uopenlist[MAX_FD_NUM];
int main()
{
    startsys();
    go();
    return 0;
}