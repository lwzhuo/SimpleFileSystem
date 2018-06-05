#include<stdlib.h>
#include"../global/define.h"
//某类结构包含link结构，即可有条件形成一个链表。
//以下是其结构
typedef struct link link;
typedef struct link{
    link *next;
    link *prev;
}link;

/*  offset(TYPE,MEMBER)计算结构成员偏移量
**  TYPE - 数据类型 MEMBER - 成员名*/
#define offset(TYPE,MEMBER) ((size_t)&((TYPE *)0)->MEMBER)

/*  list_entry(ptr,TYPE,MEMBER)
**  ptr - 结构中link类型成员的地址 TYPE - 数据类型 MEMBER - 成员名*/
#define list_entry(ptr,TYPE,MEMBER) (void *)ptr-offset(TYPE,MEMBER)  //一定要有void *

/*  list_for_each(pos, headptr)正向遍历链表
**  pos - 临时变量(link *类型) headptr - 链表头指针(link *类型)(传进来的这个指针当作链表头)*/
#define list_for_each(pos, headptr) \
	for (pos = (headptr)->next; pos != (headptr); pos = pos->next)

/*  list_for_each_reverse(pos, headptr)反向遍历链表
**  pos - 临时变量(link *类型) headptr - 链表头指针(link *类型)(传进来的这个指针当作链表头)*/
#define list_for_each_reverse(pos, headptr) \
	for (pos = (headptr)->prev; pos != (headptr); pos = pos->prev)

/*  get_node(TYPE)初始化一个TYPE类型的结点
**  TYPE - 数据类型*/
#define get_node(TYPE) (TYPE *)malloc(sizeof(TYPE));

/*  list_delete(linkptr,structptr)删除一个结点
**  linkptr - 结点的link成员指针 structptr - 结点的结构体指针*/
#define list_delete(linkptr,structptr) \
    list_unlink(linkptr); \
    free(structptr)

/*  list_init(link *headptr)初始化一个链表
**  headptr - 结点的link成员指针*/
int list_init(link *headptr){
    headptr->next = headptr;
    headptr->prev = headptr;
    return 0;
}

/*  list_insert(link * headptr,link * nodeptr)在headptr所在节点前插入一个结点
**  headptr - 结点的link成员指针(作为头节点) nodeptr - 结点的link成员指针(作为插入的新节点)*/
int list_insert(link * headptr,link * nodeptr){
    nodeptr->next = headptr;
    nodeptr->prev = headptr->prev;
    headptr->prev->next = nodeptr;
    headptr->prev = nodeptr;
    return 0;
}

/*  list_unlink(link * nodeptr)将当前结点从链表中分离
**  nodeptr - 结点的link成员指针*/
void list_unlink(link * nodeptr){
    nodeptr->next->prev = nodeptr->prev;
    nodeptr->prev->next = nodeptr->next;
}

