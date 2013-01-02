/**
 * 作者：IT垃圾
 * e-mail:liangguangjin2006@163.com
 * 日期：2012－04－01
 * 描述：UI相关函数
 */
 
#include "ui.h"
#include "signalAction.h"
  
 GtkWidget *mainWindow = NULL;//主窗口，用于确定弹出框的你窗口 
 //用于共享--信号处理函数相关
 GtkWidget *contentTitleEntry = NULL;//内容标题输入框
 GtkWidget *contentContentTextView = NULL;  //内容输入框
 //内容textview相关
 GtkTextTagTable *textTagTable = NULL;
 GtkTextBuffer *textBuffer = NULL;
 GtkWidget *saveButton = NULL;//保存按钮
 GtkWidget *deleteButton = NULL;//删除按钮
 GtkWidget *modifyButton = NULL;//修改按钮
 char *modifyFileName = NULL;//要修改的文件名
 char *selectedTreeIndexStr = NULL;//选中修改的项在file层树目录中的索引
 
 FILE_CATALOG *fileCatalogInUi = NULL;//保持与main.c目录一致，方便使用
 
 GtkWidget *dialog = NULL;//弹出框
 GtkWidget *aboutDialog = NULL;
 
 /**
  *设置mainWindow，与main.c同步
  */
 void set_main_window(GtkWidget *mv){
    mainWindow = mv; 
 }
 
 /**
 *设置file层CATALOG，保持与main.c目录一致，方便使用
 */
void set_file_catalog(FILE_CATALOG  *tempcatalog){
  fileCatalogInUi = tempcatalog;
}
 
 /*
  * 创建主界口，并设置相关信号
  * @return window窗口  
  */
 GtkWidget* create_main_window(){
    g_print("开始创建窗口\n");
    GtkWidget *window = NULL;//主窗口
    
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL); //新建窗口
    gtk_window_set_title(GTK_WINDOW(window),"劲哥日志");//设置窗口标题
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER); //设置窗口初始位置
    gtk_window_set_default_size(GTK_WINDOW(window),500,300);//设置默认大小
    g_signal_connect(G_OBJECT(window),"delete_event",G_CALLBACK(gtk_main_quit),NULL);  //设置窗口销毁信号处理
    g_print("窗口创建完毕\n");    
    return window;
 }
 
 /**
  * 创建菜单栏
  * @return menubar菜单栏
  */
 GtkWidget* create_menus(){
    g_print("创新菜单栏\n");
    GtkWidget *menubar = NULL; //菜单栏
    GtkWidget *rootMenu = NULL;//每项的菜单根
    GtkWidget *menu = NULL;//菜单
    GtkWidget *menuItem = NULL; //菜单项
    
    menu = gtk_menu_new();//创建菜单
    
    menuItem = gtk_menu_item_new_with_label("新建");  //创建新建选项
    g_signal_connect(G_OBJECT(menuItem),"activate",G_CALLBACK(menu_signal_new),NULL); //添加新建信号处理
    gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuItem);//添加进菜单
    
    menuItem = gtk_menu_item_new_with_label("退出");  //创建退出选项
    g_signal_connect(G_OBJECT(menuItem),"activate",G_CALLBACK(menu_signal_quit),NULL); //添加退出信号处理
    gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuItem);//添加进菜单
    
    rootMenu = gtk_menu_item_new_with_label("文件"); //创建文件项菜单
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(rootMenu),menu); //添加子项
    
    menubar = gtk_menu_bar_new();  //创建菜单栏
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar),rootMenu); //向菜单栏添加项
    
    //*******************************
    menu = gtk_menu_new(); //创建帮助项主菜单
    menuItem = gtk_menu_item_new_with_label("关于作者"); //创建关于项
    g_signal_connect(G_OBJECT(menuItem),"activate",G_CALLBACK(menu_signal_about_author),NULL);//添加关于作者信号处理
    gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuItem); //添加子项
    
    rootMenu = gtk_menu_item_new_with_label("帮助"); //创建帮助
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(rootMenu),menu); //添加子项
    
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar),rootMenu);
    
    g_print("菜单栏创建完毕\n");
   return menubar;    
 }
 
  /**
  *创建树目录
  * @param catalogStruct 目录参数
  ＊　本版本程序目录最多为二级
  */
