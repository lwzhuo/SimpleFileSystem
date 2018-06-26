#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>

#define BLOCK_SIZE 1024
#define BLOCK_NUMS 1024
#define FAT_ITEM_SIZE 2
#define FAT_ITEM_NUM 1024
#define FCB_SIZE 24
#define FCB_BLOCK 2
#define FCB_ITEM_NUM 42
#define FAT1_LOCATON BLOCK_SIZE * 1
#define FAT2_LOCATON BLOCK_SIZE * 3
#define DATA_LOCATION BLOCK_SIZE * 6
#define ROOT_FCB_LOCATION 5
#define FILE_NAME_LEN 11
#define MAX_FD_NUM 10
#define FREE 0
#define USED 1
#define END_OF_FILE -1

//某类结构包含link结构，即可有条件形成一个链表。
//以下是其结构
typedef struct lslink lslink;
typedef struct lslink
{
    void *sptr; //用来存放包含lslink的结构的首地址 方便销毁链表
    lslink *next;
    lslink *prev;
} lslink;
/*  offset(TYPE,MEMBER)计算结构成员偏移量
**  TYPE - 数据类型 MEMBER - 成员名*/
#define offset(TYPE, MEMBER) ((size_t) & ((TYPE *)0)->MEMBER)

/*  list_entry(ptr,TYPE,MEMBER)
**  ptr - 结构中lslink类型成员的地址 TYPE - 数据类型 MEMBER - 成员名*/
#define list_entry(ptr, TYPE, MEMBER) (void *)ptr - offset(TYPE, MEMBER) //一定要有void *

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
#define list_delete(linkptr, structptr) \
    list_unlink(linkptr);               \
    free(structptr);                    \
    structptr = NULL

typedef int Status;
typedef unsigned char byte;
typedef struct BLOCK0
{
    char identify[10];
    char info[200];
    unsigned short root; //根目录块号
    int startblock;      //数据块号
    int rootFCB;         //根目录FCB位置
} BLOCK0;

typedef struct FCB
{ //16B
    char name[FILE_NAME_LEN];
    unsigned type : 1; //标志文件类型 1-目录 0-文件
    unsigned use : 1;  //标志使用状态 1-已使用(USED) 0-未使用(FREE)
    unsigned short time;
    unsigned short date;
    unsigned int base;   //文件起始盘块
    unsigned int length; //文件长度
} FCB;

typedef struct FCBList
{
    FCB fcb_entry;
    lslink link;
} FCBList;

typedef struct FATitem
{ //FAT表项 2B -32767-32768
    signed short item : 16;
} FATitem;

typedef struct useropen
{
    FCB fcb;
    char dir[80];
    unsigned int count;     //文件指针的位置
    unsigned fcbstate : 1;  //标志fcb是否被修改 1-已修改 0-未修改
    unsigned topenfile : 1; //标志使用状态 1-已使用(USED) 0-未使用(FREE)
    int blocknum;           //所在块号
    int offset_in_block;    //所在块号偏移量
} useropen;

//盘块链
typedef struct blockchain
{
    signed short blocknum : 16;
    lslink link;
} blockchain;

int list_init(lslink *headptr, void *structptr);
int list_insert(lslink *headptr, lslink *nodeptr, void *structptr);
void list_unlink(lslink *nodeptr);
void list_destroy(lslink *headptr);
void filesystem();
int my_cd(char *dirname);
int my_mkdir(char *dirname);
int my_rmdir(char *dirname);
void my_ls();
int my_create(char *filename);
int my_rm(char *filename);
int my_open(char *filename);
int my_close(int fd);
int my_write(int fd, int *len, char wstyle);
int my_read(int fd, int *len);
char *getPwd();
void exitsys();
void format();
void showfdList();
char *header();
char **getInstruction(int *argc);
void shell();
void createDisk();
int writeToDisk(FILE *DISK, void *ptr, int size, int base, long offset);
int readFromDisk(FILE *DISK, void *buff, int size, int base, long offset);
int getFAT(FATitem *fat, int fat_location);
int changeFAT(FATitem *fat, int fat_location);
void reloadFAT();
void rewriteFAT();
int initFCBBlock(int blocknum, int parentblocknum);
int addFCB(FCB fcb, int blocknum);
int changeFCB(FCB newfcb, int blocknum, int offset_in_block);
int removeFCB(int blocknum, int offset_in_block);
int findFCBInBlockByName(char *name, int blocknum);
int getEmptyFCBOffset(int blocknum);
int getFCB(FCB *fcb, int blocknum, int offset_in_block);
int getFCBList(int blocknum, FCBList FLstruct, lslink *fcblisthead);
int getFCBNum(int blocknum);
int getEmptyBlockId();
int getOpenNum();
int getEmptyfd();
int findfdByNameAndDir(char *filename, char *dirname);
int getNextBlocknum(int blocknum);
blockchain *getBlockChain(int blocknum);
char **split(char *string, char *delimiters, int *num);
char *trim(char *str);
int doOpration(int argc, char **argv);
struct tm *getTimeStruct();
unsigned short getDate(struct tm *t);
unsigned short getTime(struct tm *t);
unsigned short getHour(unsigned short date, unsigned short time);
unsigned short getMinute(unsigned short time);
unsigned short getSecond(unsigned short time);
unsigned short getYear(unsigned short time);
unsigned short getMonth(unsigned short time);
unsigned short getDay(unsigned short time);
void showCurrentTime();

//定义需要用到的变量
char sysname[20] = "FileSystem";
char pwd[80];
FILE *DISK;
BLOCK0 block0;
FATitem FAT1[FAT_ITEM_NUM];
FATitem FAT2[FAT_ITEM_NUM];
FCB presentFCB;
useropen uopenlist[MAX_FD_NUM];
char *type[2] = {"file", "directory"};

/*  list_init(lslink *headptr)初始化一个链表
**  headptr - 结点的lslink成员指针 structptr - 结点的首地址*/
int list_init(lslink *headptr, void *structptr)
{
    headptr->next = headptr;
    headptr->prev = headptr;
    headptr->sptr = structptr;
    return 0;
}
/*  list_insert(lslink * headptr,lslink * nodeptr)在headptr所在节点前插入一个结点
**  headptr - 结点的lslink成员指针(作为头节点) 
**  nodeptr - 结点的lslink成员指针(作为插入的新节点)
**  structptr - 结点的首地址*/
int list_insert(lslink *headptr, lslink *nodeptr, void *structptr)
{
    nodeptr->sptr = structptr;
    nodeptr->next = headptr;
    nodeptr->prev = headptr->prev;
    headptr->prev->next = nodeptr;
    headptr->prev = nodeptr;
    return 0;
}
/*  list_unlink(lslink * nodeptr)将当前结点从链表中分离
**  nodeptr - 结点的lslink成员指针*/
void list_unlink(lslink *nodeptr)
{
    if (nodeptr->next != nodeptr)
    { //判断是否只剩下一个结点
        nodeptr->next->prev = nodeptr->prev;
        nodeptr->prev->next = nodeptr->next;
    }
    return;
}
/*  list_destroy(lslink * headptr)销毁整个链表
**  headptr - 结点的lslink成员指针(作为头节点)*/
void list_destroy(lslink *headptr)
{
    lslink *temp = headptr->next, *next;
    while (temp != headptr)
    {
        next = temp->next;
        free(temp->sptr);
        temp = next;
    }
    free(headptr->sptr);
    return;
}

