/**
 * 作者：IT垃圾
 * e-mail:liangguangjin2006@163.com
 * 日期：2012－04－07
 * 描述：文件相关操作
 * 感谢：read_catalog_file（）方法，由C-aries提供修改意见。
 */

#include <string.h>
#include <errno.h>
#include <time.h>
#include "files.h"

/**
  *检查是否存在存文件的目录，没有则创建
  *@return 1 存在/创建成功,0：不存在/创建失败
  */
int check_dir(void){
  DIR *dp;
  if((dp=opendir(DIR_NAME))!=NULL){ //目录存在
     closedir(dp);
     return 1;
  }else{ //目录不存在
     return !(-1 * mkdir(DIR_NAME,0777));
  }
}

/**
  *检查是data下否存在存文件的目录，没有则创建
  * @param dirName文件夹名
  *@return 1 存在/创建成功,0：不存在/创建失败
  */
int check_dir_data(char *dirName){
  DIR *dp = NULL;
  char *path = NULL;
  
  path = (char *)malloc(sizeof(char)*20);
  memset(path,0x00,sizeof(char)*20);
  strcpy(path,"./");
  strcat(path,DIR_NAME);
  strcat(path,"/");
  strcat(path,dirName);
  
  if((dp=opendir(path))!=NULL){ //目录存在
     closedir(dp);
     return 1;
  }else{ //目录不存在
     return !(-1 * mkdir(path,0777));
  }
}

/**
  *　读取数据目录文件
  * @param fileCatalog:目录结构指针
  * 注意内存分配问题
  */
FILE_CATALOG *read_catalog_file(void){
    FILE_CATALOG *fileCatalog = NULL;
    int fd;//文件描述符
    int readSize = 0;//读取数据的大小
    char *text = NULL;//目录内容
    char *buffer = NULL;//字符缓冲区
    char *temp = NULL;
    
    buffer = (char *)malloc(2*sizeof(char));
    memset(buffer,0x00,2*sizeof(char));
    
    text = (char *)malloc(sizeof(char));
    memset(text,0x00,sizeof(text));
    
    if((fd=open(CATALOG_FILE,O_RDONLY))!=-1){
       while((readSize = read(fd,buffer,1))>0){
          //fprintf(stderr,"%s",buffer);
          if(*text == '\0' || text == NULL){
            //temp = (char *)malloc(readSize);
            temp = (char *)malloc(readSize+1);
            strcpy(temp,buffer);
            free(text);
            //text = (char *)malloc(sizeof(temp));
            text = (char *)malloc(readSize+1);
            strcpy(text,temp);
          }else{
            free(temp);
            //temp = (char *)malloc(sizeof(text)+sizeof(buffer)+1);
            //memset(temp,0x00,sizeof(temp));
            temp = (char *)malloc(strlen(text)+strlen(buffer)+1);
            memset(temp,0x00,strlen(text)+strlen(buffer)+1);
            strcpy(temp,text);
            strcat(temp,buffer);
            
            if(text!=NULL || *text!='\0')
               free(text);
            //text = (char *)malloc(sizeof(temp));
            //memset(text,0x00,sizeof(text));
            text = (char *)malloc(strlen(temp)+1);
            memset(text,0x00,strlen(temp)+1);
            strcat(text,temp);
          }
       }
       
       fprintf(stderr,"目录文件内容为:%s\n",text);
       close(fd);
       
       fileCatalog = parset_string_to_file_catalog(text);
       
       if(buffer!=NULL || *buffer!='\0')
         free(buffer);
       if(temp!=NULL || *temp!='\0')
         free(temp);
       if(text!=NULL || *text!='\0')
         free(text);
       fprintf(stderr,"读目录文件完毕\n");
    }else{
       fileCatalog = NULL;
       fprintf(stderr,"读目录文件出错\n");
       perror("读目录文件");
       return; 
    }
    return fileCatalog;
}

/**
  *解析字符串，生成目录结构
  * @param src:目录结构字符串
  * @return FILE_CATALOG fileCatalog:目录结构指针
  */