GtkWidget* create_catalog(const CATALOG *catalog,const FILE_CATALOG *fileCatalog){
   g_print("开始创建目录\n");
   GtkWidget *tree = NULL;  //树视图
   GtkTreeStore *store = NULL;//树模型存今储器
   GtkTreeViewColumn *column = NULL; //树列
   GtkCellRenderer *renderer = NULL;  //树渲染器
   GtkTreeIter iter1;//树迭代器
   GtkTreeIter iter2;
   CATALOG *temp_catalog = NULL; //目录
   CATALOG *temp_catalog_2 = NULL;
   GtkAdjustment *vAdjustment = NULL;
   GtkAdjustment *hAdjustment = NULL;
   GtkWidget *scrollWindow = NULL;
   
   if(catalog == NULL){ 
      g_print("目录数据结构为NULL");
      return NULL;
   }
   
   temp_catalog = catalog;
   
   //创建树模型--MVC中的M,model
   store = gtk_tree_store_new(N_COLUMNS,G_TYPE_STRING); //只创建一列，为字符类型
   
   //目录最多只有两层，在些基础上设计算法
   do{
      //设置一级目录
      gtk_tree_store_append(store,&iter1,NULL);
      gtk_tree_store_set(store,&iter1,
                         TITLE_COLUMN,temp_catalog->text,
                         -1);
      
                         
      //设置二级目录
      temp_catalog_2 = temp_catalog->headChild;
      while(temp_catalog_2 != NULL){
         gtk_tree_store_append(store,&iter2,&iter1);
         gtk_tree_store_set(store,&iter2,
                            TITLE_COLUMN,temp_catalog_2->text,
                            -1);
         temp_catalog_2 = temp_catalog_2->brother;
      }
      
      temp_catalog = temp_catalog->brother;
   }while(temp_catalog != NULL);
   
   
   //创建树视图
   tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
   
   renderer = gtk_cell_renderer_text_new();
   
   column = gtk_tree_view_column_new_with_attributes("标题",
                                                      renderer,
                                                      "text",TITLE_COLUMN,
                                                      NULL);
   gtk_tree_view_append_column(GTK_TREE_VIEW(tree),column);
   
   GtkTreeSelection *select = NULL;
   select = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));
   gtk_tree_selection_set_mode(select,GTK_SELECTION_SINGLE);
   g_signal_connect(G_OBJECT(select),"changed",
                    G_CALLBACK(select_tree_row_signal),(gpointer *)fileCatalog);
                    
   vAdjustment = gtk_adjustment_new(0,0,100,10,10,10);
   hAdjustment = gtk_adjustment_new(0,0,100,10,10,10);
   scrollWindow = gtk_scrolled_window_new(hAdjustment,vAdjustment);
   
   gtk_scrolled_window_add_with_viewport(scrollWindow,tree);          
   
   g_print("创建目录完毕\n");
   return scrollWindow;
}

