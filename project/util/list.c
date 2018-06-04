#include<stdio.h>
#include<stdlib.h>
#include"list.h"

int main()
{
    list_head *ls,*head;
    struct mystruct **ms;
    struct mystruct *m,*temp;
    ms = &temp;
    list_init(ms,1,'1',1);
    // printf("%d %c %f\n",(*ms)->a,(*ms)->b,(*ms)->c);
    // printf("offset=%d\n",offset(struct mystruct,head));
    m = list_entry(&((*ms)->head),struct mystruct,head);
    // printf("%ld\n",list_entry(&((*ms)->head),struct mystruct,head));
    // printf("%ld %ld %ld\n",m,*ms,&((*ms)->head));
    // printf("%d %c %f\n",m->a,m->b,m->c);
    list_insert(*ms,2,'2',2);
    list_insert(*ms,3,'3',3);
    list_insert(*ms,4,'3',3);
    list_insert(*ms,5,'3',3);

    head = &((*ms)->head);
    m = list_entry(head,struct mystruct,head);
    printf("%d %c %f\n",m->a,m->b,m->c);
    list_for_each(ls,&((*ms)->head)){
        m = list_entry(ls,struct mystruct,head);
        printf("%d %c %f\n",m->a,m->b,m->c);
    }
    return 0;
}