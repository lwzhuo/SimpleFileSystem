#include<stdio.h>
int main()
{
    char buff[1024]={'0'};//1KB
    FILE *f = fopen("mydisk","a+");
    for(int i=0;i<1024*1024;i++)
        fwrite(buff,sizeof(buff),1,f);
    fclose(f);
    return 0;
}