/**
 *创建右边主界面内容
 */
 GtkWidget* init_right_main_view(void){
   GtkWidget *vBox = NULL; //竖盒布局
   GtkWidget *hBox = NULL;//竖盒布局
   GtkWidget *titleLabel = NULL;//标题标签
   GtkWidget *contentTitle = NULL;//内容标题标签
   //GtkWidget *contentTitleEntry = NULL;//内容标题输入框
   GtkWidget *contentContent = NULL;//具体内容标签
   //GtkWidget *contentContentTextView = NULL;  //内容输入框
   //GtkWidget *saveButton = NULL;//保存按钮
   //GtkWidget *deleteButton = NULL;//删除按钮
   //GtkWidget *modifyButton = NULL;//修改按钮
   GtkAdjustment *vAdjustment = NULL;
   GtkAdjustment *hAdjustment = NULL;
   GtkWidget *scrollWindow = NULL;
   
   if(textTagTable == NULL)
      textTagTable = gtk_text_tag_table_new();
    if(textBuffer == NULL)
       textBuffer = gtk_text_buffer_new(textTagTable);
   
   vBox = gtk_vbox_new(FALSE,5); //新建布局
   titleLabel = gtk_label_new("欢迎使用"); //新建标题标签
   contentTitle = gtk_label_new("标题:");
   contentTitleEntry = gtk_entry_new();
   
   contentContent = gtk_label_new("内容:");
   contentContentTextView = gtk_text_view_new();
   
   gtk_box_pack_start(GTK_BOX(vBox),
                      titleLabel,
                      FALSE,FALSE,5);
   hBox = gtk_hbox_new(FALSE,5);
   gtk_box_pack_start(GTK_BOX(hBox),
                      contentTitle,
                      FALSE,FALSE,5);
   gtk_box_pack_start(GTK_BOX(hBox),
                      contentTitleEntry,
                      TRUE,TRUE,5);
   gtk_box_pack_start(GTK_BOX(vBox),
                      hBox,
                      FALSE,FALSE,5);
   
   hBox = gtk_hbox_new(FALSE,5);
   gtk_box_pack_start(GTK_BOX(hBox),
                      contentContent,
                      FALSE,FALSE,5);                   
   gtk_box_pack_start(GTK_BOX(hBox),
                      contentContentTextView,
                      TRUE,TRUE,5);    
   gtk_box_pack_start(GTK_BOX(vBox),
                      hBox,
                      FALSE,FALSE,5); 
                      
   saveButton = gtk_button_new_with_label("保存");
   g_signal_connect(G_OBJECT(saveButton),"clicked",G_CALLBACK(button_save_signal),NULL);
   deleteButton = gtk_button_new_with_label("删除");
   g_signal_connect(G_OBJECT(deleteButton),"clicked",G_CALLBACK(button_delete_signal),NULL);
   modifyButton = gtk_button_new_with_label("修改");
   g_signal_connect(G_OBJECT(modifyButton),"clicked",G_CALLBACK(button_modify_signal),NULL);
   hBox = gtk_hbox_new(FALSE,5);
   gtk_box_pack_start(GTK_BOX(hBox),
                      gtk_label_new(""),
                      TRUE,TRUE,5);
   gtk_box_pack_start(GTK_BOX(hBox),
                      saveButton,
                      FALSE,FALSE,5);
   gtk_box_pack_start(GTK_BOX(hBox),
                      deleteButton,
                      FALSE,FALSE,5);
   gtk_box_pack_start(GTK_BOX(hBox),
                      modifyButton,
                      FALSE,FALSE,5);
   
   
   gtk_box_pack_start(GTK_BOX(vBox),
                      hBox,
                      FALSE,FALSE,5);
   
   vAdjustment = gtk_adjustment_new(0,0,100,10,10,10);
   hAdjustment = gtk_adjustment_new(0,0,100,10,10,10);
   scrollWindow = gtk_scrolled_window_new(hAdjustment,vAdjustment);
   
   gtk_scrolled_window_add_with_viewport(scrollWindow,vBox);
                        
   return scrollWindow;
 }
 
 /**
 *完成由文件层目录结构转换为UI层目录结构
 */