//字符串处理
char **split(char *string, char *delimiters, int *num)
{
    char **result;
    char *arg;
    int a = 1;
    result = (char **)malloc(sizeof(char *) * 10);
    for (int i = 0; i < 10; i++)
        result[i] = (char *)malloc(sizeof(char) * 10);
    arg = strtok(string, delimiters);
    result[0] = arg;
    while (arg != NULL)
    {
        arg = strtok(NULL, delimiters);
        result[a] = arg;
        a++;
        if (a == 10)
        {
            a++;
            break;
        }
    }
    if (arg != NULL) //判断参数是否超出个数
        *num = -1;
    else
        *num = a - 1;
    return result;
}
char *trim(char *str)
{
    int len = 0;
    char *temp = str;
    int start = 0;
    if (temp == NULL || *temp == '\0')
        return str;
    while (*temp != '\0' && isspace(*temp))
    {
        ++temp;
        ++start;
    }
    temp = str + strlen(str) - 1;
    while (*temp != '\0' && isspace(*temp))
        temp++;
    *(temp + 1) = '\0';
    return str + start;
}

//获取时间
struct tm *getTimeStruct()
{
    time_t t = time(NULL);
    return localtime(&t);
}
unsigned short getDate(struct tm *t)
{
    int year = t->tm_year - 70; //以1970为基准 最大存放88年
    int mon = t->tm_mon;
    int day = t->tm_mday;
    unsigned short result = 0;
    if (t->tm_hour > 12) //上下午标志存放在这里 short变量最高位
        result = 1 << 15;
    result += year * 12 * 31 + mon * 31 + day;
    return result;
}
unsigned short getTime(struct tm *t)
{
    //为了精确表示 只存放12h的时间 标志上下午的标志位放在date里
    unsigned short hour = t->tm_hour;
    unsigned short min = t->tm_min;
    unsigned short sec = t->tm_sec;
    if (hour > 12)
        hour -= 12;
    return hour * 3600 + min * 60 + sec;
}
unsigned short getHour(unsigned short date, unsigned short time)
{
    if (date >> 15 == 1)
        return 12 + time / 3600;
    else
        return time / 3600;
}
unsigned short getMinute(unsigned short time)
{
    return (time % 3600) / 60;
}
unsigned short getSecond(unsigned short time)
{
    return (time % 3600) % 60;
}
unsigned short getYear(unsigned short time)
{
    time = time & 32767;
    return 1970 + time / 372; //372=12*31
}
unsigned short getMonth(unsigned short time)
{
    time = time & 32767;
    return (time % 372) / 31 + 1; //372=12*31
}
unsigned short getDay(unsigned short time)
{
    time = time & 32767;
    return (time % 372) % 31; //372=12*31
}
void showCurrentTime()
{
    struct tm *ts = getTimeStruct();
    unsigned short date = getDate(ts);
    unsigned short time = getTime(ts);
    printf("%4d/%02d/%02d %02d:%02d:%02d\n",
           getYear(date), getMonth(date), getDay(date),
           getHour(date, time), getMinute(time), getSecond(time));
}

//文件处理
void createDisk()
{
    char buff[BLOCK_SIZE] = {'0'};
    FILE *f = fopen(sysname, "r+");
    for (int i = 0; i < BLOCK_NUMS; i++)
        fwrite(buff, sizeof(buff), 1, f);
    fclose(f);
}

int writeToDisk(FILE *DISK, void *ptr, int size, int base, long offset)
{
    if (base <= 0)
        fseek(DISK, offset, SEEK_SET);
    else
        fseek(DISK, base + offset, SEEK_SET);
    fwrite(ptr, size, 1, DISK);
    return 0;
}

int readFromDisk(FILE *DISK, void *buff, int size, int base, long offset)
{
    if (base <= 0)
        fseek(DISK, offset, SEEK_SET);
    else
        fseek(DISK, base + offset, SEEK_SET);
    fread(buff, size, 1, DISK);
    return 0;
}

int getFAT(FATitem *fat, int fat_location)
{
    readFromDisk(DISK, fat, FAT_ITEM_SIZE * FAT_ITEM_NUM, fat_location, 0);
    return 0;
}

int changeFAT(FATitem *fat, int fat_location)
{
    writeToDisk(DISK, fat, FAT_ITEM_SIZE * FAT_ITEM_NUM, fat_location, 0);
    return 0;
}

void reloadFAT()
{
    getFAT(FAT1, FAT1_LOCATON);
    getFAT(FAT2, FAT2_LOCATON);
}

void rewriteFAT()
{
    changeFAT(FAT1, FAT1_LOCATON);
    changeFAT(FAT2, FAT2_LOCATON);
}
//初始化一个目录块 blocknum-将要初始化的块号 parentblocknum-父目录块号
int initFCBBlock(int blocknum, int parentblocknum)
{
    //修改FAT
    FAT1[blocknum].item = FCB_BLOCK;
    FAT2[blocknum].item = FCB_BLOCK;
    rewriteFAT();
    //加入.和..目录
    //.目录
    struct tm *t = getTimeStruct();
    FCB fcb;
    strcpy(fcb.name, ".");
    fcb.type = 1;
    fcb.use = USED;
    fcb.time = getTime(t);
    fcb.date = getDate(t);
    fcb.base = blocknum; //指向当前目录
    fcb.length = 1;
    addFCB(fcb, blocknum);
    //..目录
    strcpy(fcb.name, "..");
    fcb.type = 1;
    fcb.use = USED;
    fcb.time = getTime(t);
    fcb.date = getDate(t);
    fcb.base = parentblocknum; //指向父目录
    fcb.length = 1;
    addFCB(fcb, blocknum);
    return 0;
}

int addFCB(FCB fcb, int blocknum)
{
    int offset = getEmptyFCBOffset(blocknum);
    if (offset == -1)
        return -1;
    else
    {
        writeToDisk(DISK, &fcb, sizeof(FCB), blocknum * BLOCK_SIZE, offset * FCB_SIZE);
        return 0;
    }
}

