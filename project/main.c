#include<stdio.h>
#include<string.h>
#include"function/api.h"

int main()
{
    extern char sysname[100];
    strcpy(sysname,"mydisk");
    startsys();
    showBlock0();
    exitsys();
    return 0;
}