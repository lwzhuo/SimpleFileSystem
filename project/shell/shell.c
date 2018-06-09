#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"shell.h"
#include"../global/global.h"
#include"../util/str.h"
#include"../function/api.h"
char * header(){
    char *buff;
    buff = (char *)malloc(sizeof(char)*100);
    sprintf(buff,"%s:%s$ ",sysname,pwd);
    return buff;
}
char ** getInstruction(int *argc){
    char *buff;
    char **Ins;
    buff = (char *)malloc(sizeof(char)*100);
    Ins = (char **)malloc(sizeof(char *)*10);
    for(int i=0;i<10;i++)
        Ins[i] = (char *)malloc(sizeof(char)*10);
    printf("%s",header());
    gets(buff);
    buff = trim(buff);
    Ins = split(buff," ",argc);
    return Ins;
}

int doOpration(int argc,char ** argv){
    //printf("%d **%s**\n",argc,argv[0]);
    if(strcmp(argv[0],"exit")==0){
        if(argc>1){
            printf("%s : too many arguments\n",argv[0]);
            return -1;
        }
        else{
            exitsys();
            return -2;
        }  
    }

    if(strcmp(argv[0],"pwd")==0){
        if(argc>1){
            printf("%s : too many arguments\n",argv[0]);
            return -1;
        }
        else{
            printf("%s\n",getPwd());
            return 0;
        }  
    }
    return 0;
}
void go(){
    char buff[100];
    char **argv;
    int argc,flag;
    argv = (char **)malloc(sizeof(char *)*10);
    for(int i=0;i<10;i++)
        argv[i] = (char *)malloc(sizeof(char)*10);
    while(1){
        argv = getInstruction(&argc);
        flag = doOpration(argc,argv);
        //printf("flag%d\n",flag);
        if(flag==-2)
            break;
    }
    return;
}