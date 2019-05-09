#pragma once
//mysql_demo.cpp

#include <mysql.h>	// mysqlÃÿ”–
#include <string>
#include <vector>
void MYMD5(char data[],int data_len,char hash_res[]);

struct SERVER_MYSQL{
private:
MYSQL     *mysql; 

public:

SERVER_MYSQL();
//void sql_connect();
//void sql_reconnect();
~SERVER_MYSQL();

//int sql_setup(){
//   return 0;
//}


int check_user(const char usr[],const char passwd[]);

int create_user(const char usr[],const char passwd[]);

int need_set_passwd(const char usr[]);

int set_passwd(const char usr[],const char passwd[]);

int get_userlist(std::vector<std::string> &ve);

int get_color(const char usr[]);
int set_color(const char usr[],int settings);

int get_fontcolor(const char usr[]);
int set_fontcolor(const char usr[],int settings);

int get_msgnum(const char usr[]);
int set_msgnum(const char usr[],int settings);

int get_fontsize(const char usr[]);
int set_fontsize(const char usr[],int settings);

int sql_update_msg(const char send_usr[],const char recv_usr[],const char msg[]);
int sql_update_broadcast(const char send_usr[],const char msg[]);
int get_msglist(const char from_id[],const char to_id[],int num,std::vector<std::string> &ve);
int test_db();
};