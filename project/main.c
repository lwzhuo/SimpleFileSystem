#include<stdio.h>
#include"function/api.h"
#include"./global/var.h"
#include"util/disk.h"
char sysname[100]="mydisk";
char pwd[16];
int presentFCBblocknum;
FILE * DISK;
BLOCK0 block0;
FATitem FAT1 [FAT_ITEM_NUM];
FATitem FAT2 [FAT_ITEM_NUM];
FCB presentFCB;
int main()
{
    startsys();
    printf("**********show Block0**********\n");
    showBlock0();
    printf("**********show FAT**********\n");
    showFAT();
    printf("**********show FCB**********\n");
    showFCB(5,0);
    showFCB(6,0);
    showFCB(6,1);
    showFCB(6,2);
    showFCB(6,3);
//修改FAT
    // FAT1[20].item=10;
    // rewriteFAT();
    // reloadFAT();
    // showFAT();
//获得首块空闲磁盘块id
    // printf("empty block %d\n",getEmptyBlockId(DATA_BLOCK));
    // printf("fcb block %d\n",getEmptyBlockId(FCB_BLOCK));
//创建新的目录
    my_mkdir("hhhhh");
//显示刚刚创建的目录信息
    showFCB(7,0);
    showFCB(7,1);
    showFCB(7,2);
    exitsys();
    return 0;
}