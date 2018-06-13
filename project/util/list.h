#ifndef __LIST__
#define __LIST__
#include<stdlib.h>
#include"../global/define.h"
//某类结构包含link结构，即可有条件形成一个链表。
//以下是其结构
typedef struct lslink lslink;
typedef struct lslink{
    void * sptr;//用来存放包含lslink的结构的首地址 方便销毁链表
    lslink *next;
    lslink *prev;
}lslink;
/*  offset(TYPE,MEMBER)计算结构成员偏移量
**  TYPE - 数据类型 MEMBER - 成员名*/
#define offset(TYPE,MEMBER) ((size_t)&((TYPE *)0)->MEMBER)

/*  list_entry(ptr,TYPE,MEMBER)
**  ptr - 结构中lslink类型成员的地址 TYPE - 数据类型 MEMBER - 成员名*/
#define list_entry(ptr,TYPE,MEMBER) (void *)ptr-offset(TYPE,MEMBER)  //一定要有void *

/*  list_for_each(pos, headptr)正向遍历链表
**  pos - 临时变量(lslink *类型) headptr - 链表头指针(lslink *类型)(传进来的这个指针当作链表头)*/
#define list_for_each(pos, headptr) \
	for (pos = (headptr)->next; pos != (headptr); pos = pos->next)

/*  list_for_each_reverse(pos, headptr)反向遍历链表
**  pos - 临时变量(lslink *类型) headptr - 链表头指针(lslink *类型)(传进来的这个指针当作链表头)*/
#define list_for_each_reverse(pos, headptr) \
	for (pos = (headptr)->prev; pos != (headptr); pos = pos->prev)

/*  get_node(TYPE)初始化一个TYPE类型的结点
**  TYPE - 数据类型*/
#define get_node(TYPE) (TYPE *)malloc(sizeof(TYPE));

/*  list_delete(linkptr,structptr)删除一个结点
**  linkptr - 结点的lslink成员指针 structptr - 结点的结构体指针
**  注意:删除表头的行为要多加小心*/
#define list_delete(linkptr,structptr) \
    list_unlink(linkptr); \
    free(structptr); \
    structptr = NULL

int list_init(lslink *headptr,void *structptr);
int list_insert(lslink * headptr,lslink * nodeptr,void *structptr);
void list_unlink(lslink * nodeptr);
void list_destroy(lslink * headptr);
#endif
