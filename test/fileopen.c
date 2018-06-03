#include<stdio.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<string.h>
#include<errno.h>
int main(){
    int stat = open("myfile",O_CREAT|O_EXCL,S_IRWXU);
    printf("%d\n",stat);
    perror(strerror(errno));
    printf("%d  %s\n",errno,strerror(errno));
    return 0;
}