#include<stdlib.h>
#define offset(TYPE,MEMBER) ((size_t)&((TYPE *)0)->MEMBER)
#define list_entry(ptr,TYPE,MEMBER) (void *)ptr-offset(TYPE,MEMBER)  //一定要有void *
#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)
    
typedef struct list_head list_head;
typedef struct list_head{
    list_head *next;
    list_head *prev;
}list_head;
struct mystruct{
    int a;
    char b;
    double c;
    list_head head;
};
void list_init(struct mystruct **ms,int a,char b,double c){
    *ms = (struct mystruct *)malloc(sizeof(struct mystruct));
    (*ms)->a = 1;
    (*ms)->b = '1';
    (*ms)->c = 1;
    (*ms)->head.next=&((*ms)->head);
    (*ms)->head.prev=&((*ms)->head);
}
void list_insert(struct mystruct *ms,int a,char b,double c){
    struct mystruct *m = (struct mystruct *)malloc(sizeof(struct mystruct));
    m->a = a;
    m->b = b;
    m->c = c;
    m->head.prev = &(ms->head);
    m->head.next = ms->head.next;
    ms->head.next->prev = &(m->head);
    ms->head.next = &(m->head);
    return ;
}