FILE_CATALOG *parset_string_to_file_catalog(char *src){
  FILE_CATALOG *fileCatalog = NULL;
  FILE_CATALOG *currentCatalog = NULL; //当前一级目录
  FILE_CATALOG *tempCatalog = NULL; //当前一级目录
  FILE_CATALOG *tempSwapCatalog = NULL; //当前一级目录
  
  FILE_DETAIL_CATALOG *head_son_catalog = NULL;//二级目录头结点  ]
  FILE_DETAIL_CATALOG *current_son_catalog = NULL;//当前二级目录
  FILE_DETAIL_CATALOG *temp_son_catalog = NULL;//当前二级目录
  FILE_DETAIL_CATALOG *temp_swap_son_catalog = NULL;//当前二级目录
  
  int index = 0;//当前读到源串的下标
  short isBeginFlow = 0; //是否已读到{
  short isBeginMiddle = 0; //是否已读到[
  char *buffer = NULL;//读到的有用值的缓冲区
  char *swap = NULL;//临时交换区
  
  fprintf(stderr,"要解析的为：%s\n",src);
  
  if(fileCatalog !=NULL)
    fileCatalog = NULL;
  
  if(src == NULL || *src=='\0'){
     fileCatalog = NULL;
     return;
  }
  
  if(src[strlen(src)-1] == '\n'){
    fprintf(stderr,"劲哥你:%s||%c|\n",src,src[strlen(src)-1]);
    src[strlen(src)-1] = '\0'; 
  }
  
  if(src[0] != '['){ //不是以'['开头，格式不对
    fileCatalog = NULL;
    return;
  }
  
  if(src[strlen(src)-1] != ']'){ //不是以']'结尾，格式不对  //经调试，可能问题出在这里
     fileCatalog = NULL;
     fprintf(stderr,"不是以]结尾%c\n",src[strlen(src)-1]);
     return;
  }
  
  buffer = (char *)malloc(2*sizeof(char));
  memset(buffer,0x00,2*sizeof(char));
  
  //[{2012-01:[我的文章1*liangtxt1=我的文章1*liangtxt2]},{2012-01:[我的文章1*liangtxt1=我的文章1*liangtxt2]}]
  fprintf(stderr,"要解析的：%s\n",src);
  for(index=1 ; index<(strlen(src)-1) ; index++){
    //{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{
    if(src[index] == '{'){  //读到{，则是进入第一层目录
       fprintf(stderr,"进入{\n");
       if(isBeginFlow){  //已经开始了，现在又开始，则数据格式出错
          fileCatalog = NULL;
          fprintf(stderr,"{已经开始了，现在又开始，则数据格式出错\n");
          return;
       }
       isBeginFlow = 1;
       free(buffer);
       buffer = NULL;
       free(swap);
       swap = NULL;
       currentCatalog = (FILE_CATALOG *)malloc(sizeof(FILE_CATALOG));
     //[{2012-01:[我的文章1*liangtxt1=我的文章1*liangtxt2]},{2012-01:[我的文章1*liangtxt1=我的文章1*liangtxt2]}]   
    //}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}
    }else if(src[index] == '}'){  //读到}，则是刚读完一个完整的一层目录,接下来可能是 "," 说明还有其他二级目录
      fprintf(stderr,"进入}\n");
      isBeginFlow = 0;
      
      currentCatalog->brother = NULL;//下一个兄弟结点，单向链表,NULL为结束
      currentCatalog->headChild = head_son_catalog; //第一个子结点
      if(fileCatalog == NULL){
        fileCatalog = currentCatalog;
      }else{
         tempCatalog = fileCatalog->brother;
         while(tempCatalog !=NULL){
             if(tempCatalog->brother == NULL)
               break;
             tempCatalog = tempCatalog->brother;
         }
         if(tempCatalog == NULL)//只有一个一级目录头结点
           fileCatalog->brother = currentCatalog;//连接兄弟结点
         else
           tempCatalog->brother = currentCatalog;
     }
     
    head_son_catalog = NULL;
     
    free(buffer);
    buffer = NULL;
    free(swap);
    swap = NULL;
         
     //[{2012-01:[我的文章1*liangtxt1=我的文章1*liangtxt2]},{2012-01:[我的文章1*liangtxt1=我的文章1*liangtxt2]}]
    //,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
    }else if(src[index] == ','){  //说明有多个一级层,接下来应该是读下一个二级目录层的开始标记[
      fprintf(stderr,"进入,\n");
      
      
      if(buffer !=NULL)
        free(buffer);
      if(swap !=NULL)
        free(swap);
      //[{2012-01:[我的文章1*liangtxt1=我的文章1*liangtxt2]},{2012-01:[我的文章1*liangtxt1=我的文章1*liangtxt2]}] 
    //[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
    }else if(src[index] == '['){ //开始读二级目录,接下来应该是读二级目录的标题
      fprintf(stderr,"进入[\n");
      if(isBeginMiddle){  //已经开始了，现在又开始，则数据格式出错
          fileCatalog = NULL;
          fprintf(stderr,"[已经开始了，现在又开始，则数据格式出错\n");
          return;
       }
       isBeginMiddle = 1;
       if(buffer !=NULL){
          free(buffer);
       }
       if(swap !=NULL)
          free(swap);  
       current_son_catalog = (FILE_DETAIL_CATALOG *)malloc(sizeof(FILE_DETAIL_CATALOG));
     //[{2012-01:[我的文章1*liangtxt1=我的文章1*liangtxt2]},{2012-01:[我的文章1*liangtxt1=我的文章1*liangtxt2]}]   
    //]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
    }else if(src[index] == ']'){ //刚读完一个二级目录,接下来应该是一级目录结束符}
      fprintf(stderr,"进入]\n");
      isBeginMiddle = 0;
      current_son_catalog->file_name = (char *)malloc(strlen(buffer)+1);
      strcpy(current_son_catalog->file_name,buffer); //取得文件名
      
      current_son_catalog->brother = NULL;
      if(head_son_catalog == NULL){
        head_son_catalog = current_son_catalog;
      }else{
        temp_son_catalog = head_son_catalog->brother;
        while(temp_son_catalog != NULL){
           if(temp_son_catalog->brother==NULL)
             break;
           temp_son_catalog = temp_son_catalog->brother;
        }
        
        if(temp_son_catalog == NULL)//只有一个二级目录头结点
           head_son_catalog->brother = current_son_catalog;
        else{
           temp_son_catalog->brother = current_son_catalog;
        }  
      }
      //[{2012-01:[我的文章1*liangtxt1=我的文章2*liangtxt2]},{2012-02:[我的文章3*liangtxt3=我的文章4*liangtxt4]},{2012-4:[dd*1334421653=aa*1334540184]}]
      
      free(buffer);
      buffer = NULL;
      free(swap);
      swap = NULL;
      
      //[{2012-01:[我的文章1*liangtxt1=我的文章1*liangtxt2]},{2012-01:[我的文章1*liangtxt1=我的文章1*liangtxt2]}]
    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    }else if(src[index] == ':'){
      currentCatalog->catalog_name = (char *)malloc(strlen(buffer)+1);
      strcpy(currentCatalog->catalog_name,buffer); //拿到一级标题
      fprintf(stderr,"进入:　　读到的一级标题是%s\n",currentCatalog->catalog_name);
      free(buffer);
      buffer = NULL;
      free(swap);
      swap = NULL;
     //[{2012-01:[我的文章1*liangtxt1=我的文章1*liangtxt2]},{2012-01:[我的文章1*liangtxt1=我的文章1*liangtxt2]}]
    //**************************************************************************
    }else if(src[index] == '*'){ //刚读完一个二级标题,接下来读的是此标题对应的文件名
      fprintf(stderr,"进入* 读到的二级标题是%s\n",buffer);
      current_son_catalog->title = (char *)malloc(strlen(buffer)+1);
      strcpy(current_son_catalog->title,buffer);  //拿到二级标题
      free(buffer);
      buffer = NULL;
      free(swap);
      swap = NULL;
     //[{2012-01:[我的文章1*liangtxt1=我的文章1*liangtxt2]},{2012-01:[我的文章1*liangtxt1=我的文章1*liangtxt2]}]  
    //=================================================================================
    }else if(src[index] == '='){ //刚读完一个二级目标中的一个具体标题文件对,是等号，说明还有兄弟结点
      fprintf(stderr,"进入=\n");
      current_son_catalog->file_name = (char *)malloc(strlen(buffer)+1);
      strcpy(current_son_catalog->file_name,buffer); //取得文件名
      
      current_son_catalog->brother = NULL;
      if(head_son_catalog == NULL){
        head_son_catalog = current_son_catalog;
      }else{
        temp_son_catalog = head_son_catalog->brother;
        while(temp_son_catalog != NULL){
           if(temp_son_catalog->brother==NULL)
               break;
           temp_son_catalog = temp_son_catalog->brother;
        }
        
        if(temp_son_catalog == NULL)//只有一个二级目录头结点
           head_son_catalog->brother = current_son_catalog;
        else{
           temp_son_catalog->brother = current_son_catalog;
        }  
      }
      
      current_son_catalog = (FILE_DETAIL_CATALOG *)malloc(sizeof(FILE_DETAIL_CATALOG));
      
      free(buffer);
      buffer = NULL;
      free(swap);
      swap = NULL;
      
      //[{2012-01:[我的文章1*liangtxt1=我的文章1*liangtxt2]},{2012-01:[我的文章1*liangtxt1=我的文章1*liangtxt2]}]
    //00000000000000000000000000000000000000000000000000000000000000000000000
    }else{//正文，放到缓冲区
      fprintf(stderr,"进入正文%c\n",src[index]);
      if(swap!=NULL){
         free(swap);
      }
      if(buffer != NULL){
         swap = (char *)malloc(strlen(buffer)+2);
         memset(swap,0x00,(strlen(buffer)+2));
         strcpy(swap,buffer);
      }else{ 
        swap = (char *)malloc(2*sizeof(char));
        memset(swap,0x00,(2*sizeof(char)));
      }
      
      swap[strlen(swap)] = src[index];
      
      free(buffer);
      buffer = (char *)malloc(strlen(swap)+1);
      memset(buffer,0x00,(strlen(swap)+1));
      strcpy(buffer,swap);
    }
    
  }
  fprintf(stderr,"最终剩下:swap:%s\tbuffer:%s\n",swap,buffer);
  free(buffer);
  buffer = NULL;
  free(swap);
  swap = NULL;
  
  
  //****************testing*********
  /*while(fileCatalog!=NULL){
     fprintf(stderr,"一级目录：%s ",fileCatalog->catalog_name);
     temp_son_catalog = fileCatalog->headChild;
     while(temp_son_catalog!=NULL){
       fprintf(stderr,"二级目录:%s　　文件名%s  :  ",temp_son_catalog->title,temp_son_catalog->file_name);
       temp_son_catalog = temp_son_catalog->brother;
     }
     fprintf(stderr,"\n\n");
     fileCatalog = fileCatalog->brother;
  }*/
  
  return fileCatalog;
 
}

