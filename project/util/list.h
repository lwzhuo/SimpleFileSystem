#include<stdlib.h>
#include"../global/define.h"
//某类结构包含link结构，即可有条件形成一个链表。
//以下是其结构
typedef struct lslink lslink;
typedef struct lslink{
    void * sptr;
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

/*  list_init(lslink *headptr)初始化一个链表
**  headptr - 结点的lslink成员指针 structptr - 结点的首地址*/
int list_init(lslink *headptr,void *structptr){
    headptr->next = headptr;
    headptr->prev = headptr;
    headptr->sptr = structptr;
    return 0;
}

/*  list_insert(lslink * headptr,lslink * nodeptr)在headptr所在节点前插入一个结点
**  headptr - 结点的lslink成员指针(作为头节点) 
**  nodeptr - 结点的lslink成员指针(作为插入的新节点)
**  structptr - 结点的首地址*/
int list_insert(lslink * headptr,lslink * nodeptr,void *structptr){
    nodeptr->sptr = structptr;
    nodeptr->next = headptr;
    nodeptr->prev = headptr->prev;
    headptr->prev->next = nodeptr;
    headptr->prev = nodeptr;
    return 0;
}

/*  list_unlink(lslink * nodeptr)将当前结点从链表中分离
**  nodeptr - 结点的lslink成员指针*/
void list_unlink(lslink * nodeptr){
    if(nodeptr->next!=nodeptr){//判断是否只剩下一个结点
        nodeptr->next->prev = nodeptr->prev;
        nodeptr->prev->next = nodeptr->next;
    }
    return;
}

/*  list_destroy(lslink * headptr)销毁整个链表
**  headptr - 结点的lslink成员指针(作为头节点)*/
void list_destroy(lslink * headptr){
    lslink * temp=headptr->next,*next;
    while(temp!=headptr){
        next = temp->next;
        free(temp->sptr);
        temp = next;
    }
    free(headptr->sptr);
    return;
}

