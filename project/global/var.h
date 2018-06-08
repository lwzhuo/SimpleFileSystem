#ifndef __VAR__
#define __VAR__
#include<stdio.h>
#include"define.h"
#include"../util/list.h"
typedef int Status;
typedef unsigned char byte;
typedef struct BLOCK0{
    char identify[10];
    char info[200];
    unsigned short root;//根目录块号
    int startblock;//数据块号
    int rootFCB;//根目录FCB位置
}BLOCK0;

typedef struct FCB{ //16B
    char name[11];
    unsigned type:1;//标志文件类型 1-目录 2-文件
    unsigned use:1;//标志使用状态 1-已使用 0-未使用
    unsigned short creatime;
    unsigned short moditime;
    unsigned int base;//文件起始盘块
    unsigned int length;//文件长度
}FCB;

typedef struct FATitem{//FAT表项 2B -32767-32768
    signed short item:16;
}FATitem;
#endif