//根据名字在当前目录块寻找对应(已被使用的)FCB
int findFCBInBlockByName(char *name, int blocknum)
{
    FCB fcb[FCB_ITEM_NUM];
    int offset = -1;
    readFromDisk(DISK, fcb, sizeof(FCB) * FCB_ITEM_NUM, blocknum * BLOCK_SIZE, 0);
    for (int i = 0; i < FCB_ITEM_NUM; i++)
    {
        if (fcb[i].use == USED)
        {
            if (strcmp(fcb[i].name, name) == 0)
                offset = i;
        }
    }
    return offset;
}

int changeFCB(FCB fcb, int blocknum, int offset_in_block)
{
    writeToDisk(DISK, &fcb, sizeof(FCB), blocknum * BLOCK_SIZE, offset_in_block * FCB_SIZE);
    return 0;
}

int removeFCB(int blocknum, int offset_in_block)
{
    FCB fcb;
    readFromDisk(DISK, &fcb, sizeof(FCB), blocknum * BLOCK_SIZE, offset_in_block * FCB_SIZE);
    fcb.use = 0;
    writeToDisk(DISK, &fcb, sizeof(FCB), blocknum * BLOCK_SIZE, offset_in_block * FCB_SIZE);
    return 0;
}

int getEmptyFCBOffset(int blocknum)
{
    FCB fcblist[FCB_ITEM_NUM];
    readFromDisk(DISK, fcblist, sizeof(FCB) * FCB_ITEM_NUM, blocknum * BLOCK_SIZE, 0);
    int i;
    for (i = 0; i < FCB_ITEM_NUM; i++)
        if (fcblist[i].use == FREE)
            return i;
    return -1;
}

int getFCB(FCB *fcb, int blocknum, int offset_in_block)
{
    readFromDisk(DISK, fcb, sizeof(FCB), blocknum * BLOCK_SIZE, offset_in_block * FCB_SIZE);
    return 0;
}

int getFCBList(int blocknum, FCBList FLstruct, lslink *fcblisthead)
{
    FCB fcblist[FCB_ITEM_NUM];
    readFromDisk(DISK, &fcblist, sizeof(FCB) * FCB_ITEM_NUM, blocknum * BLOCK_SIZE, 0);
    list_init(fcblisthead, &FLstruct);
    for (int i = 0; i < FCB_ITEM_NUM; i++)
    {
        if (fcblist[i].use == USED)
        {
            FCBList *temp = get_node(FCBList);
            temp->fcb_entry = fcblist[i];
            list_insert(fcblisthead, &(temp->link), temp);
        }
    }
    return 0;
}

int getFCBNum(int blocknum)
{
    int num = 0;
    FCB fcblist[FCB_ITEM_NUM];
    readFromDisk(DISK, &fcblist, sizeof(FCB) * FCB_ITEM_NUM, blocknum * BLOCK_SIZE, 0);
    for (int i = 0; i < FCB_ITEM_NUM; i++)
    {
        if (fcblist[i].use == USED)
            num++;
    }
    return num;
}

int getEmptyBlockId()
{
    int flag = 0;
    for (int i = 0; i < FAT_ITEM_NUM; i++)
    {
        if (FAT1[i].item == FREE && flag == 0)
        {
            flag = i;
            break;
        }
    }
    if (flag == 0)
        return -1;
    else
        return flag;
}

int getOpenNum()
{
    int num = 0;
    for (int i = 0; i < MAX_FD_NUM; i++)
        if (uopenlist[i].topenfile == USED)
            num++;
    return num;
}

int getEmptyfd()
{
    int fd = -1;
    for (int i = 0; i < MAX_FD_NUM; i++)
        if ((uopenlist[i].topenfile == FREE) && (fd == -1))
        {
            fd = i;
            break;
        }
    return fd;
}

int findfdByNameAndDir(char *filename, char *dirname)
{
    int fd = -1;
    for (int i = 0; i < MAX_FD_NUM; i++)
        if (strcmp(filename, uopenlist[i].fcb.name) == 0)
        {
            if (strcmp(dirname, uopenlist[i].dir) == 0)
            {
                fd = i;
                break;
            }
        }
    return fd;
}

int getNextBlocknum(int blocknum)
{
    return FAT1[blocknum].item;
}

blockchain *getBlockChain(int blocknum)
{
    int num = blocknum;
    blockchain *blc, *first;
    blc = get_node(struct blockchain);
    list_init(&(blc->link), blc);
    if (num == 1 || num == 2 || num == -1)
    { //磁盘信息块或者是FCB块掠过
        first = get_node(struct blockchain);
        first->blocknum = blocknum;
        list_insert(&(blc->link), &(first->link), blc);
        return blc;
    }
    while (num != -1)
    {
        blockchain *temp;
        temp = get_node(struct blockchain);
        temp->blocknum = num;
        list_insert(&(blc->link), &(temp->link), blc);
        num = FAT1[num].item;
    }
    return blc;
}

//函数封装
void format()
{ //文件系统格式化
    //引导块 BLOCK0
    strcpy(block0.identify, "MYDISK");
    strcpy(block0.info, "blocksize:1024B\nblocknum:1024 Disksize:1MB");
    block0.root = 5;
    block0.startblock = 6;
    block0.rootFCB = 6;
    fseek(DISK, 0, SEEK_SET);
    fwrite(&block0, sizeof(BLOCK0), 1, DISK);
    //两个FAT块 每个占2个磁盘块 每个FAT表项2B大小
    FATitem *fat, fi;
    fat = (FATitem *)malloc(sizeof(FATitem) * BLOCK_NUMS * 2);
    memset(fat, 0, sizeof(FATitem));
    fseek(DISK, 1 * BLOCK_SIZE, SEEK_SET);
    fwrite(fat, sizeof(FATitem), BLOCK_NUMS * 2, DISK);
    free(fat);
    //根目录区 存放在5号盘块
    struct tm *t = getTimeStruct();
    FCB rootFCB;
    strcpy(rootFCB.name, "/");
    rootFCB.type = 1;   //类型-目录
    rootFCB.use = USED; //已使用
    rootFCB.time = getTime(t);
    rootFCB.date = getDate(t);
    rootFCB.base = 6;   //起始盘块号
    rootFCB.length = 1; //长度
    FAT1[5].item = FCB_BLOCK;
    FAT2[5].item = FCB_BLOCK;
    addFCB(rootFCB, 5);
    //根目录FCB 存放在6号盘块
    initFCBBlock(6, 6);
    //修改对应FAT表
    fi.item = END_OF_FILE;
    for (int i = 0; i < 5; i++)
        FAT1[i].item = USED;
    rewriteFAT();
}

