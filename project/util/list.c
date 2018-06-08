#include"list.h"
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