#ifndef __VAR__
#define __VAR__
#include<stdio.h>
#include"define.h"
#include"../util/list.h"
typedef int Status;
typedef unsigned char byte;
typedef struct lslink lslink;
typedef struct lslink{
    void * sptr;
    lslink *next;
    lslink *prev;
}lslink;
typedef struct BLOCK0{
    char identify[10];
    char info[200];
    unsigned short root;
    int startblock;
}BLOCK0;

typedef struct FCB{ //16B
    unsigned type:1;//标志文件类型 1-目录 2-文件
    unsigned full:1;//标志目录项是否满(仅在文件类型为目录时判断此位) 1-满 0-有空余位置
    unsigned short creatime;
    unsigned short moditime;
    unsigned int base;//文件起始盘块
    unsigned int length;//文件占用盘块数
}FCB;

typedef struct FCB_Block_info{
    unsigned long use:64;
}FBi;

typedef struct Directory{//目录表
    char name[14];       //当前目录名
    int layer;           //层数
    int filenum;         //文件数   
    lslink itemhead;       //目录项链表头
}Directory;

typedef struct DirItem_disk{//目录项 16B
    unsigned inode:16; //i结点指针 2B大小 1-10块地址 11-16块内偏移 计算:块大小*块地址+FCB的大小块内偏移
    char name[14];     //目录项名字
}DirItem_disk;
typedef struct DirItem{
    DirItem_disk did;
    lslink itemlink;
}DirItem;

typedef struct FATitem{//FAT表项 2B -32767-32768
    signed short item:16;
}FATitem;
#endif