void filesystem()
{ //初始化文件系统
    printf("\033[33m\t\t\t欢迎使用文件系统,help显示帮助\033[0m\n");
    int fd;
    //以创建新文件的方式打开
    if ((fd = open(sysname, O_CREAT | O_EXCL, S_IRWXU)) < 0)
    {
        //打开失败(通常是已经存在这个文件) 检测失败原因
        if (errno == EEXIST)
        { //文件已存在
            if ((fd = open(sysname, O_RDWR, S_IRWXU)) < 0)
            {                                                    //直接打开这个文件
                printf("open:%d  %s\n", errno, strerror(errno)); //打开失败
                exit(-1);
            }
        }
        else //其他原因
            exit(-1);
    }
    else
    { //如果采取新建一个文件，那么新建一个磁盘文件并分配空间
        close(fd);
        createDisk();
        if ((fd = open(sysname, O_RDWR, S_IRWXU)) < 0)
        {                                                    //直接打开这个文件
            printf("open:%d  %s\n", errno, strerror(errno)); //打开失败
            exit(-1);
        }
    }

    //变成标准IO流
    if ((DISK = fdopen(fd, "r+")) == NULL)
    {
        printf("fdopen:%d  %s\n", errno, strerror(errno));
        exit(-1);
    }
    char buff[10];
    if (fread(&block0, sizeof(BLOCK0), 1, DISK) == 1) //取出第一个磁盘块(引导块)
                                                      //判断是否格式化
        if (strcmp(block0.identify, "MYDISK") != 0)
        {
            format();
        }
    //进行其他初始化操作
    strcpy(pwd, "/");           //设置当前目录
    getFCB(&presentFCB, 5, 0);  //将根目录设置成当前内存中的FCB
    getFAT(FAT1, FAT1_LOCATON); //加载FAT1
    getFAT(FAT2, FAT2_LOCATON); //加载FAT2
                                //初始化文件打开表
    for (int i = 0; i < MAX_FD_NUM; i++)
        uopenlist[i].topenfile = FREE; //全部设置成0
    return;
}

char *getPwd()
{
    return pwd;
}


int my_mkdir(char *dirname)
{
    //判断文件名长度
    if (strlen(dirname) > FILE_NAME_LEN)
    {
        printf("mkdir: cannot create directory ‘%s’: directory name must less than %d bytes\n", dirname, FILE_NAME_LEN);
        return -1;
    }
    //检查是否重名
    if (findFCBInBlockByName(dirname, presentFCB.base) > 0)
    {
        printf("mkdir: cannot create directory ‘%s’: File exists\n", dirname);
        return -1;
    }
    //获得当前目录表中空余位置，以便存放FCB
    int offset = getEmptyFCBOffset(presentFCB.base);
    if (offset < 0)
    {
        printf("mkdir:no empty space to make directory\n");
        return -1;
    }
    //获得空的盘块以便存储新的目录表
    int blocknum = getEmptyBlockId();
    FCB fcb;
    if (blocknum < 0)
    {
        printf("mkdir:no empty block\n");
        return -1;
    }
    //构造FCB
    struct tm *t = getTimeStruct();
    strcpy(fcb.name, dirname);
    fcb.type = 1;
    fcb.use = USED;
    fcb.time = getTime(t);
    fcb.date = getDate(t);
    fcb.base = blocknum;
    fcb.length = 1;
    initFCBBlock(blocknum, presentFCB.base);
    addFCB(fcb, presentFCB.base); //在当前目录表加入这个目录项
    rewriteFAT();
    return 0;
}

int my_rmdir(char *dirname)
{
    //检查是否有这个目录
    int offset;
    FCB fcb; //将要删除的FCB
    if ((offset = findFCBInBlockByName(dirname, presentFCB.base)) < 0)
    {
        printf("rmdir: cannot remove '%s': No such file or directory\n", dirname);
        return -1;
    }
    else
    {
        // 清空这个FCB指向的盘块
        getFCB(&fcb, presentFCB.base, offset);
        //判断是否为目录类型
        if (fcb.type != 1)
        {
            printf("rmdir: cannot remove '%s': Is a file, please use rm\n", dirname);
            return -1;
        }
        //判断是否删除的是 .或 ..目录
        if (strcmp(fcb.name, ".") == 0 || strcmp(fcb.name, "..") == 0)
        {
            printf("rmdir: refusing to remove '.' or '..'\n");
            return -1;
        }
        //判断这个目录是否为空
        for (int i = 0; i < FCB_ITEM_NUM; i++)
        {
            FCB f;
            getFCB(&f, fcb.base, i);
            if (f.use == USED)
            {
                if (strcmp(f.name, ".") != 0 && strcmp(f.name, "..") != 0)
                {
                    printf("rmdir: cannot remove '%s': %s not empty\n", dirname, dirname);
                    return -1;
                }
            }
        }
        //修改FAT
        FAT1[fcb.base].item = FREE;
        FAT2[fcb.base].item = FREE;
        rewriteFAT();
        //删除这个FCB
        removeFCB(presentFCB.base, offset);
        return 0;
    }
}

void my_ls()
{
    int blocknum = presentFCB.base; //获得当前fcb所在的盘块号
    lslink *FCBlisthead, *temp;
    FCBList FL, *Fnode;
    FCBlisthead = &(FL.link);
    unsigned short date;
    unsigned short time;
    printf("directory %s\n", pwd);
    printf("%-12s %-10s %-20s %-6s\n", "name", "type", "time", "length(bytes)");
    getFCBList(blocknum, FL, FCBlisthead);
    list_for_each(temp, FCBlisthead)
    {
        Fnode = list_entry(temp, FCBList, link);
        date = Fnode->fcb_entry.date;
        time = Fnode->fcb_entry.time;
        printf("%-12s %-10s %4d/%02d/%02d %02d:%02d:%02d  %-6d\n", Fnode->fcb_entry.name,
               type[Fnode->fcb_entry.type], getYear(date), getMonth(date), getDay(date),
               getHour(date, time), getMinute(time), getSecond(time), Fnode->fcb_entry.length);
    }
}