CATALOG *convert_file_struct_to_ui_struct_catalog(const FILE_CATALOG *file_src_catalog){
   /**
  *  目录
  
	 typedef struct UI_CATALOG{
	    int id;//ID
	    char* text;//内容
	    struct UI_CATALOG *brother;//下一个兄弟结点，没有为NULL
	    struct UI_CATALOG *headChild;//长子结点,没有为NULL
	 }CATALOG;*/
   	 
   fprintf(stderr,"开始转换\n");
   CATALOG *catalog = NULL;
   CATALOG *tempC = NULL;
   CATALOG *tempHC = NULL;
   CATALOG *childHC = NULL;
   CATALOG *childC = NULL;
   
   free(catalog);
   catalog = NULL;
   
   
   FILE_CATALOG *tempCatalog = NULL; //当前一级目录
   FILE_DETAIL_CATALOG *temp_son_catalog = NULL;//当前二级目录
   fprintf(stderr,"劲==%s\n",file_src_catalog);
  tempCatalog = file_src_catalog; 
  while(tempCatalog != NULL){
     tempC = (CATALOG*)malloc(sizeof(CATALOG));
     fprintf(stderr,"劲==\n");
     fprintf(stderr,"劲吹哥：%s\n",tempCatalog->catalog_name);
     tempC->text = (char *)malloc(strlen(tempCatalog->catalog_name)+1);
     strcpy(tempC->text,tempCatalog->catalog_name);
     tempC->headChild = NULL;
     tempC->brother = NULL;
     temp_son_catalog = tempCatalog->headChild;
     
     fprintf(stderr,"一级目录%s\n",tempCatalog->catalog_name);
     
     while(temp_son_catalog!=NULL){
       childC = (CATALOG*)malloc(sizeof(CATALOG));
       childC->text = (char *)malloc(strlen(temp_son_catalog->title)+1);
       strcpy(childC->text,temp_son_catalog->title);
       childC->headChild = NULL;
       childC->brother = NULL;
        
       if(tempC->headChild == NULL){  //头一个结点
          childHC = childC;
          tempC->headChild = childHC;
       }else{
          childHC->brother = childC;
          childHC = childC;
       }
       fprintf(stderr,"二级目录%s\n",temp_son_catalog->title);
       temp_son_catalog = temp_son_catalog->brother;
     }
     tempCatalog = tempCatalog->brother;
     
     if(catalog == NULL){
          tempHC = tempC;
          catalog = tempHC;
       }else{
         tempHC->brother = tempC;
         tempHC = tempC;
       }
  }
  fprintf(stderr,"已完成结转换\n\n\n");
  
  return catalog;
}

/**
 *显示文章具体内容
 * filename 文件名
 * selectIndex 在树中选中的位置索引，与树file层数据结构对应
 */
void init_main_view_content(const char *filename,const char *selectIndex){
  selectedTreeIndexStr = selectIndex;
  
  fprintf(stderr,"开始显示文章:%s  %s\n",filename,selectedTreeIndexStr);
  
  if(modifyFileName!=NULL){
     free(modifyFileName);
  }
  modifyFileName = (char *)malloc(strlen(filename)+1);
  memset(modifyFileName,0x00,strlen(filename)+1);//设置全局变量
  strcpy(modifyFileName,filename);
    
  FILE_CONTENT *filecontent = read_log_file(filename);
   
  if(filecontent != NULL){
    gtk_widget_hide(saveButton);
    
    gtk_entry_set_text(GTK_ENTRY(contentTitleEntry),filecontent->title); //设置标题
    
    gtk_text_buffer_set_text(textBuffer,filecontent->content,strlen(filecontent->content));
    gtk_text_view_set_buffer(GTK_TEXT_VIEW(contentContentTextView),textBuffer);
    
    gtk_widget_hide(saveButton);
    gtk_widget_show(contentTitleEntry);
    gtk_widget_show(contentContentTextView);
    gtk_widget_show(modifyButton);
    gtk_widget_show(deleteButton);
      
  }else{ //读文件出错
  }
  
}

