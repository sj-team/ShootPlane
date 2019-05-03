#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include<openssl/md5.h>
#include "sql.h"
using namespace std;
//登陆数据库的账号密码
#define USRNAME "u1651025"
#define PASSWD "u1651025"
#define HOSTNAME "localhost"
#define DBNAME "db1651025"

//写了大部分之后发现sprintf比strcat更好写，先不改

SERVER_MYSQL::SERVER_MYSQL(){
   if ((mysql = mysql_init(NULL))==NULL) {  //初始化错误
    	cout << "mysql_init failed" << endl;  
      exit(0);
   }

   if (mysql_real_connect(mysql,HOSTNAME,USRNAME, PASSWD,DBNAME,0, NULL, 0)==NULL) {//连接数据库，失败返回NULL
      cout << "mysql_real_connect failed(" << mysql_error(mysql) << ")" << endl;
      exit(0);
   }
   mysql_set_character_set(mysql, "gbk"); // 设置字符集，否则读出的字符乱码
}
SERVER_MYSQL::~SERVER_MYSQL(){
   mysql_close(mysql);
}



int SERVER_MYSQL::test_db(){
   //测试用
   MYSQL_RES *result;
   MYSQL_ROW  row;
   if (mysql_query(mysql, "select * from test")) {
      cout << "mysql_query failed(" << mysql_error(mysql) << ")" << endl;
      return -1;
   }

   /* 将查询结果存储起来，出现错误则返回NULL
      注意：查询结果为NULL，不会返回NULL */
   if ((result = mysql_store_result(mysql))==NULL) {
   cout << "mysql_store_result failed" << endl;
   return -1;
   }

   /* 当前查询到的记录的数量 */
   int res=(int)mysql_num_rows(result);

   /* 循环读取所有满足条件的记录
   1、返回的列顺序与select指定的列顺序相同，从row[0]开始
   2、不论数据库中是什么类型，C中都当作是字符串来进行处理，如果有必要，需要自己进行转换
   3、根据自己的需要组织输出格式 */
   while((row=mysql_fetch_row(result))!=NULL) {
      cout << row[0];
      cout << endl;
      }

   /* 释放result */
   mysql_free_result(result);
   return res;
}


int SERVER_MYSQL::check_user(const char usr[],const char passwd[]){
   //usr:用户名
   //passwd:密码
   //正确返回1，密码错误返回0 ，运行错误返回-1
   MYSQL_RES *result;
   MYSQL_ROW  row;
   char str[128]="select count(*) from userlist where id=\"";
   strcat(str,usr);
   strcat(str,"\" and passwd=MD5(\"");
   strcat(str,passwd);
   strcat(str,"\");");

   if (mysql_query(mysql, str)) {
      cout << "mysql_query failed(" << mysql_error(mysql) << ")" << endl;
      return -1;
   }

   if ((result = mysql_store_result(mysql))==NULL) {
      cout << "mysql_store_result failed" << endl;
      return -1;
   }
   row=mysql_fetch_row(result);
   int res = atoi(row[0]);

   mysql_free_result(result);
   return res;
}

int SERVER_MYSQL::create_user(const char usr[],const char passwd[]){
   //usr:用户名
   //passwd:密码
   //正确返回0，运行错误返回-1
   char str[128]="insert into userlist(id,passwd,log_time) values(\"";
   strcat(str,usr);
   strcat(str,"\",MD5(\"");
   strcat(str,passwd);
   strcat(str,"\"),\"1970-1-1 00:00:00\");");  //需要修改密码的时间
   if (mysql_query(mysql, str)) {
      cout << "mysql_query failed(" << mysql_error(mysql) << ")" << endl;
      return -1;
   }
   strcpy(str,"insert into settings values(\"");
   strcat(str,usr);
   strcat(str,"\",0,0,0);");
   if (mysql_query(mysql, str)) {
      cout << "mysql_query failed(" << mysql_error(mysql) << ")" << endl;
      return -1;
   }
   return 0;
}

int SERVER_MYSQL::need_set_passwd(const char usr[]){
   //判断是否需要设置密码，如果需要返回1，否则返回0
   MYSQL_RES *result;
   MYSQL_ROW  row;
   char str[128]="select count(*) from userlist where id=\"";
   strcat(str,usr);
   strcat(str,"\" and log_time=\"1970-1-1 00:00:00\";");

   if (mysql_query(mysql, str)) {
      cout << "mysql_query failed(" << mysql_error(mysql) << ")" << endl;
      return -1;
   }

   if ((result = mysql_store_result(mysql))==NULL) {
      cout << "mysql_store_result failed" << endl;
      return -1;
   }
   row=mysql_fetch_row(result);
   int res = atoi(row[0]);
   mysql_free_result(result);
   return res;
}

