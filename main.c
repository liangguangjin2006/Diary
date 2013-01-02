/**
 * 作者：IT垃圾
 * e-mail:liangguangjin2006@163.com
 * 日期：2012－04－01
 * 描述：相关日志小软件,确保程序有权限操作当前目录(07**)
 */

#include <stdio.h>
#include <gtk/gtk.h>
#include "ui.h"

//用open打开的文件，重新写入时，如果字符串不及原来的长，原来比它还长的字符还保存有。所有有关文件的操作都有这个问题

GtkWindow *window = NULL;  //主窗口
GtkWindow *treeCatalog = NULL;//树目录
FILE_CATALOG *fileCatalog=NULL;//data/catalog文目录结构指针
CATALOG *catalog = NULL; //UI层目录结构
GtkWidget *leftCatalogVBox = NULL;//左边菜单布局

/**
 *程序入口
 */
int main(int argc,char *argv[]){
   GtkWidget *vBox = NULL;//主布局
   GtkWidget *hPanel = NULL; //操作主布局
   GtkWidget *rightMainVBox = NULL; //右边主布局
   
   gtk_init(&argc,&argv);
   
   g_print("检查目录%d\n",check_dir());
   
   
   window = create_main_window();//创建主窗口
   set_main_window(window);//同步ui.c的window
   hPanel = gtk_hpaned_new(); //创建操作主布局
   leftCatalogVBox = gtk_vbox_new(FALSE,0);  //新建竖向布局
   rightMainVBox = gtk_vbox_new(FALSE,0);  //新建竖向布局
   
   //设置主布局
   vBox = gtk_vbox_new(FALSE,0);  //新建竖向布局
   gtk_container_add(GTK_CONTAINER(window),vBox);//向窗口加入主布局--不可视
   
   gtk_box_pack_start(GTK_BOX(vBox),create_menus(),FALSE,FALSE,0);  //添加菜单
   gtk_box_pack_start(GTK_BOX(vBox),hPanel,TRUE,TRUE,10);  //添加面板
   
   gtk_paned_add1(GTK_PANED(hPanel),leftCatalogVBox);// 添加布局
   gtk_paned_add2(GTK_PANED(hPanel),rightMainVBox);// 添加布局
   
   gtk_widget_set_size_request(leftCatalogVBox,150,300); //设置左边大小
   
   init_tree_view_catalog();
   gtk_box_pack_start(GTK_BOX(rightMainVBox),init_right_main_view(),TRUE,TRUE,0); //显示左边主要内容
   
   
   gtk_widget_show_all(window);  //显示所有组件
   
   init_right_main_view_hide_all(NULL);
   gtk_main();
   return 0;
}

/**
 *刷新树目录
 */
void init_tree_view_catalog(){
  fprintf(stderr,"劲哥啊，这里是初始化左边目录树\n");
  fileCatalog = read_catalog_file();
  if(catalog!=NULL)
    free(catalog);
 
  catalog = (CATALOG *)malloc(sizeof(CATALOG));
  memset(catalog,0x00,sizeof(catalog));
  fprintf(stderr,"在min.c:%s\n",fileCatalog);
  catalog = convert_file_struct_to_ui_struct_catalog(fileCatalog);//由file结构转换为ui目录结构
  set_file_catalog(fileCatalog);
  
  
  if(treeCatalog!=NULL){
     gtk_container_remove(GTK_CONTAINER(leftCatalogVBox),treeCatalog);
  }
   
  treeCatalog =  create_catalog(catalog,fileCatalog);
  gtk_box_pack_start(GTK_BOX(leftCatalogVBox),treeCatalog,TRUE,TRUE,0); //设置目录 
  gtk_widget_show_all(leftCatalogVBox);
}
