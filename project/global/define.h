#ifndef __DEFINE__
    #define __DEFINE__
    /*盘块相关位置*/
    #define BLOCK_SIZE 1024                             // 磁盘块大小(字节)
    #define BLOCK_NUMS 1024                             // 磁盘块数
    
    /*盘块各个部分位置的相关配置*/
    #define FAT_ITEM_SIZE 2                             // FAT表项大小(字节)
    #define FAT_ITEM_NUM 1024                           // FAT表项数
    #define FCB_SIZE 24                                 // FCB大小(字节)
    #define FCB_ITEM_NUM 42                             // FCB大小
    #define FAT1_INIT_BLOCK 1                           // FAT1起始盘块位置(盘块号)
    #define FAT2_INIT_BLOCK 3                           // FAT2起始盘块位置(盘块号)
    #define ROOT_FCB_LOCATION 5                         // 根目录FCB位置(盘块号)
    #define DATA_INIT_BLOCK 6                           // 数据块起始位置(盘块号)
    #define FAT1_LOCATON (BLOCK_SIZE*FAT1_INIT_BLOCK)   // FAT1起始位置的字节数
    #define FAT2_LOCATON (BLOCK_SIZE*FAT2_INIT_BLOCK)   // FAT2起始位置的字节数
    #define DATA_LOCATION (BLOCK_SIZE*DATA_INIT_BLOCK)  // 数据块起始位置的字节数
    
    /*文件相关配置*/
    #define FILE_NAME_LEN 11                            // 文件名长度(字节)
    #define MAX_FD_NUM 10                               // 最大支持打开的文件标识符个数
    
    /*FAT表项相关配置*/
    #define END_OF_FILE -1                              // FAT表项中文件结尾盘块标识
    #define FREE 0                                      // FAT表项中未分配的盘块标识
    #define USED 1                                      // FAT表项中已分配的盘块标识
    #define FCB_BLOCK 2                                 // FAT表项中文件存放控制块的盘块的标识
#endif