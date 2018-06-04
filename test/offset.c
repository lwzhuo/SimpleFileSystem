//计算结构体中某个成员的偏移量
#include<stdio.h>
#define offset(TYPE,MEMBER) ((size_t)&((TYPE *)0)->MEMBER)
#define list_entry(ptr,TYPE,MEMBER) (ptr)-offset(TYPE,MEMBER)
struct mystruct{
    int a;
    char b;
    float c;
};
int main()
{   
    struct mystruct ms;
    ms.a = 1;
    ms.b = '2';
    ms.c = 3;
    printf("%d\n",sizeof(struct mystruct));
    printf("2 in mystruct %d\n",offset(struct mystruct,b));
    printf("%ld %ld\n",&ms,list_entry(&(ms.b),struct mystruct,b));
    printf("%d %c %f\n",ms.a,ms.b,ms.c);
    return 0;
}