int my_cd(char *dirname)
{
    int offset;
    if ((offset = findFCBInBlockByName(dirname, presentFCB.base)) < 0)
    {
        printf("cd: %s: No such file or directory\n", dirname);
        return -1;
    }
    else
    {
        FCB fcb;
        getFCB(&fcb, presentFCB.base, offset);
        if (fcb.type == 0)
        {
            printf("cd: %s: Not a directory\n", dirname);
            return -1;
        }
        presentFCB = fcb;              //修改当前fcb值
        if (strcmp(dirname, ".") == 0) //当前目录
            ;
        else if (strcmp(dirname, "..") == 0)
        { //上一级目录
            if (strcmp(pwd, "/") != 0)
            {                                //不是根目录情况
                char *a = strchr(pwd, '/');  //从左往右第一次出现/的位置
                char *b = strrchr(pwd, '/'); //从右往左第一次出现/的位置
                if (a != b)                  //判断是否只有一个/字符 不相等则有多个
                    *b = '\0';
                else
                    *(b + 1) = '\0';
            }
        }
        else
        { //下一级目录
            if (strcmp(pwd, "/") != 0)
                strcat(pwd, "/");
            strcat(pwd, dirname);
        }
        return 0;
    }
}

int my_create(char *filename)
{
    //判断文件名长度
    if (strlen(filename) > FILE_NAME_LEN)
    {
        printf("create: cannot create file ‘%s’: file name must less than %d bytes\n", filename, FILE_NAME_LEN);
        return -1;
    }
    int offset = findFCBInBlockByName(filename, presentFCB.base);
    if (offset > 0)
    { //判断是否已经存在此文件
        printf("create: cannot create file ‘%s’: File exists\n", filename);
        return -1;
    }
    else
    {
        offset = getEmptyFCBOffset(presentFCB.base); //判断FCB块是否有剩余空间加入FCB
        if (offset < 0)
        {
            printf("create: cannot create file ‘%s’: %s Lack of space\n", pwd, filename);
            return -1;
        }
        else
        {
            int blocknum = getEmptyBlockId();
            if (blocknum < 0)
            {
                printf("create: cannot create file ‘%s’: %s Lack of space\n", sysname, filename);
                return -1;
            }
            else
            {
                //构建FCB
                struct tm *t = getTimeStruct();
                FCB fcb;
                strcpy(fcb.name, filename);
                fcb.type = 0;
                fcb.use = USED;
                fcb.time = getTime(t);
                fcb.date = getDate(t);
                fcb.base = blocknum;
                fcb.length = 1;
                addFCB(fcb, presentFCB.base);
                //修改FAT
                FAT1[blocknum].item = END_OF_FILE;
                FAT2[blocknum].item = END_OF_FILE;
                rewriteFAT();
                return 0;
            }
        }
    }
    return 0;
}

int my_rm(char *filename)
{
    int offset = findFCBInBlockByName(filename, presentFCB.base);
    if (offset < 0)
    { //判断是否已经存在此文件
        printf("rm: cannot remove '%s': No such file\n", filename);
        return -1;
    }
    else
    {
        FCB fcb;
        getFCB(&fcb, presentFCB.base, offset);
        //判断文件类型
        if (fcb.type == 1)
        {
            printf("rm: cannot remove '%s': Is a directory\n", filename);
            return -1;
        }
        //修改FAT
        blockchain *blc;
        lslink *temp;
        blc = getBlockChain(fcb.base);
        list_for_each(temp, &(blc->link))
        {
            blockchain *b = list_entry(temp, struct blockchain, link);
            FAT1[b->blocknum].item = FREE;
            FAT2[b->blocknum].item = FREE;
        }
        FAT1[fcb.base].item = FREE;
        FAT2[fcb.base].item = FREE;
        rewriteFAT();
        //删除FCB
        removeFCB(presentFCB.base, offset);
        int fd = findfdByNameAndDir(filename, pwd);
        if (fd >= 0 && uopenlist[fd].topenfile == USED)
            uopenlist[fd].topenfile = FREE;
        return 0;
    }
}

int my_open(char *filename)
{
    int fd;
    fd = findfdByNameAndDir(filename, pwd);
    if (fd >= 0 && uopenlist[fd].topenfile == USED)
    { //判断是否已经打开
        printf("open: cannot open file ‘%s’: %s is already open\n", filename, filename);
        return -1;
    }
    if ((fd = getEmptyfd()) < 0)
    { //查看是否有空的fd
        printf("open: cannot open file ‘%s’: Lack of empty fd\n", filename);
        return -1;
    }
    else
    {
        int offset = findFCBInBlockByName(filename, presentFCB.base); //获得fcb位置
        if (offset < 0)
        {
            printf("open: %s: No such file or directory\n", filename);
            return -1;
        }
        else
        {
            //构造打开表项
            FCB fcb;
            getFCB(&fcb, presentFCB.base, offset);
            uopenlist[fd].fcb = fcb;
            strcpy(uopenlist[fd].dir, pwd);
            uopenlist[fd].count = BLOCK_SIZE * fcb.base + 0;
            uopenlist[fd].fcbstate = 0;
            uopenlist[fd].topenfile = USED;
            uopenlist[fd].blocknum = presentFCB.base;
            uopenlist[fd].offset_in_block = findFCBInBlockByName(filename, presentFCB.base);
            printf("filename:%s fd:%d\n", filename, fd);
            return 0;
        }
    }
}

int my_close(int fd)
{
    if (fd >= MAX_FD_NUM || fd < 0)
    {
        printf("close: invalid fd\n");
        return -1;
    }
    else
    {
        if (uopenlist[fd].topenfile == FREE)
        { //判断是否已经关闭
            printf("close: cannot close fd ‘%d’: fd %d is already close\n", fd, fd);
            return -1;
        }
        if (uopenlist[fd].fcbstate == 1) //fcb被修改了
            changeFCB(uopenlist[fd].fcb, uopenlist[fd].blocknum, uopenlist[fd].offset_in_block);
        uopenlist[fd].topenfile = FREE; //清空文件打开表项
        return 0;
    }
}

