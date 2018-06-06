#include<stdio.h>
#include"function/api.h"
#include"./global/var.h"
char sysname[100]="mydisk";
FILE * DISK;
BLOCK0 block0;
int main()
{
    startsys();
    showBlock0();
    // showFAT();
    exitsys();
    return 0;
}