/**
 *把file层数据结构转换为字符类型
 * @param srcCatalog 原file层目录结层
 * @param title 新增时的title 当title=NULL时为修改，不为NULL时为新增
 * @param saveSuccess 新增日志时返回的数据结构，修改时为NULL
 */
char *convert_file_to_str_catalog(const FILE_CATALOG *srcCatalog,const char *title,const SAVE_LOG_FILE_SUCCESS_RETURN_TYPE *saveSuccess){
   FILE_CATALOG *ff = srcCatalog;
   FILE_DETAIL_CATALOG *ss = NULL; //二级
   char *catalogFileStr = NULL;
   char *temp = NULL;
   int idAddCatalogStr = 0;//文件新增成功后,新添的目录索引是否加入到目录索引字符串，
   
   temp = (char *)malloc(sizeof(char)*2);
   memset(temp,0x00,sizeof(char)*2); 
   
   catalogFileStr = (char *)malloc(sizeof(char)*1);
   memset(catalogFileStr,0x00,sizeof(char)*1);
   
   while(ff!=NULL){
           //生成一级目录
           free(temp);
           temp = (char *)malloc(strlen(catalogFileStr)+strlen(ff->catalog_name)+4);
           memset(temp,0x00,strlen(catalogFileStr)+strlen(ff->catalog_name)+4);
           if(strlen(catalogFileStr)<1){
             strcpy(temp,"{");
             strcat(temp,catalogFileStr);
           }else{
             strcpy(temp,catalogFileStr);
             strcat(temp,"{");
           }
           strcat(temp,ff->catalog_name);
           strcat(temp,":[");
           
           free(catalogFileStr);
           catalogFileStr = (char *)malloc(strlen(temp)+1);
           memset(catalogFileStr,0x00,strlen(temp)+1);
           strcpy(catalogFileStr,temp);
           
           ss = ff->headChild;//取头子结点
           while(ss!=NULL){
              free(temp);
              temp = (char *)malloc(strlen(catalogFileStr)+strlen(ss->title)+2);
              memset(temp,0x00,strlen(catalogFileStr)+strlen(ss->title)+2);
              strcpy(temp,catalogFileStr);
              strcat(temp,ss->title);
              strcat(temp,"*");
              
              free(catalogFileStr);
              catalogFileStr = (char *)malloc(strlen(temp)+3+strlen(ss->file_name));
              memset(catalogFileStr,0x00,strlen(temp)+3+strlen(ss->file_name));
              
              strcpy(catalogFileStr,temp);
              strcat(catalogFileStr,ss->file_name);
              
              
              ss = ss->brother;//向兄弟结点移动
              if(ss==NULL){//二级目录完
                 //如果在二级目录结尾，且新增的在同一层，则添加更新
                 if(saveSuccess!=NULL){
                   if(strcmp(ff->catalog_name,saveSuccess->dir)==0 && title!=NULL){
                     free(temp);
                     temp = (char *)malloc(strlen(catalogFileStr)+strlen(title)+strlen(saveSuccess->file_name)+3);
                     memset(temp,0x00,strlen(catalogFileStr)+strlen(title)+strlen(saveSuccess->file_name)+3);
                     strcpy(temp,catalogFileStr);
                     strcat(temp,"=");
                     strcat(temp,title);
                     strcat(temp,"*");
                     strcat(temp,saveSuccess->file_name);
                     
                     
                     free(catalogFileStr);
                     catalogFileStr = (char *)malloc(strlen(temp)+3);
                     memset(catalogFileStr,0x00,strlen(temp)+3);
                     strcpy(catalogFileStr,temp);
                     idAddCatalogStr = 1;
                   }
                 }
                 strcat(catalogFileStr,"]}");
                 break;
              }
              strcat(catalogFileStr,"=");
           }
           
          ff = ff->brother;//向兄弟结点移动
          if(ff==NULL){
             if(!idAddCatalogStr && title!=NULL){//本次新增的日志为新的一个日期，另建一二级目录
                free(temp);
                temp = (char *)malloc(strlen(catalogFileStr)+strlen(saveSuccess->dir)+strlen(title)+strlen(saveSuccess->file_name)+8);
                memset(temp,0x00,strlen(catalogFileStr)+strlen(saveSuccess->dir)+strlen(saveSuccess->file_name)+8);
                strcpy(temp,catalogFileStr);
                strcat(temp,",{");
                strcat(temp,saveSuccess->dir);
                strcat(temp,":[");
                strcat(temp,title);
                strcat(temp,"*");
                strcat(temp,saveSuccess->file_name);
                strcat(temp,"]}");
                //[{2012-01:[我的文章1*liangtxt1=我的文章2*liangtxt2]},{2012-02:[我的文章3*liangtxt3=我的文章4*liangtxt4]}]
                
                free(catalogFileStr);
                catalogFileStr = (char *)malloc(strlen(temp)+1);
                memset(catalogFileStr,0x00,strlen(temp)+1);
                strcpy(catalogFileStr,temp);
             }
          
             free(temp);
             temp = (char *)malloc(strlen(catalogFileStr)+3);
             memset(temp,0x00,strlen(catalogFileStr)+3);
             strcpy(temp,"["); //开始
             strcat(temp,catalogFileStr);
             strcat(temp,"]");
             
             free(catalogFileStr);
             catalogFileStr = (char *)malloc(strlen(temp)+1);
             memset(catalogFileStr,0x00,strlen(temp)+1);
             strcpy(catalogFileStr,temp);
             
             break;
          }
          
          free(temp);
          temp = (char *)malloc(strlen(catalogFileStr)+2);
          memset(temp,0x00,strlen(catalogFileStr)+2);
          strcpy(temp,catalogFileStr);
          strcat(temp,",");
          
          free(catalogFileStr);
          catalogFileStr = (char *)malloc(strlen(temp)+1);
          memset(catalogFileStr,0x00,strlen(temp)+1);
          strcpy(catalogFileStr,temp);
       }
       free(temp);
       return catalogFileStr;
}