/**
 *在点击菜单新建项后，初始化界面
 */
 void init_new_content_views(void){
    gtk_entry_set_text(GTK_ENTRY(contentTitleEntry),"");
    gtk_text_buffer_set_text(textBuffer,"",0);
    gtk_text_view_set_buffer(GTK_TEXT_VIEW(contentContentTextView),textBuffer);
    
    gtk_widget_show(contentTitleEntry);
    gtk_widget_show(contentContentTextView);
    gtk_widget_show(saveButton);
    gtk_widget_hide(deleteButton);
    gtk_widget_hide(modifyButton);
 
 }
 
 /**
  *初始化右边主界面,所有不可见
  *@param str如果不为空，则弹出框显示str信息
  */
 void init_right_main_view_hide_all(const char *str){
    if(str != NULL){
          dialog = gtk_message_dialog_new(GTK_WINDOW(mainWindow),
                                           GTK_DIALOG_MODAL,
                                           GTK_MESSAGE_INFO,
                                           GTK_BUTTONS_OK,
                                           str);
       gtk_message_dialog_set_markup(GTK_MESSAGE_DIALOG(dialog),str);
       gtk_dialog_run(GTK_DIALOG(dialog));
       gtk_widget_destroy(dialog);
    }
    gtk_entry_set_text(GTK_ENTRY(contentTitleEntry),"");
    gtk_text_buffer_set_text(textBuffer,"",0);
    gtk_text_view_set_buffer(GTK_TEXT_VIEW(contentContentTextView),textBuffer);
    
    gtk_widget_hide(contentTitleEntry);
    gtk_widget_hide(contentContentTextView);
    gtk_widget_hide(saveButton);
    gtk_widget_hide(deleteButton);
    gtk_widget_hide(modifyButton);
 }
 
 /**
  *保存日志
  */
  void save_log(void){
    gchar *title = gtk_entry_get_text(GTK_ENTRY(contentTitleEntry));
    GtkTextBuffer *gtb = gtk_text_view_get_buffer(GTK_TEXT_VIEW(contentContentTextView));
    GtkTextIter start;
    GtkTextIter end;
    SAVE_LOG_FILE_SUCCESS_RETURN_TYPE *saveSuccess = NULL;//是否保存成功
    char *catalogFileStr = NULL; //目录文件字符串
    
    gtk_text_buffer_get_start_iter(gtb,&start);
    gtk_text_buffer_get_end_iter(gtb,&end);
    
    gchar *content = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(gtb),&start,&end,TRUE);
    
    saveSuccess = save_log_file(SAVE_NEW_LOG_FILE,title,content,NULL);
    
    if(saveSuccess!=NULL){//内容保存成功，接下来更新目录文件以及视图
       catalogFileStr = convert_file_to_str_catalog(fileCatalogInUi,title,saveSuccess);  
       fprintf(stderr,"修改目录文件%d\n",save_or_modify_catalog_file(catalogFileStr));       
       free(catalogFileStr);
       
       free(saveSuccess);
       init_right_main_view_hide_all("新增成功");
       init_tree_view_catalog(); //刷新树目录;
    }else{ //内容保存失败
       init_right_main_view_hide_all("新增失败");
    }
    
  }
  
