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
    list_head *ls,*lhead;
    struct mystruct *m,*temp;

    m = get_node(struct mystruct);
    m->a=1;m->b='1';m->c=1;
    list_init(m->head);
    printf("%d %c %f\n",m->a,m->b,m->c);

    temp = get_node(struct mystruct);
    temp->a=2;temp->b='2';temp->c=2;
    list_insert(m->head,temp->head);
    printf("%ld\n",temp);
    temp = get_node(struct mystruct);
    temp->a=3;temp->b='3';temp->c=3;
    list_insert(m->head,temp->head);

    temp = get_node(struct mystruct);
    temp->a=4;temp->b='4';temp->c=4;
    list_insert(m->head,temp->head);

    printf("show all\n");
    lhead = &(m->head);
    printf("%d %c %f\n",m->a,m->b,m->   c);
    list_for_each(ls,lhead){
        temp = list_entry(ls,struct mystruct,head);
        printf("%d %c %f\n",temp->a,temp->b,temp->c);
    }
    printf("reverse\n");
    list_for_each_reverse(ls,lhead){
        temp = list_entry(ls,struct mystruct,head);
        printf("%d %c %f\n",temp->a,temp->b,temp->c);
    }
    printf("%d %c %f\n",m->a,m->b,m->c);
    printf("%ld\n",list_entry(lhead->next,struct mystruct,head));
    temp = list_entry(lhead->next,struct mystruct,head);
    list_delete(temp,head);
    printf("show all\n");
    lhead = &(m->head);
    printf("%d %c %f\n",m->a,m->b,m->c);
    list_for_each(ls,lhead){
        temp = list_entry(ls,struct mystruct,head);
        printf("%d %c %f\n",temp->a,temp->b,temp->c);
    }
    return 0;
}