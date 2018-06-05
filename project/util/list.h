#include<stdlib.h>
#include"../global/define.h"
//某类结构包含list_head结构，即可有条件形成一个链表。
//以下是其结构
typedef struct list_head list_head;
typedef struct list_head{
    list_head *next;
    list_head *prev;
}list_head;

/*  offset(TYPE,MEMBER)计算结构成员偏移量
**  TYPE - 数据类型 MEMBER - 成员名*/
#define offset(TYPE,MEMBER) ((size_t)&((TYPE *)0)->MEMBER)

/*  list_entry(ptr,TYPE,MEMBER)
**  ptr - 结构中head的地址 TYPE - 数据类型 MEMBER - 成员名*/
#define list_entry(ptr,TYPE,MEMBER) (void *)ptr-offset(TYPE,MEMBER)  //一定要有void *

/*  list_for_each(pos, head)遍历链表
**  pos - 临时变量(list_head *类型) head - 链表头指针(list_head *类型)*/
#define list_for_each(pos, headptr) \
	for (pos = (headptr)->next; pos != (headptr); pos = pos->next)

/*  list_for_each_reverse(pos, head)反向遍历链表
**  pos - 临时变量(list_head *类型) head - 链表头指针(list_head *类型)*/
#define list_for_each_reverse(pos, headptr) \
	for (pos = (headptr)->prev; pos != (headptr); pos = pos->prev)

/*  get_node(TYPE)初始化一个结点
**  TYPE - 数据类型*/
#define get_node(TYPE) (TYPE *)malloc(sizeof(TYPE));

#define list_delete(nodeptr,structptr) \
    list_unlink(nodeptr); \
    free(structptr)

int list_init(list_head *headptr){
    headptr->next = headptr;
    headptr->prev = headptr;
    return 0;
}
int list_insert(list_head * headptr,list_head * nodeptr){
    nodeptr->next = headptr;
    nodeptr->prev = headptr->prev;
    headptr->prev->next = nodeptr;
    headptr->prev = nodeptr;
    return 0;
}
void list_unlink(list_head * nodeptr){
    nodeptr->next->prev = nodeptr->prev;
    nodeptr->prev->next = nodeptr->next;
}