/**
 *修改日志
 */
 void modify_log(void){
    gchar *title = gtk_entry_get_text(GTK_ENTRY(contentTitleEntry)); //获取标题
    GtkTextBuffer *gtb = gtk_text_view_get_buffer(GTK_TEXT_VIEW(contentContentTextView));
    GtkTextIter start;
    GtkTextIter end;
    SAVE_LOG_FILE_SUCCESS_RETURN_TYPE *saveSuccess = NULL;//是否保存成功
    
    gtk_text_buffer_get_start_iter(gtb,&start);
    gtk_text_buffer_get_end_iter(gtb,&end);
    
    gchar *content = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(gtb),&start,&end,TRUE);  //获取内容
    
    saveSuccess = save_log_file(SAVE_MODIFY_LOG_FILE,title,content,modifyFileName);
    
    fprintf(stderr,"要修改的文件为%s  标题%s  内容%s  是否保存成功%s\n",modifyFileName,title,content,(saveSuccess==NULL?"否":"是"));
    
    if(saveSuccess!=NULL){
      int i = 0;
      int k = 0;
      short hasDoublePoint = 0;//是否有冒号，用于判断是否点击第二层目录
      int first = 0;//第一层
      int second = 0;//第二层
      int temp = 0;  //计算当前权数
      int nums = 0;//当前数的位数-1
      char *catalogFileStr = NULL; //目录文件字符串
      
      for(i=0 ; i<strlen(selectedTreeIndexStr) ; i++){
         if(selectedTreeIndexStr[i] == ':'){
            hasDoublePoint = 1;
            nums = 0;
         }else{
           if(!hasDoublePoint){ //正在读一级目录序
              temp = 0;
              for(k=0 ; k<nums ; k++){
                temp = temp*10;
              }
              
              if(temp==0)
                 first += selectedTreeIndexStr[i]-48;
              else
                 first += ((selectedTreeIndexStr[i]-48)*temp);
             
              nums++;
           }else{ //正在读二级目录序
             temp = 0;
              for(k=0 ; k<nums ; k++){
                temp = temp*10;
              }
              
              if(temp==0)
                 second += selectedTreeIndexStr[i]-48;
              else
                 second += ((selectedTreeIndexStr[i]-48)*temp);
              nums++;
           }
         }
      }
      
      FILE_CATALOG *tempfc = fileCatalogInUi;
      for(i=0 ; i<first ; i++)
         tempfc = tempfc->brother;
         
      FILE_DETAIL_CATALOG *tempdc = tempfc->headChild;
      for(i=0 ; i<second ; i++)
        tempdc = tempdc->brother;
      
      if(tempdc->title!=NULL)
        free(tempdc->title);
      tempdc->title = (char *)malloc(strlen(title)+1);
      memset(tempdc->title,0x00,strlen(title)+1);
      strcpy(tempdc->title,title);
      catalogFileStr = convert_file_to_str_catalog(fileCatalogInUi,NULL,NULL);  
      fprintf(stderr,"劲哥你好:%s\n",catalogFileStr);
      fprintf(stderr,"修改目录文件%d\n",save_or_modify_catalog_file(catalogFileStr));     
      fprintf(stderr,"点击的  first:%d  second:%d  原标题为：%s\n",first,second,tempdc->title);
      
      init_right_main_view_hide_all("修改成功");
      init_tree_view_catalog();//刷新树目录
    }else{
      init_right_main_view_hide_all("修改失败");
    }
    
    free(saveSuccess);    
    
 }

/**
 *删除日志
 */
