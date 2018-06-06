#include<stdio.h>
#ifndef __GLOBAL__
    #define __GLOBAL__
    #include"define.h"
    typedef int Status;
    typedef struct BLOCK0{
        char identify[10];
        char info[200];
        unsigned short root;
        int startblock;
    }BLOCK0;
    extern char sysname[];
    extern FILE * DISK;
    extern BLOCK0 block0;
#endif