//wstyle w-截断写 a-追加写 c-覆盖写
int my_write(int fd, int *sumlen, char wstyle)
{
    if (fd >= MAX_FD_NUM || fd < 0)
    { //判断fd合法性
        printf("close: invalid fd\n");
        return -1;
    }
    else
    {
        if (uopenlist[fd].topenfile == FREE)
        { //判断是否已经关闭
            printf("write: cannot write to fd ‘%d’: fd %d is already close\n", fd, fd);
            return -1;
        }
        else
        {
            if (uopenlist[fd].fcb.type == 1)
            { //判断如果是目录
                printf("write: cannot write to fd ‘%d’: fd %d is a directory\n", fd, fd);
                return -1;
            }
            char str[BLOCK_SIZE], buff[BLOCK_SIZE];
            int blocknum, nextblocknum;
            int len, bloffset; //len-一次读取的长度 bloffset-文件指针块内偏移量
            *sumlen = 0;       //sumlen-总长(所有输入长度之和)
            memset(str, 0, BLOCK_SIZE);
            memset(buff, 0, BLOCK_SIZE);
            //截断写 将文件长度截断成0写
            if (wstyle == 'w')
            {
                bloffset = 0;
                blocknum = uopenlist[fd].fcb.base;
                //做截断处理
                if (FAT1[blocknum].item != END_OF_FILE)
                {
                    blockchain *blc = getBlockChain(blocknum);
                    lslink *temp;
                    list_for_each(temp, &(blc->link))
                    {
                        blockchain *b = list_entry(temp, struct blockchain, link);
                        //清空对应FAT块
                        FAT1[b->blocknum].item = FREE;
                        FAT2[b->blocknum].item = FREE;
                        uopenlist[fd].fcb.length = 0;
                    }
                }
                //循环读取直到EOF 每次最多读取一个盘块大小的内容 多余部分留在缓冲区作为下次读取
                while (fgets(str, BLOCK_SIZE, stdin) != NULL)
                {
                    len = strlen(str); //记录实际读取到的长度
                    //printf("len %d\n",len);
                    if (bloffset + len < BLOCK_SIZE)
                    {
                        //文件指针块内偏移量和将要输入的长度之和小于一个盘块--不停地输入到缓冲区中
                        *sumlen += len;
                        bloffset += len;
                        strcat(buff, str);
                    }
                    else
                    {
                        //长度大于一个盘块
                        int lastlen = BLOCK_SIZE - bloffset - 1;                       //要留出一位给\0作为结尾标志!!!!!!重要
                        int leavelen = len - lastlen;                                  //计算剩下的长度
                        strncat(buff, str, lastlen);                                   //填满上一个块
                        writeToDisk(DISK, buff, BLOCK_SIZE, blocknum * BLOCK_SIZE, 0); //先把之前整个盘块的内容存放起来
                        memset(buff, '\0', BLOCK_SIZE);                                //重新初始化buff 清空原有数据 准备下一次输入
                                                                                       //修改总长度和块内指针位置
                        *sumlen += len;
                        bloffset = leavelen;
                        //将剩下部分输入缓冲区
                        strcat(buff, str + lastlen);
                        //获得一个新的盘块
                        nextblocknum = getEmptyBlockId();
                        //判断空间是否充足
                        if (nextblocknum < 0)
                        {
                            printf("write: cannot write to fd ‘%d’: lack of space\n", fd);
                            return -1;
                        }
                        //标记使用状态(重要！！！)
                        FAT1[nextblocknum].item = END_OF_FILE;
                        FAT2[nextblocknum].item = END_OF_FILE;
                        //修改FAT
                        FAT1[blocknum].item = nextblocknum;
                        FAT2[blocknum].item = nextblocknum;
                        blocknum = nextblocknum; //修改当前块号
                    }
                }
                writeToDisk(DISK, buff, BLOCK_SIZE, blocknum * BLOCK_SIZE, 0);
                uopenlist[fd].count = BLOCK_SIZE * blocknum + bloffset;
                uopenlist[fd].fcb.length = *sumlen;
                //修改对应FCB
                changeFCB(uopenlist[fd].fcb, uopenlist[fd].blocknum, uopenlist[fd].offset_in_block);
                //设置文件结尾
                FAT1[blocknum].item = END_OF_FILE;
                FAT2[blocknum].item = END_OF_FILE;
                rewriteFAT();
                return 0;
            }
            else if (wstyle == 'a')
            {
                //追加写
                int currentlen = uopenlist[fd].fcb.length; //计算当前(写前)长度
                                                           //计算块号
                blocknum = uopenlist[fd].fcb.base;
                while (FAT1[blocknum].item != END_OF_FILE)
                {
                    blocknum = FAT1[blocknum].item;
                }
                int oldblocknum = blocknum; //记录写前最后一个块号
                                            //计算块内偏移量
                bloffset = currentlen % BLOCK_SIZE;
                //循环读取直到EOF 每次最多读取一个盘块大小的内容 多余部分留在缓冲区作为下次读取
                while (fgets(str, BLOCK_SIZE, stdin) != NULL)
                {
                    len = strlen(str); //记录实际读取到的长度
                    if (bloffset + len < BLOCK_SIZE)
                    {
                        //文件指针块内偏移量和将要输入的长度之和小于一个盘块--不停地输入到缓冲区中
                        *sumlen += len;
                        bloffset += len;
                        strcat(buff, str);
                    }
                    else
                    {
                        //长度大于一个盘块
                        int lastlen = BLOCK_SIZE - bloffset - 1;                                                         //要留出一位给\0作为结尾标志!!!!!!重要
                        int leavelen = len - lastlen;                                                                    //计算剩下的长度
                        strncat(buff, str, lastlen);                                                                     //填满上一个块
                        if (blocknum == oldblocknum)                                                                     //和w的区别 这里要分类处理 讨论是不是要填满原有的块
                            writeToDisk(DISK, buff, strlen(buff), blocknum * BLOCK_SIZE, BLOCK_SIZE - strlen(buff) - 1); //填满上一次没有写满盘块
                        else
                            writeToDisk(DISK, buff, BLOCK_SIZE, blocknum * BLOCK_SIZE, 0); //往一个全新的盘块写
                        memset(buff, '\0', BLOCK_SIZE);                                    //重新初始化buff 清空原有数据 准备下一次输入
                                                                                           //修改总长度和块内指针位置
                        *sumlen += len;
                        bloffset = leavelen;
                        //将剩下部分输入缓冲区
                        strcat(buff, str + lastlen);
                        //获得一个新的盘块
                        nextblocknum = getEmptyBlockId();
                        //判断空间是否充足
                        if (nextblocknum < 0)
                        {
                            printf("write: cannot write to fd ‘%d’: lack of space\n", fd);
                            return -1;
                        }
                        //标记使用状态(重要！！！)
                        FAT1[nextblocknum].item = END_OF_FILE;
                        FAT2[nextblocknum].item = END_OF_FILE;
                        //修改FAT
                        FAT1[blocknum].item = nextblocknum;
                        FAT2[blocknum].item = nextblocknum;
                        blocknum = nextblocknum; //修改当前块号
                    }
                }
                if (blocknum == oldblocknum)                                                          //和w的区别 这里要分类处理 讨论是不是要填满原有的第一个块
                    writeToDisk(DISK, buff, strlen(buff), blocknum * BLOCK_SIZE, bloffset - *sumlen); //填满上一次没有写满盘块
                else
                    writeToDisk(DISK, buff, BLOCK_SIZE, blocknum * BLOCK_SIZE, 0); //往一个全新的盘块写
                //writeToDisk(DISK,buff,strlen(buff),blocknum*BLOCK_SIZE,bloffset-strlen(buff));
                uopenlist[fd].count = BLOCK_SIZE * blocknum + bloffset;
                uopenlist[fd].fcb.length = currentlen + *sumlen;
                //修改对应FCB
                changeFCB(uopenlist[fd].fcb, uopenlist[fd].blocknum, uopenlist[fd].offset_in_block);
                //文件结尾
                FAT1[blocknum].item = END_OF_FILE;
                FAT2[blocknum].item = END_OF_FILE;
                rewriteFAT();
                return 0;
            }
            else if (wstyle == 'c')
            {
                //覆盖写 在当前指针后继续写
                int currentlen = uopenlist[fd].fcb.length;   //计算当前(写前)长度
                bloffset = uopenlist[fd].count % BLOCK_SIZE; //计算块内偏移量
                blocknum = uopenlist[fd].count / BLOCK_SIZE; //计算块号
                int oldblocknum = blocknum;                  //记录写前最后一个块号
                //循环读取直到EOF 每次最多读取一个盘块大小的内容 多余部分留在缓冲区作为下次读取
                while (fgets(str, BLOCK_SIZE, stdin) != NULL)
                {
                    len = strlen(str); //记录实际读取到的长度
                    if (bloffset + len < BLOCK_SIZE)
                    {
                        //文件指针块内偏移量和将要输入的长度之和小于一个盘块--不停地输入到缓冲区中
                        *sumlen += len;
                        bloffset += len;
                        strcat(buff, str);
                    }
                    else
                    {
                        //长度大于一个盘块
                        int lastlen = BLOCK_SIZE - bloffset - 1;                                    //要留出一位给\0作为结尾标志!!!!!!重要
                        int leavelen = len - lastlen;                                               //计算剩下的长度
                        strncat(buff, str, lastlen);                                                //填满上一个块
                        if (blocknum == oldblocknum)                                                //和w的区别 这里要分类处理 讨论是不是要填满原有的第一个块
                            writeToDisk(DISK, buff, strlen(buff), blocknum * BLOCK_SIZE, bloffset); //填满上一次没有写满盘块
                        else
                            writeToDisk(DISK, buff, BLOCK_SIZE, blocknum * BLOCK_SIZE, 0); //往一个全新的盘块写
                        //writeToDisk(DISK,buff,strlen(buff),blocknum*BLOCK_SIZE,bloffset);//填满上一个盘块
                        memset(buff, '\0', BLOCK_SIZE); //重新初始化buff 清空原有数据 准备下一次输入
                                                        //修改总长度和块内指针位置
                        *sumlen += len;
                        bloffset = leavelen;
                        //将剩下部分输入缓冲区
                        strcat(buff, str + lastlen);
                        //获得一个新的盘块
                        nextblocknum = getEmptyBlockId();
                        //判断空间是否充足
                        if (nextblocknum < 0)
                        {
                            printf("write: cannot write to fd ‘%d’: lack of space\n", fd);
                            return -1;
                        }
                        //标记使用状态(重要！！！)
                        FAT1[nextblocknum].item = END_OF_FILE;
                        FAT2[nextblocknum].item = END_OF_FILE;
                        //修改FAT
                        FAT1[blocknum].item = nextblocknum;
                        FAT2[blocknum].item = nextblocknum;
                        blocknum = nextblocknum; //修改当前块号
                    }
                }
                if (blocknum == oldblocknum)                                                               //和w的区别 这里要分类处理 讨论是不是要填满原有的第一个块
                    writeToDisk(DISK, buff, strlen(buff), blocknum * BLOCK_SIZE, bloffset - strlen(buff)); //填满上一次没有写满盘块
                else
                    writeToDisk(DISK, buff, BLOCK_SIZE, blocknum * BLOCK_SIZE, 0); //往一个全新的盘块写
                uopenlist[fd].count = BLOCK_SIZE * blocknum + bloffset;
                uopenlist[fd].fcb.length = currentlen + *sumlen;
                //修改对应FCB
                changeFCB(uopenlist[fd].fcb, uopenlist[fd].blocknum, uopenlist[fd].offset_in_block);
                //文件结尾
                FAT1[blocknum].item = END_OF_FILE;
                FAT2[blocknum].item = END_OF_FILE;
                rewriteFAT();
                return 0;
            }
            else
            {
                printf("write: invalid write type ‘%c’\n", wstyle);
                return -1;
            }
        }
    }
}

