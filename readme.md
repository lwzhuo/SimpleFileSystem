# 简单文件系统的实现

## 构建运行方式:
    1.进入project目录
    2.创建名为out的文件夹
    3.在终端使用make命令
    4.运行生成的名为fs的程序
    或者
    1.执行build.sh脚本
    2.运行生成的名为fs的程序

## 使用方法:
- 运行fs程序以后
输入help命令，根据相应命令进行操作。
注:本系统的命令操作，不能照搬linux shell下的命令，可以依据项目下shell目录的shell.c源码或者在输入命令参数出错以后的提示进行操作。

## 实验要求和说明:
- 1.在内存中开辟一个虚拟磁盘空间作为文件存储分区，在其上实现一个简单的基于多级目录的单用户单任务系统中的文件系统。在退出该文件系统的使用时，应将该虚拟文件系统以一个文件的方式保存到磁盘上，以便下次可以再将它恢复到内存的虚拟磁盘空间中。

- 2.文件存储空间的分配可采用显式链接分配或其他的办法。

- 3.空闲磁盘空间的管理可选择位示图或其他的办法。如果采用位示图来管理文件存储空间，并采用显式链接分配方式，那么可以将位示图合并到 FAT 中。

- 4.文件目录结构采用多级目录结构。为了简单起见，可以不使用索引结点，其中的每个目录项应包含文件名、物理地址、长度等信息，还可以通过目录项实现对文件的读和写的保护。

- 5.要求提供以下操作命令：
    + my_format：对文件存储器进行格式化，即按照文件系统的结构对虚拟磁盘空间进行布局，并在其上创建根目录以及用于管理文件存储空间等的数据结构。
    + my_mkdir：用于创建子目录。
    + my_rmdir：用于删除子目录。
    + my_ls：用于显示目录中的内容。
    + my_cd：用于更改当前目录。
    + my_create：用于创建文件。
    + my_open：用于打开文件。
    + my_close：用于关闭文件。
    + my_write：用于写文件。
    + my_read：用于读文件。
    + my_rm：用于删除文件。
    + my_exitsys：用于退出文件系统。

### 时间仓促，本项目还有很多地方不完善，提交bug反馈，学习交流，欢迎提issue。