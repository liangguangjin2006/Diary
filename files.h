/**
 * 作者：IT垃圾
 * e-mail:liangguangjin2006@163.com
 * 日期：2012－04－07
 * 描述：文件相关操作
 */

#ifndef FILES_H
 #define FILES_H
 #include <stdio.h>
 #include <sys/types.h>
 #include <dirent.h>
 #include <fcntl.h>
 #define DIR_NAME "data"  //数据目录名
 #define CATALOG_FILE "./data/catalog" //数据索引文件
 /**
  *定义存文件类型
  */
 #define SAVE_NEW_LOG_FILE 0 //保存新建文件
 #define SAVE_MODIFY_LOG_FILE 1 //保存修改文件
 
 //[{2012-01:[我的文章1:liangtxt1=我的文章1:liangtxt2]},{2012-01:[我的文章1:liangtxt1=我的文章1:liangtxt2]}]
 
 typedef struct{ //二级目录
   char *title; //标题
   char *file_name;//文件名
   struct FILE_DETAIL_CATALOG *brother; //下一个兄弟结点，单向链表，NULL为结束
 }FILE_DETAIL_CATALOG;
 
 typedef struct{  //文件目录数据结构
   char *catalog_name; //一级目录名
   struct FILE_DETAIL_CATALOG *headChild; //第一个子结点
   struct FILE_CATALOG *brother;//下一个兄弟结点，单向链表,NULL为结束
 }FILE_CATALOG;
 
 typedef struct{ //日志文件具体内容
    char *title; //标题
    char *content;  //具体内容
 }FILE_CONTENT;
 
 typedef struct{  //保存、修改日志文件成功后返回的数据类型
    char *dir;//文件夹名
    char *file_name; //文件名
 }SAVE_LOG_FILE_SUCCESS_RETURN_TYPE;
 
 /**
  *检查是否存在存文件的目录，没有则创建
  *@return 1 存在/创建成功,0：不存在/创建失败
  */
  int check_dir(void);
  
 /**
  *检查是data下否存在存文件的目录，没有则创建
  * @param dirName文件夹名
  *@return 1 存在/创建成功,0：不存在/创建失败
  */
  int check_dir_data(char *dirName);
  
 /**
  *　读取数据目录文件
  * @param fileCatalog:目录结构指针
  */
 FILE_CATALOG *read_catalog_file(void);
 
 /**
  *解析字符串，生成目录结构
  * @param fileCagalog:目录结构指针
  * @param src:目录结构字符串
  */
FILE_CATALOG *parset_string_to_file_catalog(char *src);

/**
 *把file层数据结构转换为字符类型
 * @param srcCatalog 原file层目录结层
 * @param title 新增时的title 当title=NULL时为修改，不为NULL时为新增
 * @param saveSuccess 新增日志时返回的数据结构，修改时为NULL
 */
char *convert_file_to_str_catalog(const FILE_CATALOG *srcCatalog,const char *title,const SAVE_LOG_FILE_SUCCESS_RETURN_TYPE *saveSuccess);

/**
 *读取并解析具体日志文件
 */
FILE_CONTENT *read_log_file(const char *filename);

/**
 *@param type SAVE_NEW_LOG_FILE 0 //保存新建文件  SAVE_MODIFY_LOG_FILE 1 //保存修改文件
 *@param modifyFileName 要修改的文件名
 *@param title标题
 *@param content 内容
 *@return NULL:失败 SAVE_LOG_FILE_SUCCESS_RETURN_TYPE *:成功
 */
SAVE_LOG_FILE_SUCCESS_RETURN_TYPE * save_log_file(int type,char *title,char *content,char *modifyFileName);

/**
 *保存、修改目录文件
 *@param str 内容
 *-1:失败 0:成功
 */
int save_or_modify_catalog_file(char *str);

/**
 *删除日志
 * @param fileName
 *@return -1失败
 */
int delete_log_file(const char *fileName);

/**
 *删除第一层文件夹
 * @param dirName
 *@return -1失败
 */
int delete_dir(const char *dirName);
 
#endif
