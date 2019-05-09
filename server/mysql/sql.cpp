#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include<openssl/md5.h>
#include "sql.h"
using namespace std;
//��½���ݿ���˺�����
#define USRNAME "u1651025"
#define PASSWD "u1651025"
#define HOSTNAME "localhost"
#define DBNAME "db1651025"

//д�˴󲿷�֮����sprintf��strcat����д���Ȳ���
SERVER_MYSQL::SERVER_MYSQL(){
   if ((mysql = mysql_init(NULL))==NULL) {  //��ʼ������
    	cout << "mysql_init failed" << endl;  
      exit(0);
   }

   if (mysql_real_connect(mysql,HOSTNAME,USRNAME, PASSWD,DBNAME,0, NULL, 0)==NULL) {//�������ݿ⣬ʧ�ܷ���NULL
      cout << "mysql_real_connect failed(" << mysql_error(mysql) << ")" << endl;
      exit(0);
   }
   mysql_set_character_set(mysql, "gbk"); // �����ַ���������������ַ�����
}
SERVER_MYSQL::~SERVER_MYSQL(){
   mysql_close(mysql);
}



int SERVER_MYSQL::test_db(){
   //������
   MYSQL_RES *result;
   MYSQL_ROW  row;
   if (mysql_query(mysql, "select * from test")) {
      cout << "mysql_query failed(" << mysql_error(mysql) << ")" << endl;
      return -1;
   }

   /* ����ѯ����洢���������ִ����򷵻�NULL
      ע�⣺��ѯ���ΪNULL�����᷵��NULL */
   if ((result = mysql_store_result(mysql))==NULL) {
   cout << "mysql_store_result failed" << endl;
   return -1;
   }

   /* ��ǰ��ѯ���ļ�¼������ */
   int res=(int)mysql_num_rows(result);

   /* ѭ����ȡ�������������ļ�¼
   1�����ص���˳����selectָ������˳����ͬ����row[0]��ʼ
   2���������ݿ�����ʲô���ͣ�C�ж��������ַ��������д�������б�Ҫ����Ҫ�Լ�����ת��
   3�������Լ�����Ҫ��֯�����ʽ */
   while((row=mysql_fetch_row(result))!=NULL) {
      cout << row[0];
      cout << endl;
      }

   /* �ͷ�result */
   mysql_free_result(result);
   return res;
}


int SERVER_MYSQL::check_user(const char usr[],const char passwd[]){
   //usr:�û���
   //passwd:����
   //��ȷ����1��������󷵻�0 �����д��󷵻�-1
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
   //usr:�û���
   //passwd:����
   //��ȷ����0�����д��󷵻�-1
   char str[128]="insert into userlist(id,passwd,log_time) values(\"";
   strcat(str,usr);
   strcat(str,"\",MD5(\"");
   strcat(str,passwd);
   strcat(str,"\"),\"1970-1-1 00:00:00\");");  //��Ҫ�޸������ʱ��
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
   //�ж��Ƿ���Ҫ�������룬�����Ҫ����1�����򷵻�0
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
   //�������룬�ɹ�����0�����򷵻�-1
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
   //��ѯ�����û����ɹ������û�����,�ַ����ŵ�vector���У����򷵻�-1
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
   //��ѯ��ɫ����ѯ�ɹ�����ֵ��ʧ�ܷ���-1
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
   //������ɫ����ȷ������ɫ�����󷵻�-1
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
   //��ѯ������ɫ����ѯ�ɹ�����ֵ��ʧ�ܷ���-1
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
   //����������ɫ����ȷ������ɫ�����󷵻�-1
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
   //��ѯ������ɫ����ѯ�ɹ�����ֵ��ʧ�ܷ���-1
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
   //����������ɫ����ȷ������ɫ�����󷵻�-1
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
   //��ѯ������ɫ����ѯ�ɹ�����ֵ��ʧ�ܷ���-1
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
   //����������ɫ����ȷ������ɫ�����󷵻�-1
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
//���������¼,�ɹ�����0�����򷵻�-1
   char str[512]="insert into msgs(from_id,to_id,msg_time,msg) values(\"";
   strcat(str,send_usr);
   strcat(str,"\",\"");
   strcat(str,recv_usr);
   strcat(str,"\",now(),\"");
   strcat(str,msg);
   strcat(str,"\");");  //��Ҫ�޸������ʱ��
   if (mysql_query(mysql, str)) {
      cout << "mysql_query failed(" << mysql_error(mysql) << ")" << endl;
      return -1;
   }

   return 0;
}
int SERVER_MYSQL::sql_update_broadcast(const char send_usr[],const char msg[]){
   //����㲥�����¼,�ɹ�����0�����򷵻�-1
   char str[512];
   sprintf(str,"insert into msgs(from_id,to_id,msg_time,msg) values(\"%s\",NULL,now(),\"%s\");");
   if (mysql_query(mysql, str)) {
      cout << "mysql_query failed(" << mysql_error(mysql) << ")" << endl;
      return -1;
   }
   return 0;
}
int SERVER_MYSQL::get_msglist(const char from_id[],const char to_id[],int num,std::vector<std::string> &ve){
   //��ѯ���ͷ�Ϊfrom_id�����շ�Ϊto_id�����num�Ŀǰ��ʵ��һ��Ч�������Ż�
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