int my_read(int fd, int *sumlen)
{
    if (fd >= MAX_FD_NUM || fd < 0)
    {
        printf("read: invalid fd\n");
        return -1;
    }
    else
    {
        if (uopenlist[fd].topenfile == FREE)
        { //判断是否已经关闭
            printf("read: cannot read to fd ‘%d’: fd %d is already close\n", fd, fd);
            return -1;
        }
        else
        {
            if (uopenlist[fd].fcb.type == 1)
            { //判断如果是目录
                printf("read: cannot read to fd ‘%d’: fd %d is a directory\n", fd, fd);
                return -1;
            }
            char buff[BLOCK_SIZE];
            int blocknum = uopenlist[fd].fcb.base;
            while (blocknum != END_OF_FILE)
            {
                readFromDisk(DISK, buff, BLOCK_SIZE, blocknum * BLOCK_SIZE, 0);
                *sumlen += strlen(buff);
                fputs(buff, stdout);
                blocknum = FAT1[blocknum].item;
            }
            return 0;
        }
    }
}

void showfdList()
{
    int num = 0;
    for (int i = 0; i < MAX_FD_NUM; i++)
    {
        if (uopenlist[i].topenfile == USED)
        {
            num++;
            printf("%-2d fd:%d name:%s dir:%s fileptr:%d [FCB in block:%d FCB offset:%d]\n", num, i,
                   uopenlist[i].fcb.name, uopenlist[i].dir, uopenlist[i].count,
                   uopenlist[i].blocknum, uopenlist[i].offset_in_block);
        }
    }
}

void exitsys()
{ //退出文件系统
    fclose(DISK);
}