/**
 *读取并解析具体日志文件
 */
FILE_CONTENT *read_log_file(const char *filename){
   FILE_CONTENT *filecontent = NULL;
   //解读规则：因为为<标题>内容格式，所以，直接以>为结束符
   char *title = NULL;//标题
   char *content = NULL;//内容
   char *swap = NULL;//交换区
   char *readChar = NULL;//本方法以1字节方式读取
   int fd;//打开的文件描述符
   int isReadTitle = 1;//是否正在读标题 1读标题，０读内容
   
   fprintf(stderr,"正在读文件:%s\n",filename);
   
   
   title = (char *)malloc(sizeof(char));
   memset(title,0x00,sizeof(char));
   
   content = (char *)malloc(sizeof(char));
   memset(content,0x00,sizeof(char));
   
   swap = (char *)malloc(sizeof(char));
   memset(swap,0x00,sizeof(char));
   
   if((fd = open(filename,O_RDONLY))!=-1){ //打开文件成功
      readChar = (char *)malloc(2*sizeof(char));
      memset(readChar,0x00,2*sizeof(char));
      
      while(read(fd,readChar,1)>0){
         
         if(isReadTitle && readChar[0] == '>'){  //读完标题
           isReadTitle = 0;
         }else{
            if(isReadTitle){ //读标题
               if(strlen(title)==0 && readChar[0]=='<')
                  continue;
               
               free(swap);
	       swap = (char *)malloc(strlen(title)+2); 
	       memset(swap,0x00,strlen(title)+2);
	       strcpy(swap,title);
	       strcat(swap,readChar);
	       
	       free(title);
	       title = (char *)malloc(strlen(swap)+1);
	       memset(title,0x00,strlen(swap)+1);
	       strcpy(title,swap);
	       
            }else{  //读内容
               free(swap);
               swap = (char *)malloc(strlen(content)+2); 
	       memset(swap,0x00,strlen(content)+2);
	       strcpy(swap,content);
	       strcat(swap,readChar);
	       
	       free(content);
	       content = (char *)malloc(strlen(swap)+1);
	       memset(content,0x00,strlen(swap)+1);
	       strcpy(content,swap);
            }
         }
         
      }     
      
      fprintf(stderr,"终于读完了:%s  :  %s\n",title,content);
      
     filecontent = (FILE_CONTENT *)malloc(sizeof(FILE_CONTENT)); 
     
     filecontent->title = (char *)malloc(strlen(title)+1);
     memset(filecontent->title,0x00,strlen(title)+1);
     strcpy(filecontent->title,title);
     free(title);
     
     filecontent->content = (char *)malloc(strlen(content)+1);
     memset(filecontent->content,0x00,(strlen(content)+1));
     strcpy(filecontent->content,content);
     free(content);
     
     fprintf(stderr,"劲哥啊，读完具体文件了\n");
   
   }else{ 
     fprintf(stderr,"劲哥，日志文件读不了咯\n");
     perror("读日志文件");
   }
   
   return filecontent;
}

