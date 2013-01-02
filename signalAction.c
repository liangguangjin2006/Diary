/**
 * 作者：IT垃圾
 * e-mail:liangguangjin2006@163.com
 * 日期：2012－04－07
 * 描述：信号相关函数
 */
#include "signalAction.h"
#include "ui.h"

/**
  * 按钮新建信号事件
  */
void menu_signal_new(GtkMenuItem *memuitem,gpointer *data){
   g_printf("点击新建\n");
   init_new_content_views();
 }

/**
 *菜单退出信号处理
 */
 void menu_signal_quit(GtkMenuItem *memuitem,gpointer *data){
    g_print("退出\n"); 
    gtk_main_quit();
 }

/**
 *菜单关于作者
 */
 void menu_signal_about_author(GtkMenuItem *menuitem,gpointer *data){
   g_print("关于作者:liangguangjin2006@163.com\n");
   about_author();
 }
 
 /**
  *树目录选中事件
  * ASC II　： 数字0的ASC II为48
  */
  void select_tree_row_signal(GtkTreeSelection *selection, gpointer data){
    FILE_CATALOG *fileCatalog = (FILE_CATALOG *)data;
    FILE_CATALOG *tempfileCatalog = NULL;
    FILE_DETAIL_CATALOG *detailfile = NULL;
    
    gchar *filename = NULL;//要打开的文件名
    gchar *tempfilename = NULL;//
    
    GtkTreeIter iter; 
    GtkTreeModel *model = NULL;
    gchar *text = NULL;
    gchar *selectIndex = NULL;//返回数据格式为:(xx:yy)，按照两层目录结构，可以解析出来我们需要的文件所在的目录以及它指向的文件名
    
    if(gtk_tree_selection_get_selected(selection,&model,&iter)){
      gtk_tree_model_get(model,&iter,TITLE_COLUMN,&text,-1);
      g_print("选中树:%s\n",text);
      
      selectIndex = gtk_tree_model_get_string_from_iter(model,&iter);
      g_print("依据这里可以得到了。哈哈=>  %s,%c,%d\n",selectIndex,selectIndex[0],selectIndex[0]);
      
      int i = 0;
      int k = 0;
      short hasDoublePoint = 0;//是否有冒号，用于判断是否点击第二层目录
      int first = 0;//第一层
      int second = 0;//第二层
      int temp = 0;  //计算当前权数
      int nums = 0;//当前数的位数-1
      
      for(i=0 ; i<strlen(selectIndex) ; i++){
         if(selectIndex[i] == ':'){
            fprintf(stderr,"点击第二层\n");
            hasDoublePoint = 1;
            nums = 0;
         }else{
           if(!hasDoublePoint){ //正在读一级目录序
              temp = 0;
              for(k=0 ; k<nums ; k++){
                temp = temp*10;
              }
              
              if(temp==0)
                 first += selectIndex[i]-48;
              else
                 first += ((selectIndex[i]-48)*temp);
             
              nums++;
           }else{ //正在读二级目录序
             temp = 0;
              for(k=0 ; k<nums ; k++){
                temp = temp*10;
              }
              
              if(temp==0)
                 second += selectIndex[i]-48;
              else
                 second += ((selectIndex[i]-48)*temp);
              nums++;
           }
         }
      }
      
      
      
      fprintf(stderr,"点击为  first:%d  second:%d  hasDoublePoint:%d\n",first,second,hasDoublePoint);
      
      if(hasDoublePoint && first>=0 && second>=0){
	  tempfileCatalog = fileCatalog;
	  for(i=0 ; i<first ; i++)
	     tempfileCatalog = tempfileCatalog->brother;
	     
	  filename = (gchar *)malloc(strlen(tempfileCatalog->catalog_name)+9);
	  memset(filename,0x00,strlen(tempfileCatalog->catalog_name)+9);
	  strcpy(filename,"./data/");
	  strcat(filename,tempfileCatalog->catalog_name);  //获取到一级目录
	  strcat(filename,"/");
	  
	  detailfile =  tempfileCatalog->headChild;
	  for(i=0 ; i<second ; i++)
	     detailfile = detailfile->brother;
	  
	  tempfilename = (gchar *)malloc(strlen(filename)+1);
	  memset(tempfilename,0x00,strlen(filename)+1);
	  strcpy(tempfilename,filename);
	  
	  free(filename);
	  
	  filename = (gchar *)malloc(strlen(tempfilename)+strlen(detailfile->file_name)+1);
          memset(filename,0x00,strlen(tempfilename)+strlen(detailfile->file_name)+1);
          strcpy(filename,tempfilename);
          strcat(filename,detailfile->file_name);
          
          free(tempfilename);
          
          init_main_view_content(filename,selectIndex);
          
          free(filename);
      }
      
      free(text); 
    }
 }
  
  /**
  *按钮保存事件
  */
 void button_save_signal(GtkButton *button,gpointer data){
    g_printf("点击保存按钮\n");
    save_log();//保存日志
 }
 
 /**
  * 按钮删除信号事件
  */
 void button_delete_signal(GtkButton *button,gpointer data){
   g_printf("点击删除按钮\n");
   delete_log(); //删除日志
 }
 
 /**
  * 按钮修改信号事件
  */
 void button_modify_signal(GtkButton *button,gpointer data){
   g_printf("点击修改按钮\n");
   modify_log();//修改文件
 }
 
 