//shell命令层
char *header()
{
    char *buff;
    buff = (char *)malloc(sizeof(char) * 100);
    sprintf(buff, "\033[32m\033[01m%s\033[0m:\033[34m\033[01m%s\033[0m$ ", sysname, pwd);
    return buff;
}
char **getInstruction(int *argc)
{
    char *buff;
    char **Ins;
    buff = (char *)malloc(sizeof(char) * 100);
    Ins = (char **)malloc(sizeof(char *) * 10);
    for (int i = 0; i < 10; i++)
        Ins[i] = (char *)malloc(sizeof(char) * 10);
    printf("%s", header());
    fgets(buff, 100, stdin);
    buff[strlen(buff) - 1] = '\0';
    buff = trim(buff);
    Ins = split(buff, " ", argc);
    return Ins;
}
void help()
{
    printf("简单文件系统帮助\n");
    printf("%-10s : %s\n", "cd", "切换文件目录");
    printf("%-10s : %s\n", "close", "关闭文件");
    printf("%-10s : %s\n", "date", "显示时间");
    printf("%-10s : %s\n", "exit", "退出文件系统");
    printf("%-10s : %s\n", "help", "文件系统帮助");
    printf("%-10s : %s\n", "ls", "文件详情");
    printf("%-10s : %s\n", "mkdir", "创建文件夹");
    printf("%-10s : %s\n", "ol", "查看当前打开的文件列表");
    printf("%-10s : %s\n", "open", "打开文件");
    printf("%-10s : %s\n", "pwd", "当前目录");
    printf("%-10s : %s\n", "read", "读出文件内容");
    printf("%-10s : %s\n", "rm", "删除文件");
    printf("%-10s : %s\n", "rmdir", "删除文件夹");
    printf("%-10s : %s\n", "touch", "新建文件");
    printf("%-10s : %s\n", "write", "写入文件内容");
}
int doOpration(int argc, char **argv)
{
    //printf("%d **%s**\n",argc,argv[0]);
    if (strcmp(argv[0], "help") == 0)
    {
        if (argc > 1)
        {
            printf("%s : too many arguments\n", argv[0]);
            return -1;
        }
        else
        {
            help();
            return 0;
        }
    }

    if (strcmp(argv[0], "exit") == 0)
    {
        if (argc > 1)
        {
            printf("%s : too many arguments\n", argv[0]);
            return -1;
        }
        else
        {
            exitsys();
            return -2;
        }
    }

    if (strcmp(argv[0], "pwd") == 0)
    {
        if (argc > 1)
        {
            printf("%s : too many arguments\n", argv[0]);
            return -1;
        }
        else
        {
            printf("%s\n", getPwd());
            return 0;
        }
    }

    if (strcmp(argv[0], "ls") == 0)
    {
        if (argc > 1)
        {
            printf("%s : too many arguments\n", argv[0]);
            return -1;
        }
        else
        {
            my_ls();
            return 0;
        }
    }

    if (strcmp(argv[0], "mkdir") == 0)
    {
        if (argc != 2)
        {
            printf("usage %s [directory name]\n", argv[0]);
            return -1;
        }
        else
        {
            my_mkdir(argv[1]);
            return 0;
        }
    }

    if (strcmp(argv[0], "cd") == 0)
    {
        if (argc != 2)
        {
            printf("usage %s [directory name]\n", argv[0]);
            return -1;
        }
        else
        {
            my_cd(argv[1]);
            return 0;
        }
    }

    if (strcmp(argv[0], "create") == 0)
    {
        if (argc != 2)
        {
            printf("usage %s [file name]\n", argv[0]);
            return -1;
        }
        else
        {
            my_create(argv[1]);
            return 0;
        }
    }

    if (strcmp(argv[0], "rm") == 0)
    {
        if (argc != 2)
        {
            printf("usage %s [file name]\n", argv[0]);
            return -1;
        }
        else
        {
            my_rm(argv[1]);
            return 0;
        }
    }

    if (strcmp(argv[0], "rmdir") == 0)
    {
        if (argc != 2)
        {
            printf("usage %s [directory name]\n", argv[0]);
            return -1;
        }
        else
        {
            my_rmdir(argv[1]);
            return 0;
        }
    }

    if (strcmp(argv[0], "open") == 0)
    {
        if (argc != 2)
        {
            printf("usage %s [file name]\n", argv[0]);
            return -1;
        }
        else
        {
            my_open(argv[1]);
            return 0;
        }
    }

    if (strcmp(argv[0], "close") == 0)
    {
        if (argc != 2)
        {
            printf("usage %s [fd num]\n", argv[0]);
            return -1;
        }
        else
        {
            int a;
            a = atoi(argv[1]);
            if (strcmp(argv[1], "0") && a == 0)
            {
                printf("usage %s [fd num]\n", argv[0]);
                return -1;
            }
            my_close(a);
            return 0;
        }
    }

    if (strcmp(argv[0], "write") == 0)
    {
        if (argc != 3)
        {
            printf("usage %s [fd] [write method]\n", argv[0]);
            return -1;
        }
        else
        {
            int a1, len = 0;
            char a2;
            //printf("a1%s a2%s\n",argv[1],argv[2]);
            a1 = atoi(argv[1]);
            a2 = argv[2][0];
            if ((strcmp(argv[1], "0") && a1 == 0))
            {
                printf("usage %s [fd] [write method]\n", argv[0]);
                return -1;
            }
            if (strlen(argv[1]) != 1)
            {
                printf("usage %s [fd] [write method]\n", argv[0]);
                return -1;
            }
            if (my_write(a1, &len, a2) == 0)
            {
                printf("succeed write to fd %d with %d bytes\n", a1, len);
                return 0;
            }
            return 0;
        }
    }

    if (strcmp(argv[0], "read") == 0)
    {
        if (argc != 2)
        {
            printf("usage %s [fd num]\n", argv[0]);
            return -1;
        }
        else
        {
            int a, len = 0;
            a = atoi(argv[1]);
            if (strcmp(argv[1], "0") && a == 0)
            {
                printf("usage %s [fd num]\n", argv[0]);
                return -1;
            }
            if (my_read(a, &len) == 0)
                printf("read fd %d with %d bytes\n", a, len);
            return 0;
        }
    }
    if(strcmp(argv[0],"date")==0)
    {
        if(argc>1)
        {
            printf("%s : too many arguments\n",argv[0]);
            return -1;
        }
        else
        {
            showCurrentTime();
            return 0;
        }
    }
    if(strcmp(argv[0],"ol")==0)
    {
         if(argc>1)
         {
            printf("%s : too many arguments\n",argv[0]);
            return -1;
        }
        else
        {
            showfdList();
            return 0;
        }
    }    
    printf("%s: command not found\n", argv[0]);
    return 0;
}

void shell()
{
    char buff[100];
    char **argv;
    int argc, flag;
    argv = (char **)malloc(sizeof(char *) * 10);
    for (int i = 0; i < 10; i++)
        argv[i] = (char *)malloc(sizeof(char) * 10);
    while (1)
    {
        argv = getInstruction(&argc);
        flag = doOpration(argc, argv);
        //printf("flag%d\n",flag);
        if (flag == -2)
            break;
    }
    return;
}

int main()
{
    filesystem();
    shell();
    return 0;
}