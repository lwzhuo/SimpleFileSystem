#include<stdio.h>
#include"define.h"
typedef int Status;
char sysname[100]="mydisk";
FILE * DISK;
typedef struct BLOCK0{
    char identify[10];
    char info[200];
    unsigned short root;
    int startblock;
}BLOCK0;
BLOCK0 block0;
