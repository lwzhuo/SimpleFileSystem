#include<stdio.h>
#include"../project/util/list.h"
#include"../project/util/list.c"
struct mystruct{
        int a;
        char b;
        double c;
        lslink head;
    };
void showAll(lslink *lhead){
    lslink *ltemp;
    struct mystruct *temp;
    temp = lhead->sptr;
    if(temp!=NULL)
        printf("%d %c %f\n",temp->a,temp->b,temp->c);
    list_for_each(ltemp,lhead){
        temp = ltemp->sptr;
        printf("%d %c %f\n",temp->a,temp->b,temp->c);
    }
}
int main()
{
    
    struct mystruct *m,*temp;
    lslink *ltemp,*lhead;
//构造链表
    m = get_node(struct mystruct);
    m->a=1;m->b='1';m->c=1;
    lhead = &(m->head);
    list_init(lhead,m);

    temp = get_node(struct mystruct);
    temp->a=2;temp->b='2';temp->c=2;
    list_insert(lhead,&(temp->head),temp);

    temp = get_node(struct mystruct);
    temp->a=3;temp->b='3';temp->c=3;
    list_insert(lhead,&(temp->head),temp);

    temp = get_node(struct mystruct);
    temp->a=4;temp->b='4';temp->c=4;
    list_insert(lhead,&(temp->head),temp);
//遍历链表
    printf("show all\n");
    temp = list_entry(lhead,struct mystruct,head);
    printf("%d %c %f\n",temp->a,temp->b,temp->c);
    list_for_each(ltemp,lhead){
        temp = ltemp->sptr;
        printf("%d %c %f\n",temp->a,temp->b,temp->c);
    }
//反向遍历链表
    printf("show all reverse\n");
    
    list_for_each_reverse(ltemp,lhead){
        temp = ltemp->sptr;
        printf("%d %c %f\n",temp->a,temp->b,temp->c);
    }
    temp = lhead->sptr;
    printf("%d %c %f\n",temp->a,temp->b,temp->c);
//删除结点
    temp = lhead->next->sptr;//获得结构体地址，用于下一步传参
    list_delete(lhead->next,temp);
    printf("show all\n");
    showAll(lhead);
//删除结点
    temp = lhead->prev->sptr;
    list_delete(lhead->prev,temp);
    printf("show all\n");
    showAll(lhead);
//删除结点
    temp = lhead->prev->sptr;
    list_delete(lhead->prev,temp);
    printf("show all\n");
    showAll(lhead);
//删除最后一个结点
    temp = lhead->prev->sptr;
    list_delete(lhead->prev,temp);
    printf("show all\n");
    showAll(lhead);

    printf("destroy test\n");
//再次构造链表、链表销毁测试
    m = get_node(struct mystruct);
    m->a=1;m->b='1';m->c=1;
    lhead = &(m->head);
    list_init(lhead,m);

    temp = get_node(struct mystruct);
    temp->a=2;temp->b='2';temp->c=2;
    list_insert(lhead,&(temp->head),temp);

    temp = get_node(struct mystruct);
    temp->a=3;temp->b='3';temp->c=3;
    list_insert(lhead,&(temp->head),temp);

    temp = get_node(struct mystruct);
    temp->a=4;temp->b='4';temp->c=4;
    list_insert(lhead,&(temp->head),temp);

    printf("show all\n");
    showAll(lhead);
    temp = lhead->prev->sptr;
    list_destroy(lhead);
    printf("after destroy\n");
    showAll(lhead);

    return 0;
}