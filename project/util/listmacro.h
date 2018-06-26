/**************************************************/
/*                     已废弃                      */
/*                   无法正常使用                   */
/**************************************************/ 
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
#define list_for_each(pos, headptr) \
	for (pos = (headptr)->next; pos != (headptr); pos = pos->next)

/*  list_for_each_reverse(pos, head)反向遍历链表
**  pos - 临时变量(list_head *类型) head - 链表头指针(list_head *类型)*/
#define list_for_each_reverse(pos, headptr) \
	for (pos = (headptr)->prev; pos != (headptr); pos = pos->prev)

/*  get_node(TYPE)初始化一个结点
**  TYPE - 数据类型*/
#define get_node(TYPE) (TYPE *)malloc(sizeof(TYPE));

/*  list_init(HEAD)初始化一个链表"头"
**  HEAD - 链表头(list_head 类型)*/
#define list_init(HEAD) \
    HEAD.next = &(HEAD); HEAD.prev = &(HEAD)

/*  list_insert(HEAD,list_head_for_NODE)在head之前插入结点
**  HEAD - 链表头(list_head 类型) list_head_for_NODE - 新的结构的链表头*/
#define list_insert(HEAD,list_head_for_NODE) \
    list_head_for_NODE.next = &(HEAD); \
    list_head_for_NODE.prev = HEAD.prev; \
    HEAD.prev->next = &(list_head_for_NODE); \
    HEAD.prev = &(list_head_for_NODE)

// #define list_delete(struct_ptr,HEAD_MEMBERNAME) \
//     struct_ptr->HEAD_MEMBERNAME.prev->next = struct_ptr->HEAD_MEMBERNAME.next; \
//     struct_ptr->HEAD_MEMBERNAME.next->prev = struct_ptr->HEAD_MEMBERNAME.prev; \
//     free(struct_ptr)

/*  list_delete(ptr,TYPE,MEMBER)删除结点
**  ptr - 结构中head的地址(list_head *类型) pos - 临时变量(list_head *类型) TYPE - 数据类型 MEMBER - head成员名 */
#define list_delete(ptr,pos,TYPE,MEMBER) \
    ptr->prev->next = ptr->next; \
    ptr->next->prev = ptr->prev
    // free(list_entry(pos,TYPE,MEMBER))