int SERVER_MYSQL::set_passwd(const char usr[],const char passwd[]){
   //设置密码，成功返回0，否则返回-1
   char str[128]="update userlist set passwd=MD5(\"";
   strcat(str,passwd);
   strcat(str,"\"),log_time=now() where id=\"");
   strcat(str,usr);
   strcat(str,"\";");
   if (mysql_query(mysql, str)) {
      cout << "mysql_query failed(" << mysql_error(mysql) << ")" << endl;
      return -1;
   }
   return 0;
}

int SERVER_MYSQL::get_userlist(vector<string> & ve){
   //查询所有用户，成功返回用户个数,字符串放到vector当中，否则返回-1
   ve.clear();
   MYSQL_RES *result;
   MYSQL_ROW  row;
   char str[128]="select id from userlist;";
   if (mysql_query(mysql, str)) {
      cout << "mysql_query failed(" << mysql_error(mysql) << ")" << endl;
      return -1;
   }

   if ((result = mysql_store_result(mysql))==NULL) {
      cout << "mysql_store_result failed" << endl;
      return -1;
   }
   int res=(int)mysql_num_rows(result);
   while((row=mysql_fetch_row(result))!=NULL) {
      ve.push_back(row[0]);
   }

   mysql_free_result(result);
   return res;
}

int SERVER_MYSQL::get_color(const char usr[]){
   //查询颜色，查询成功返回值，失败返回-1
   MYSQL_RES *result;
   MYSQL_ROW  row;
   char str[128]="select set_color from settings where id=\"";
   strcat(str,usr);
   strcat(str,"\";");
   if (mysql_query(mysql, str)) {
      cout << "mysql_query failed(" << mysql_error(mysql) << ")" << endl;
      return -1;
   }

   if ((result = mysql_store_result(mysql))==NULL) {
      cout << "mysql_store_result failed" << endl;
      return -1;
   }
   if((int)mysql_num_rows(result)>0){
      row=mysql_fetch_row(result);
      int res=atoi(row[0]);
      mysql_free_result(result);
      return res;
   }else{
      mysql_free_result(result);
      return -1;
   }

   return 0;
}
int SERVER_MYSQL::set_color(const char usr[],int settings){
   //设置颜色，正确返回颜色，错误返回-1
   char str[128]="update settings set set_color=";
   int len=strlen(str);
   sprintf(str+len,"%d",settings);
   strcat(str," where id=\"");
   strcat(str,usr);
   strcat(str,"\";");
   cout<<str<<endl;
   if (mysql_query(mysql, str)) {
      cout << "mysql_query failed(" << mysql_error(mysql) << ")" << endl;
      return -1;
   }
   return settings;
}

int SERVER_MYSQL::get_fontcolor(const char usr[]){
   //查询字体颜色，查询成功返回值，失败返回-1
   MYSQL_RES *result;
   MYSQL_ROW  row;
   char str[128]="select set_fontcolor from settings where id=\"";
   strcat(str,usr);
   strcat(str,"\";");
   if (mysql_query(mysql, str)) {
      cout << "mysql_query failed(" << mysql_error(mysql) << ")" << endl;
      return -1;
   }

   if ((result = mysql_store_result(mysql))==NULL) {
      cout << "mysql_store_result failed" << endl;
      return -1;
   }
   if((int)mysql_num_rows(result)>0){
      row=mysql_fetch_row(result);
      int res=atoi(row[0]);
      mysql_free_result(result);
      return res;
   }else{
      mysql_free_result(result);
      return -1;
   }

   return 0;
}
int SERVER_MYSQL::set_fontcolor(const char usr[],int settings){
   //设置字体颜色，正确返回颜色，错误返回-1
   char str[128]="update settings set set_fontcolor=";
   int len=strlen(str);
   sprintf(str+len,"%d",settings);
   strcat(str," where id=\"");
   strcat(str,usr);
   strcat(str,"\";");
   cout<<str<<endl;
   if (mysql_query(mysql, str)) {
      cout << "mysql_query failed(" << mysql_error(mysql) << ")" << endl;
      return -1;
   }
   return settings;
}