void delete_log(void){
   if(!delete_log_file(modifyFileName)){
      int i = 0;
      int k = 0;
      short hasDoublePoint = 0;//是否有冒号，用于判断是否点击第二层目录
      int first = 0;//第一层
      int second = 0;//第二层
      int temp = 0;  //计算当前权数
      int nums = 0;//当前数的位数-1
      char *catalogFileStr = NULL; //目录文件字符串
      char *deleteDir = NULL;//如果是删除第一层，则连文件夹一起删除
      
      for(i=0 ; i<strlen(selectedTreeIndexStr) ; i++){
         if(selectedTreeIndexStr[i] == ':'){
            hasDoublePoint = 1;
            nums = 0;
         }else{
           if(!hasDoublePoint){ //正在读一级目录序
              temp = 0;
              for(k=0 ; k<nums ; k++){
                temp = temp*10;
              }
              
              if(temp==0)
                 first += selectedTreeIndexStr[i]-48;
              else
                 first += ((selectedTreeIndexStr[i]-48)*temp);
             
              nums++;
           }else{ //正在读二级目录序
             temp = 0;
              for(k=0 ; k<nums ; k++){
                temp = temp*10;
              }
              
              if(temp==0)
                 second += selectedTreeIndexStr[i]-48;
              else
                 second += ((selectedTreeIndexStr[i]-48)*temp);
              nums++;
           }
         }
      }
      
      //***********START 删除目录**************
      //FILE_CATALOG *fileCatalogInUi
      FILE_CATALOG *tfc = fileCatalogInUi;
      FILE_CATALOG *ttffcc = NULL;
      for(i=0 ; i<first ; i++)
        tfc = tfc->brother;
      
      FILE_DETAIL_CATALOG *tfdc = tfc->headChild;
      FILE_DETAIL_CATALOG *ttffddcc = NULL;
      for(i=0 ;; i++){
        if(i == second){
           if(second==0){
              if(tfdc->brother == NULL){ //如果是第一个且没有下一个，则连第一层一起删除
                if(first==0){ //删除第一层的第一个
                  tfc = fileCatalogInUi->brother;
                  deleteDir = (char *)malloc(strlen(fileCatalogInUi->catalog_name)+8);
                  memset(deleteDir,0x00,strlen(fileCatalogInUi->catalog_name)+8);
                  strcpy(deleteDir,"./data/");
                  strcat(deleteDir,fileCatalogInUi->catalog_name);
                  free(fileCatalogInUi);
                  fileCatalogInUi = tfc;
                  fprintf(stderr,"删除第一层第一个\n");
                }else{
                  tfc = fileCatalogInUi;
                  for(k=0 ; tfc!=NULL ; k++){
                     if(k == first-1){
                        ttffcc = tfc->brother;
                        tfc->brother = ttffcc->brother;
                        deleteDir = (char *)malloc(strlen(fileCatalogInUi->catalog_name)+8);
                        memset(deleteDir,0x00,strlen(ttffcc->catalog_name)+8);
                        strcpy(deleteDir,"./data/");
                        strcat(deleteDir,ttffcc->catalog_name);
                        free(ttffcc);
                        fprintf(stderr,"去掉非第一个的第一层\n");
                     }
                     tfc = tfc->brother;
                  }
                }
              }else{//改变头结点
                 tfc->headChild = tfdc->brother;
                 free(tfdc);
                 fprintf(stderr,"改变头子结点\n");
              }
           }else{
             for(k=0 ;; k++){
                if(k == (second-1)){
                   ttffddcc = tfdc->brother;
                   tfdc->brother = ttffddcc->brother;
                   free(ttffddcc);
                   fprintf(stderr,"第二层非第一个去掉\n");
                   break;
                }
                   tfdc = tfdc->brother;
             }
           }
           //tfdc = tfdc->brother;
           break;
        }
      }
      //**********END***************
      
      catalogFileStr = convert_file_to_str_catalog(fileCatalogInUi,NULL,NULL); 
      fprintf(stderr,"劲测试目录:%s\n",catalogFileStr);
      //更新目录
      if(!save_or_modify_catalog_file(catalogFileStr)){
        init_right_main_view_hide_all("日志删除成功");
        if(deleteDir != NULL)
	   delete_dir(deleteDir); //删除文件夹
        init_tree_view_catalog();//刷新树
      }else{
        init_right_main_view_hide_all("日志删除失败");
      }
      
      free(catalogFileStr);
   }else{
     init_right_main_view_hide_all("日志删除失败");
   }
}

/**
 *关于作者
 */
void about_author(void){
  if(aboutDialog == NULL){
          aboutDialog = gtk_message_dialog_new(GTK_WINDOW(mainWindow),
                                           GTK_DIALOG_MODAL,
                                           GTK_MESSAGE_INFO,
                                           GTK_BUTTONS_OK,
                                           "作者:IT 垃圾\n\
                                           e-mail:liangguangjin2006@163.com");
          gtk_dialog_set_default_response(GTK_MESSAGE_DIALOG(aboutDialog),
                                          GTK_BUTTONS_OK);
       }
       gtk_message_dialog_set_markup(GTK_MESSAGE_DIALOG(aboutDialog),"作者:IT 垃圾\ne-mail:liangguangjin2006@163.com");
       gtk_dialog_run(GTK_DIALOG(aboutDialog));
       gtk_widget_destroy(aboutDialog);
       aboutDialog = NULL;
}

 
