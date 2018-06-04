#include<stdlib.h>
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
#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

/*  get_node(TYPE)初始化一个结点
**  TYPE - 数据类型*/
#define get_node(TYPE) (TYPE *)malloc(sizeof(TYPE));

/*  list_init(HEAD)初始化一个链表"头"
**  HEAD - 链表头指针(list_head *类型)*/
#define list_init(HEAD) \
    HEAD.next = &(HEAD); HEAD.prev = &(HEAD)

/*  list_insert(HEAD,NODE)插入结点
**  HEAD - 链表头指针(list_head *类型) NODE - 新的结构*/
#define list_insert(HEAD,NODE) \
    NODE->head.next = &(HEAD); \
    NODE->head.prev = HEAD.prev; \
    HEAD.prev->next = &(NODE->head); \
    HEAD.prev = &(NODE->head)
    