int SERVER_MYSQL::get_msgnum(const char usr[]){
   //查询字体颜色，查询成功返回值，失败返回-1
   MYSQL_RES *result;
   MYSQL_ROW  row;
   char str[128]="select set_msgnum from settings where id=\"";
   strcat(str,usr);
   strcat(str,"\";");
   if (mysql_query(mysql, str)) {
      cout << "mysql_query failed(" << mysql_error(mysql) << ")" << endl;
      return -1;
   }

   if ((result = mysql_store_result(mysql))==NULL) {
      cout << "mysql_store_result failed" << endl;
      return -1;
   }
   if((int)mysql_num_rows(result)>0){
      row=mysql_fetch_row(result);
      int res=atoi(row[0]);
      mysql_free_result(result);
      return res;
   }else{
      mysql_free_result(result);
      return -1;
   }

   return 0;
}
int SERVER_MYSQL::set_msgnum(const char usr[],int settings){
   //设置字体颜色，正确返回颜色，错误返回-1
   char str[128]="update settings set set_msgnum=";
   int len=strlen(str);
   sprintf(str+len,"%d",settings);
   strcat(str," where id=\"");
   strcat(str,usr);
   strcat(str,"\";");
   cout<<str<<endl;
   if (mysql_query(mysql, str)) {
      cout << "mysql_query failed(" << mysql_error(mysql) << ")" << endl;
      return -1;
   }
   return settings;
}

int SERVER_MYSQL::get_fontsize(const char usr[]){
   //查询字体颜色，查询成功返回值，失败返回-1
   MYSQL_RES *result;
   MYSQL_ROW  row;
   char str[128]="select set_fontsize from settings where id=\"";
   strcat(str,usr);
   strcat(str,"\";");
   if (mysql_query(mysql, str)) {
      cout << "mysql_query failed(" << mysql_error(mysql) << ")" << endl;
      return -1;
   }

   if ((result = mysql_store_result(mysql))==NULL) {
      cout << "mysql_store_result failed" << endl;
      return -1;
   }
   if((int)mysql_num_rows(result)>0){
      row=mysql_fetch_row(result);
      int res=atoi(row[0]);
      mysql_free_result(result);
      return res;
   }else{
      mysql_free_result(result);
      return -1;
   }

   return 0;
}
int SERVER_MYSQL::set_fontsize(const char usr[],int settings){
   //设置字体颜色，正确返回颜色，错误返回-1
   char str[128]="update settings set set_fontsize=";
   int len=strlen(str);
   sprintf(str+len,"%d",settings);
   strcat(str," where id=\"");
   strcat(str,usr);
   strcat(str,"\";");
   cout<<str<<endl;
   if (mysql_query(mysql, str)) {
      cout << "mysql_query failed(" << mysql_error(mysql) << ")" << endl;
      return -1;
   }
   return settings;
}


int SERVER_MYSQL::sql_update_msg(const char send_usr[],const char recv_usr[],const char msg[]){
//保存聊天记录,成功返回0，否则返回-1
   char str[512]="insert into msgs(from_id,to_id,msg_time,msg) values(\"";
   strcat(str,send_usr);
   strcat(str,"\",\"");
   strcat(str,recv_usr);
   strcat(str,"\",now(),\"");
   strcat(str,msg);
   strcat(str,"\");");  //需要修改密码的时间
   if (mysql_query(mysql, str)) {
      cout << "mysql_query failed(" << mysql_error(mysql) << ")" << endl;
      return -1;
   }

   return 0;
}
int SERVER_MYSQL::sql_update_broadcast(const char send_usr[],const char msg[]){
   //保存广播聊天记录,成功返回0，否则返回-1
   char str[512];
   sprintf(str,"insert into msgs(from_id,to_id,msg_time,msg) values(\"%s\",NULL,now(),\"%s\");");
   if (mysql_query(mysql, str)) {
      cout << "mysql_query failed(" << mysql_error(mysql) << ")" << endl;
      return -1;
   }
   return 0;
}
int SERVER_MYSQL::get_msglist(const char from_id[],const char to_id[],int num,std::vector<std::string> &ve){
   //查询发送方为from_id，接收方为to_id的最近num项，目前简单实现一个效果，待优化
   ve.clear();
   MYSQL_RES *result;
   MYSQL_ROW  row;
   char str[128];
   sprintf(str,"select from_id,msg from msgs where (from_id=\"%s\" and to_id=\"%s\") or (from_id=\"%s\" and to_id=\"%s\") order by msg_time asc limit %d;",
      from_id,to_id,to_id,from_id,num);
   if (mysql_query(mysql, str)) {
      cout << "mysql_query failed(" << mysql_error(mysql) << ")" << endl;
      return -1;
   }

   if ((result = mysql_store_result(mysql))==NULL) {
      cout << "mysql_store_result failed" << endl;
      return -1;
   }
   int res=(int)mysql_num_rows(result);
   string tmp;
   while((row=mysql_fetch_row(result))!=NULL) {
      tmp=row[0];
      if(tmp==from_id)
         tmp="1";
      else
         tmp="0";
      ve.push_back(tmp+row[1]);
   }
   mysql_free_result(result);
   return res;
}