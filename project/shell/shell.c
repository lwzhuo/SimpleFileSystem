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
    sprintf(buff,"\033[33m\033[01m%s\033[0m:\033[36m\033[01m%s\033[0m$ ",sysname,pwd);
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

    if(strcmp(argv[0],"ls")==0){
        if(argc>1){
            printf("%s : too many arguments\n",argv[0]);
            return -1;
        }
        else{
            my_ls();
            return 0;
        }  
    }

    if(strcmp(argv[0],"mkdir")==0){
        if(argc!=2){
            printf("usage %s [directory name]\n",argv[0]);
            return -1;
        }
        else{
            my_mkdir(argv[1]);
            return 0;
        }  
    }

    if(strcmp(argv[0],"cd")==0){
        if(argc!=2){
            printf("usage %s [directory name]\n",argv[0]);
            return -1;
        }
        else{
            my_cd(argv[1]);
            return 0;
        }  
    }

    printf("%s: command not found\n",argv[0]);
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