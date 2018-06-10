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
    char name[FILE_NAME_LEN];
    unsigned type:1;//标志文件类型 1-目录 0-文件
    unsigned use:1;//标志使用状态 1-已使用(USED) 0-未使用(FREE)
    unsigned short time;
    unsigned short date;
    unsigned int base;//文件起始盘块
    unsigned int length;//文件长度
}FCB;

typedef struct FCBList{
    FCB fcb_entry;
    lslink link;
}FCBList;

typedef struct FATitem{//FAT表项 2B -32767-32768
    signed short item:16;
}FATitem;

typedef struct useropen{
    FCB fcb;
    char dir[80];
    int count;//文件指针的位置
    unsigned fcbstate:1;//标志fcb是否被修改 1-已修改 0-未修改
    unsigned topenfile:1;//标志使用状态 1-已使用(USED) 0-未使用(FREE)
    int blocknum;//所在块号
    int offset_in_block;//所在块号偏移量
}useropen;
#endif

