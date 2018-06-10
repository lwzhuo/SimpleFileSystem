#ifndef __API__
    #define __API__
    void startsys();
    void format();
    void showBlock0();
    void showFAT();
    void showFCB(int blocknum,int num_in_block);
    int my_cd(char *dirname);
    int my_mkdir(char *dirname);
    void my_rmdir(char *dirname);
    void my_ls();
    int my_create(char *filename);
    int my_rm(char *filename);
    int my_open(char *filename);
    void my_close(int fd);
    int my_write(int fd);
    int do_write(int fd,char *text,int len,char wstyle);
    int my_read(int fd,int len);
    int do_read(int fd,int len,char *text);
    char *getPwd();
    void exitsys();
#endif
