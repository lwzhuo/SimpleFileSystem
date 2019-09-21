#include<string.h>
#include<ctype.h>
#include<stdlib.h>
//限定返回个数最多10个 每一个字符串最多10个字符
char ** split(char *string,char * delimiters,int *num){
    char ** result;
    char * arg;
    int a=1;
    result = (char **)malloc(sizeof(char *)*10);
    for(int i=0;i<10;i++)
        result[i] = (char *)malloc(sizeof(char)*10);
    arg = strtok(string,delimiters);
    result[0] = arg;
    while(arg!=NULL){
        arg = strtok(NULL,delimiters);
        result[a] = arg;
        a++;
        if(a==10){
            a++;
            break;
        }
    }
    if(arg!=NULL)//判断参数是否超出个数
        *num=-1;
    else
        *num = a-1;
    return result;
}
char * trim(char *str){
    int len=0;
    char *temp = str;
    int start=0;
    if(temp==NULL||*temp=='\0')
        return str;
    while (*temp!='\0'&&isspace(*temp)){
        ++temp;
        ++start;
    }
    temp = str+strlen(str)-1;
    while(*temp!='\0'&&isspace(*temp))
        temp++;
    *(temp+1)='\0';
    return str+start;
}

int is_empty(char * str){
    int empty = 1;
    int not_empty = 0;
    if(str==NULL)//字符串为null直接判定为空
        return empty;
    for(int i=0;i<strlen(str);i++)
        if(!isspace(str[i]))//使用isspace判定是否有非空字符，如果有立刻判定字符串非空
            return not_empty;
    return empty;
}