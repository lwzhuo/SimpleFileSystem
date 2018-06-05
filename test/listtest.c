#include<stdio.h>
#include"../project/util/list.h"
int main()
{
    struct mystruct{
        int a;
        char b;
        double c;
        list_head head;
    };
    struct mystruct *m,*temp;
    list_head *ltemp,*lhead;
    m = get_node(struct mystruct);
    m->a=1;m->b='1';m->c=1;
    lhead = &(m->head);
    list_init(lhead);

    temp = get_node(struct mystruct);
    temp->a=2;temp->b='2';temp->c=2;
    list_insert(lhead,&(temp->head));

    temp = get_node(struct mystruct);
    temp->a=3;temp->b='3';temp->c=3;
    list_insert(lhead,&(temp->head));

    temp = get_node(struct mystruct);
    temp->a=4;temp->b='4';temp->c=4;
    list_insert(lhead,&(temp->head));

    printf("show all\n");
    temp = list_entry(lhead,struct mystruct,head);
    printf("%d %c %f\n",temp->a,temp->b,temp->c);
    list_for_each(ltemp,lhead){
        temp = list_entry(ltemp,struct mystruct,head);
        printf("%d %c %f\n",temp->a,temp->b,temp->c);
    }

    temp = list_entry(lhead->next,struct mystruct,head);
    list_delete(lhead->next,temp);
    
    temp = list_entry(lhead->next,struct mystruct,head);
    list_delete(lhead->next,temp);
    printf("show all\n");
    temp = list_entry(lhead,struct mystruct,head);
    printf("%d %c %f\n",temp->a,temp->b,temp->c);
    list_for_each(ltemp,lhead){
        temp = list_entry(ltemp,struct mystruct,head);
        printf("%d %c %f\n",temp->a,temp->b,temp->c);
    }
    return 0;
}