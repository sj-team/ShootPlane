#pragma once 
#include <iostream>
#include <fstream>
#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/signal.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/file.h>//flock,lockf
#include <errno.h>
#include <signal.h>
#include <sys/prctl.h>//修改进程名
#include <queue>

#include "common.h"

using namespace std ;
#define MAXDATALEN 2048
#define MAXNAMELEN 32
#define MAXPASSWDLEN 32

#define HEADERLEN 4 

#define SNDALL "/@all"

namespace cstate 
{
	const char online = '1' ;
	const char offline = '0' ;
	const char playing = '2';

};

 namespace mt
 {
	 // client  send 
	const unsigned char login = 0x11;
	const unsigned char sndFileHead = 0x12 ;
	const unsigned char sndFile = 0x13;
	const unsigned char sndTxt = 0X14;

	const unsigned char conf = 0x21 ;
	const unsigned char updateList = 0x22 ;

	const unsigned char resLogin = 0x71;
	const unsigned char resSend = 0X72;
	const unsigned char resConf = 0X81 ;
	const unsigned char resFileHead =0x73;
 
	const unsigned char connect = 0x31;
	const unsigned char resConnect = 0x41;
	const unsigned char askGame = 0x51;
	const unsigned char init = 0x52 ;

	const unsigned char play = 0x32 ;
	const unsigned char resPlay = 0x42;

 };

namespace sbt
{
	const unsigned char myDefault = 0x00 ;

	const unsigned char request = 0x00 ;
	const unsigned char success = 0x01 ;
	const unsigned char changepwd = 0x02 ;

	const unsigned char groupChat = 0x00 ;
	const unsigned char file = 0x01;
	const unsigned char jpg = 0x02;
	const unsigned char gif = 0x03 ;

	const unsigned char failed = 0x00;
	const unsigned char pwderror = 0x03 ;
	const unsigned char repeatoff = 0x04 ;
	const unsigned char repeaton = 0x05 ;
	const unsigned char pwdChangeErr = 0x06;

	const unsigned char idNotExit = 0xfe;
	const unsigned char idOffline = 0xfd;
	const unsigned char idPlaying = 0xfc;

	const unsigned char winTheme = 0x01;
	const unsigned char friList = 0x02 ;
	const unsigned char hisNum = 0x03 ;
	const unsigned char fontColor = 0x04 ;
	const unsigned char fontSize = 0x05;
	const unsigned char hisMsg = 0x06 ;

	const unsigned char tellOnline = 0x01 ;
	const unsigned char tellOffline = 0x00 ;
	const unsigned char tellPlaying = 0x02 ;
	const unsigned char tellEnd = 0x03 ;


	const unsigned char offlineSnd = 0x07 ;
	const unsigned char packetok = 0x08 ;

	// mt::connect || mt::resConnect || mt::askInit
 	//const unsigned char request 
	const unsigned char accept = 0x02 ;
	const unsigned char deny = 0x03 ;
	const unsigned char locate = 0x01;
	const unsigned char wait = 0x00 ;
	const unsigned char turn = 0x02 ;
	const unsigned char relocate = 0x10 ;
	

	const unsigned char unmask = 0x02;
	const unsigned char win = 0x03 ;
	const unsigned char lose = 0x04;


};




struct packetHeader{
	unsigned char mainType ;
	unsigned char subType ;
	unsigned short length;
};

struct TxtData
{
	char friName [MAXNAMELEN ];
	char txtMsg [MAXDATALEN];
};

//  传输文件类型的数据 ，第一个包是这个格式  
struct fileHeader 
{
	char friName [MAXNAMELEN]; 
	char fileName [MAXNAMELEN];
	int  fileId ;
	int  packNum ;
};

//  file 类型
struct fileData{
	char friName[MAXNAMELEN];
	int fileId ;
	int count ;
	char data[MAXDATALEN];
};


//  认证登录
struct loginData{
	char username [MAXNAMELEN];
	char passwd [MAXPASSWDLEN];
};

struct changePwdData {
	char newPasswd [MAXPASSWDLEN];
};

// locate 
struct locateData {
	unsigned char p1_x1 , p1_y1 , p1_x2 , p1_y2 ;
	unsigned char p2_x1 , p2_y1 , p2_x2 , p2_y2 ;
	unsigned char p3_x1 , p3_y1 , p3_x2 , p3_y2 ;
};



