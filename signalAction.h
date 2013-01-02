/**
 * 作者：IT垃圾
 * e-mail:liangguangjin2006@163.com
 * 日期：2012－04－07
 * 描述：信号相关函数
 */
#ifndef SIGNAL_ACTION_H
  #define SIGNAL_ACTION_H
  #include <gtk/gtk.h>
  
  /**
  * 按钮新建信号事件
  */
 void menu_signal_new(GtkMenuItem *memuitem,gpointer *data);
  
  /**
   *菜单退出信号处理
   */
  void menu_signal_quit(GtkMenuItem *memuitem,gpointer *data);
 
  /**
   *菜单关于作者
   */
  void menu_signal_about_author(GtkMenuItem *menuitem,gpointer *data);
 
 /**
  *树目录选中事件
  */
  void select_tree_row_signal(GtkTreeSelection *selection,gpointer data);
  
 /**
  *按钮保存事件
  */
 void button_save_signal(GtkButton *button,gpointer data);
  
 /**
  * 按钮删除信号事件
  */
 void button_delete_signal(GtkButton *button,gpointer data);
 
 /**
  * 按钮修改信号事件
  */
 void button_modify_signal(GtkButton *button,gpointer data);
#endif

