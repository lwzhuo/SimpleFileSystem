/*存储整个项目用到的全局变量和字符一些符号常量*/
#include<stdio.h>
char sysname[100];
FILE * DISK;
typedef struct BLOCK0{
    char identify[10];
    char info[200];
    unsigned short root;
    int startblock;
}BLOCK0;
BLOCK0 block0;