/**
 *@param type SAVE_NEW_LOG_FILE 0 //保存新建文件  SAVE_MODIFY_LOG_FILE 1 //保存修改文件
 *@param modifyFileName 要修改的文件名
 *@param title标题
 *@param content 内容
 *@return NULL:失败 SAVE_LOG_FILE_SUCCESS_RETURN_TYPE *:成功
 */
SAVE_LOG_FILE_SUCCESS_RETURN_TYPE *save_log_file(int type,char *title,char *content,char *modifyFileName){
  SAVE_LOG_FILE_SUCCESS_RETURN_TYPE *result = NULL;//保存、修改日志文件成功后返回的数据类型
  char *path = NULL;//最终路径
  char *dir = NULL;//文件夹
  char *temp = NULL;
  
  fprintf(stderr,"保存中＝》标题:%s\n内容:%s\n",title,content);
  if(type == SAVE_NEW_LOG_FILE){
    time_t timep; //当前时间秒数
    timep = time((time_t *)NULL);
    struct tm *t;
    t = gmtime(&timep);
    char *str = NULL;
    int fd;//文件描术符
    int tryTime = 0;  //重试写入－－》针对已存在文件，则重命名
    int isWriteSuccess = 0;//是否写成功
    
    dir = (char *)malloc(sizeof(char)*30);
    memset(dir,0x00,sizeof(char)*30);
    sprintf(dir,"%d-%d",(t->tm_year+1900),(t->tm_mon+1));
    
    path = (char *)malloc(50*sizeof(char));
    memset(path,0x00,50*sizeof(char));
    strcpy(path,"./data/");
    strcat(path,dir);
    strcat(path,"/");
    
    temp = (char *)malloc(sizeof(char)*30);
    memset(temp,0x00,sizeof(char)*30);
    sprintf(temp,"%ld",timep);
    strcat(path,temp);

    
    while(tryTime<10 && !isWriteSuccess){
       if(!check_dir_data(dir)){ //检查目录是否存在，没有则创建
           tryTime++;
           continue;
       }
        
       if((fd = creat(path,S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IWOTH|S_IXOTH))!=-1){ //创建文件成功
            int i = 0;
            write(fd,"<",1);//标题开始
            write(fd,title,strlen(title)); //定标题
            write(fd,">",1);//标题结束
            write(fd,content,strlen(content));//写内容
            if(!close(fd))
               isWriteSuccess = 1;
            else
              isWriteSuccess = 0;
           break;
       }else{ //创建文件失败，重命名
            free(temp);
            temp = (char *)malloc(sizeof(char)*30);
	    memset(temp,0x00,sizeof(char)*30);
	    sprintf(temp,"%d-%d-%d\n",(t->tm_year+1900),(t->tm_mon+1),t->tm_mday);
	    
	    free(path);
	    path = (char *)malloc(50*sizeof(char));
	    memset(path,0x00,50*sizeof(char));
	    strcpy(path,"./data/");
	    strcat(path,temp);

	    memset(temp,0x00,sizeof(char)*30);
	    sprintf(temp,"%ld",timep+1);
	    strcat(path,temp);
	    tryTime++;
       }
    
    }
    
    if(isWriteSuccess){
       result = (SAVE_LOG_FILE_SUCCESS_RETURN_TYPE *)malloc(sizeof(SAVE_LOG_FILE_SUCCESS_RETURN_TYPE));
       result->dir = (char *)malloc(strlen(dir)+1);
       memset(result->dir,0x00,strlen(dir)+1);
       strcpy(result->dir,dir);
       
       result->file_name = (char *)malloc(strlen(temp)+1);
       memset(result->file_name,0x00,strlen(temp)+1);
       strcpy(result->file_name,temp);
    }
    
    free(path);
    free(dir);
    free(temp);
    
  }else if(type == SAVE_MODIFY_LOG_FILE){  //修改文件
    int fd; //文件描述符
    if((fd = open(modifyFileName,O_WRONLY|O_TRUNC))!=-1){
       write(fd,"<",1);//标题开始
       write(fd,title,strlen(title));//标题
       write(fd,">",1);//标题结束
       write(fd,content,strlen(content));//内容
       
       if(!close(fd));
          result = (SAVE_LOG_FILE_SUCCESS_RETURN_TYPE *)malloc(sizeof(SAVE_LOG_FILE_SUCCESS_RETURN_TYPE)); //建一个空的，到时直接回收
    }
  }
  return result;
}

/**
 *保存、修改目录文件
 *@param str 内容
 *-1:失败 0:成功
 */
int save_or_modify_catalog_file(char *str){
   int tryTime = 0; //重试次数
   int isSuccess = -1;//是否成功
   int fd = -1;
   int i = 0;
   
   while(tryTime<10 && isSuccess==-1){
      if(check_dir()){ //文件夹data存在
        fd = open(CATALOG_FILE,O_WRONLY|O_CREAT|O_TRUNC);
        
        if(fd == -1){ //失败
          isSuccess = -1;
        }else{ //成功
          fprintf(stderr,"新增或修改目录:%s\n",str);
          /*for(i=0 ; i<strlen(str) ; i++){
            write(fd,str[i],1);
            fprintf(stderr,"%c",str[i]);
          }*/
          write(fd,str,strlen(str));
          isSuccess = close(fd);
        }
        
      }
      tryTime++;
   }
   
   return isSuccess;
}

/**
 *删除日志
 * @param fileName
 *@return -1失败
 */
int delete_log_file(const char *fileName){
   return unlink(fileName);
}

/**
 *删除第一层文件夹
 * @param dirName
 *@return -1失败
 */
int delete_dir(const char *dirName){
   return rmdir(dirName);
}