struct unmaskPointData{
	unsigned char x ;
	unsigned char y ;
};

struct unmaskLocateData{
	unsigned char x1 , y1 ;
	unsigned char x2 , y2 ;
};

struct unmaskPointResult{
	unsigned char x ;
	unsigned char y ;
	unsigned char result ;
};

struct unmaskLocateResult{
	unsigned char x1 , y1 ;
	unsigned char x2 , y2 ;
	unsigned char result ;
};




int fillPacketHeader(packetHeader & header , unsigned char mainType , unsigned char resType , unsigned short msgLen);


struct Packet{
	packetHeader header ;
	char msg[MAXDATALEN+1024];

	bool isMainType( unsigned char maintp)
	{
		return header.mainType == maintp ;
	}

	bool isSubType (unsigned char subtp)
	{
		return header.subType == subtp ;
	}

	bool isType(unsigned char maintp , unsigned char subtp )
	{
		return header.mainType==maintp && header.subType == subtp ;
	}

	int getLen()
	{
		return ntohs(header.length);
	}

	int fillPacket (unsigned char maintp , unsigned char subtp , const void * data , int len )
	{
		fillPacketHeader(header,maintp , subtp , len );
		memcpy (msg , data , len );
		return 0 ;
	}

	bool isGroupSnd()
	{
		if(isMainType(mt::sndTxt)&& (header.subType >1 || strcmp(msg,SNDALL)==0))
			return true ;
		return false ;
	}



};

struct ClientInfo{
    int cfd;
    string name;
    unsigned char status ;
    int gameId ; 
    sockaddr_in sockaddr ; 
    ChessBoard *my_board , *oppo_board ; 


    vector<Packet> offlinePacks ;
    ClientInfo(string _name , int _cfd = -1){
        name = _name ;
        cfd = _cfd;
        gameId = -1;
        my_board = oppo_board = NULL ;
        status = cstate::offline ;
        offlinePacks.clear();

    }

};

struct loginAction 
{
    int cfd ; 
    int index ; 
    string username ;
    sockaddr_in sockaddr ; 
    unsigned char state ;

    loginAction ( int _cfd , const sockaddr_in & _sockaddr )
    {
        cfd = _cfd;
        sockaddr = _sockaddr ;
        state = sbt::request;

    }
};

struct gameInfo {
    
//    int gameId ; 
    // 表示双方已经进入正常游戏界面
	// 已经完成了对局的建立和飞机的摆放
	bool ready ; 

	// turn is true , then index1's turn 
    bool turn ; 
    // 1 为挑战者  2 为被挑战者 为clientinfo的下标
    int index1 , index2 ;

    ChessBoard *board1 , *board2 ;

    gameInfo(int id1 , int id2 ){
        index1 = id1 ; 
        index2 = id2 ;
 //       gameId = gid ;
        ready = false ;
        turn = false;
        board1 = board2 = NULL ;
    }

};


//  以下是几种消息报文的格式 可以将msg强制类型转换 如 fileData * datap = (fileData *) msg ;  

int socketSend (int cfd , const Packet & packet);

int socketRecv(int cfd , Packet & packet);

int getPacketLen(const Packet & packet);

unsigned char getMainType (const Packet & packet);

unsigned char getSubType (const Packet & packet );

int getFileSize(const char * filePath);

//   clinet   以下所有的形参int cfd 都是client端自己的socket

//  client端接收数据包  输入自己的cfd 和  写数据包的变量
int clientRecv(int cfd , Packet & packet );

//  client端 发送登录请求  ， 输入用户名和密码
int sndLogin(int cfd , const char * username , const char * passwd );

//  client端发送文本消息， id为指定格式的用户名串  
//  用户名串的类型为 "/name1/name2/name3/name4/end"  代表想要发送给name1-4
//  或 "/@all"   代表想要发送给所有组员
int sndText(int cfd , const char * id , const char * text );

//  client端发送jpg  ， 传入指向图片文件的位置
int sndJPG (int cfd , const char * id , const char * jpgpath );

// client端发送gif  ， 形参意义同上
int sndGIF(int cfd , const char * id , const char * gifpath);

//  向个人好友发送文件 ， 形参意义同上
int sndFile (int cfd , const char * id , const char * filepath);

// client端 首次登录的更改密码  传入一个新的密码
int firstChangePwd(int cfd , const char * newPwd );


int sndFileKind(int cfd , const char * id , unsigned char subType, const char * filepath);

// server 