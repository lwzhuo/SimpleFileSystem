#include<stdio.h>
#include"function/api.h"
#include"./global/var.h"
char sysname[100]="mydisk";
FILE * DISK;
BLOCK0 block0;
int main()
{
    startsys();
    printf("show Block0\n");
    showBlock0();
    printf("show FAT");
    // showFAT();
    printf("show FAT\n");
    showFCB(6,0);
    exitsys();
    return 0;
}