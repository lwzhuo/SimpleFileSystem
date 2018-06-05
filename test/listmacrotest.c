#include<stdio.h>
#include<stdlib.h>
#include"../project/util/list.h"

int main()
{
    struct mystruct{
        int a;
        char b;
        double c;
        list_head head;
    };
    list_head *lstemp,*lhead;
    struct mystruct *m,*temp;
//制作链表
    m = get_node(struct mystruct);
    m->a=1;m->b='1';m->c=1;
    list_init(m->head);
    printf("%d %c %f\n",m->a,m->b,m->c);

    temp = get_node(struct mystruct);
    temp->a=2;temp->b='2';temp->c=2;
    list_insert(m->head,temp->head);
    
    temp = get_node(struct mystruct);
    temp->a=3;temp->b='3';temp->c=3;
    list_insert(m->head,temp->head);

    temp = get_node(struct mystruct);
    temp->a=4;temp->b='4';temp->c=4;
    list_insert(m->head,temp->head);
//正向遍历链表
    printf("show all\n");
    lhead = &(m->head);
    printf("%d %c %f\n",m->a,m->b,m->   c);
    list_for_each(lstemp,lhead){
        temp = list_entry(lstemp,struct mystruct,head);
        printf("%d %c %f\n",temp->a,temp->b,temp->c);
    }
//反向遍历链表
    printf("reverse\n");
    list_for_each_reverse(lstemp,lhead){
        temp = list_entry(lstemp,struct mystruct,head);
        printf("%d %c %f\n",temp->a,temp->b,temp->c);
    }
    printf("%d %c %f\n",m->a,m->b,m->c);
//删除结点
    printf("%p \n",list_entry(lhead->next,struct mystruct,head));
    lhead = &(m->head);
    list_delete(lhead->next,lstemp,struct mystruct,head);
    printf("show all\n");
    lhead = &(m->head);
    printf("%d %c %f\n",m->a,m->b,m->c);
    list_for_each(lstemp,lhead){
        temp = list_entry(lstemp,struct mystruct,head);
        printf("%d %c %f\n",temp->a,temp->b,temp->c);
    }

//删除结点
    printf("%p \n",list_entry(lhead->next,struct mystruct,head));
    lhead = &(m->head);
    list_delete(lhead->next,lstemp,struct mystruct,head);
    printf("show all\n");
    lhead = &(m->head);
    printf("%d %c %f\n",m->a,m->b,m->c);
    list_for_each(lstemp,lhead){
        temp = list_entry(lstemp,struct mystruct,head);
        printf("%d %c %f\n",temp->a,temp->b,temp->c);
    }
    return 0;
}