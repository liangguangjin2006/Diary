/**
 * 作者：IT垃圾
 * e-mail:liangguangjin2006@163.com
 * 日期：2012－04－01
 * 描述：UI相关函数
 */
 
#ifndef UI_H
 #define UI_H
 #include <gtk/gtk.h>
 #include "files.h"
 /**
  *  目录
  */
 typedef struct UI_CATALOG{
    char* text;//内容
    struct UI_CATALOG *brother;//下一个兄弟结点，没有为NULL
    struct UI_CATALOG *headChild;//长子结点,没有为NULL
 }CATALOG;
 
 /**
  *　目标类型枚举
  */
 enum{
    TITLE_COLUMN, //形名
    N_COLUMNS  //列总数
 };
 
 /**
  *设置mainWindow，与main.c同步
  */
 void set_main_window(GtkWidget *mv);
 
 /**
 *设置file层CATALOG，保持与main.c目录一致，方便使用
 */
void set_file_catalog(FILE_CATALOG  *tempcatalog);

 /*
  * 创建主界口，并设置相关信号  
  */
 GtkWidget* create_main_window();
 
 /**
  * 创建菜单
  */
 GtkWidget* create_menus();
 
 /**
  *创建树目录
  * @param catalog 目录参数
  ＊　本版本程序目录最多为二级
  */
GtkWidget* create_catalog(const CATALOG *catalog,const FILE_CATALOG *fileCatalog);
 
/**
 *创建右边主界面内容
 */
 GtkWidget* init_right_main_view(void);
 
/**
 *完成由文件层目录结构转换为UI层目录结构
 */
CATALOG *convert_file_struct_to_ui_struct_catalog(const FILE_CATALOG *file_src_catalog);

/**
 *显示文章具体内容
 * filename 文件名
 * selectIndex 在树中选中的位置索引，与树file层数据结构对应
 */
void init_main_view_content(const char *filename,const char *selectIndex);

/**
 *在点击菜单新建项后，初始化界面
 */
 void init_new_content_views(void);
 
 /**
  *初始化右边主界面,所有不可见
  *@param str如果不为空，则弹出框显示str信息
  */
 void init_right_main_view_hide_all(const char *str);
 
 /**
  *保存日志
  */
  void save_log(void);
  
/**
 *修改日志
 */
 void modify_log(void);
 
/**
 *删除日志
 */
void delete_log(void);

/**
 *关于作者
 */
void about_author(void);
 
#endif
