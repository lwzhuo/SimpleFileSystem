#include<stdio.h>
#ifndef __GLOBAL__
    #define __GLOBAL__
    extern char sysname[];
    extern FILE * DISK;
    extern BLOCK0 block0;
    extern char pwd[];
    extern FATitem FAT1 [];
    extern FATitem FAT2 [];
#endif