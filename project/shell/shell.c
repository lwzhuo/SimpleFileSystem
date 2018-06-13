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
    fgets(buff,100,stdin);
    buff[strlen(buff)-1]='\0';
    buff = trim(buff);
    Ins = split(buff," ",argc);
    return Ins;
}
void help(){
    printf("**********************HELP**********************\n");
    printf("%-10s - %s\n","exit","exit system");
    printf("%-10s - %s\n","pwd","print name of current/working directory");
    printf("%-10s - %s\n","ls","list directory contents");
    printf("%-10s - %s\n","mkdir","make directories");
    printf("%-10s - %s\n","cd","change working directory");
    printf("%-10s - %s\n","create","create new file");
    printf("%-10s - %s\n","rm","remove files");    
    printf("%-10s - %s\n","rmdir","remove directory");
    printf("%-10s - %s\n","open","open a file");
    printf("%-10s - %s\n","close","close a file");
    printf("%-10s - %s\n","write","write to fd");
    printf("%-10s - %s\n","read","read from fd");
    printf("%-10s - %s\n","opl","show open file list");

    printf("******************For Developer******************\n");
    printf("%-10s - %s\n","block0","show block0 information");
    printf("%-10s - %s\n","fat","show fat");
    printf("%-10s - %s\n","fcb","show a fcb information use blocknum and offset");
    printf("%-10s - %s\n","pfcb","show current fcb");
    printf("%-10s - %s\n","sbc","show block chain");
    printf("%-10s - %s\n","sbd","show data in block");
}
int doOpration(int argc,char ** argv){
    //printf("%d **%s**\n",argc,argv[0]);
    if(strcmp(argv[0],"help")==0){
        if(argc>1){
            printf("%s : too many arguments\n",argv[0]);
            return -1;
        }
        else{
            help();
            return 0;
        }  
    }

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

    if(strcmp(argv[0],"create")==0){
        if(argc!=2){
            printf("usage %s [file name]\n",argv[0]);
            return -1;
        }
        else{
            my_create(argv[1]);
            return 0;
        }  
    }

    if(strcmp(argv[0],"rm")==0){
        if(argc!=2){
            printf("usage %s [file name]\n",argv[0]);
            return -1;
        }
        else{
            my_rm(argv[1]);
            return 0;
        }  
    }

    if(strcmp(argv[0],"rmdir")==0){
        if(argc!=2){
            printf("usage %s [directory name]\n",argv[0]);
            return -1;
        }
        else{
            my_rmdir(argv[1]);
            return 0;
        }  
    }

    if(strcmp(argv[0],"open")==0){
        if(argc!=2){
            printf("usage %s [file name]\n",argv[0]);
            return -1;
        }
        else{
            my_open(argv[1]);
            return 0;
        }
    }

    if(strcmp(argv[0],"close")==0){
        if(argc!=2){
            printf("usage %s [fd num]\n",argv[0]);
            return -1;
        }
        else{
            int a;
            a = atoi(argv[1]);
            if(strcmp(argv[1],"0")&&a==0){
                printf("usage %s [fd num]\n",argv[0]);
                return -1;
            }
            my_close(a);
            return 0;
        }  
    }

    if(strcmp(argv[0],"write")==0){
        if(argc!=3){
            printf("usage %s [fd] [write method]\n",argv[0]);
            return -1;
        }
        else{
            int a1,len=0;
            char a2;
            //printf("a1%s a2%s\n",argv[1],argv[2]);
            a1 = atoi(argv[1]);
            a2 = argv[2][0];
            if((strcmp(argv[1],"0")&&a1==0)){
                printf("usage %s [fd] [write method]\n",argv[0]);
                return -1;
            }
            if(strlen(argv[1])!=1){
                printf("usage %s [fd] [write method]\n",argv[0]);
                return -1;
            }
            if(my_write(a1,&len,a2)==0){
                printf("succeed write to fd %d with %d bytes\n",a1,len);
                return 0;
            }
            return 0;
        }  
    }

    if(strcmp(argv[0],"read")==0){
        if(argc!=2){
            printf("usage %s [fd num]\n",argv[0]);
            return -1;
        }
        else{
            int a,len=0;
            a = atoi(argv[1]);
            if(strcmp(argv[1],"0")&&a==0){
                printf("usage %s [fd num]\n",argv[0]);
                return -1;
            }
            if(my_read(a,&len)==0)
                printf("read fd %d with %d bytes\n",a,len);
            return 0;
        }  
    }

    if(strcmp(argv[0],"block0")==0){
         if(argc>1){
            printf("%s : too many arguments\n",argv[0]);
            return -1;
        }
        else{
            showBlock0();
            return 0;
        }
    }

    if(strcmp(argv[0],"fat")==0){
         if(argc!=3){
            printf("usage %s [blocknum start][blocknum end]\n",argv[0]);
            return -1;
        }
        else{
            int a1,a2;
            //printf("a1%s a2%s\n",argv[1],argv[2]);
            a1 = atoi(argv[1]);
            a2 = atoi(argv[2]);
            if((strcmp(argv[1],"0")&&a1==0)||(strcmp(argv[2],"0")&&a2==0)){
                printf("usage %s [blocknum start][blocknum end]\n",argv[0]);
                return -1;
            }
            showFAT(a1,a2);
            return 0;
        }
    }

    if(strcmp(argv[0],"fcb")==0){
        if(argc!=3){
            printf("usage %s [blocknum] [FCB offset in block]\n",argv[0]);
            return -1;
        }
        else{
            int a1,a2;
            //printf("a1%s a2%s\n",argv[1],argv[2]);
            a1 = atoi(argv[1]);
            a2 = atoi(argv[2]);
            if((strcmp(argv[1],"0")&&a1==0)||(strcmp(argv[2],"0")&&a2==0)){
                printf("usage %s [blocknum] [FCB offset in block]\n",argv[0]);
                return -1;
            }
            showFCB(a1,a2);
            return 0;
        }  
    }

    if(strcmp(argv[0],"opl")==0){
         if(argc>1){
            printf("%s : too many arguments\n",argv[0]);
            return -1;
        }
        else{
            showfdList();
            return 0;
        }
    }

    if(strcmp(argv[0],"pfcb")==0){
         if(argc>1){
            printf("%s : too many arguments\n",argv[0]);
            return -1;
        }
        else{
            showPresentFCB();
            return 0;
        }
    }

    if(strcmp(argv[0],"sbc")==0){
        if(argc!=2){
            printf("usage %s [blocknum]\n",argv[0]);
            return -1;
        }
        else{
            int a;
            a = atoi(argv[1]);
            if(strcmp(argv[1],"0")&&a==0){
                printf("usage %s [blocknum]\n",argv[0]);
                return -1;
            }
            showBlockChain(a);
            return 0;
        }  
    }

    if(strcmp(argv[0],"sbd")==0){
        if(argc!=2){
            printf("usage %s [blocknum]\n",argv[0]);
            return -1;
        }
        else{
            int a;
            a = atoi(argv[1]);
            if(strcmp(argv[1],"0")&&a==0){
                printf("usage %s [blocknum]\n",argv[0]);
                return -1;
            }
            showBlockData(a);
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