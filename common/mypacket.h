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
#include <sys/prctl.h>//�޸Ľ�����
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

	const unsigned char winTheme = 0x01;
	const unsigned char friList = 0x02 ;
	const unsigned char hisNum = 0x03 ;
	const unsigned char fontColor = 0x04 ;
	const unsigned char fontSize = 0x05;
	const unsigned char hisMsg = 0x06 ;

	const unsigned char tellOnline = 0x01 ;
	const unsigned char tellOffline = 0x02 ;
	const unsigned char offlineSnd = 0x07 ;
	const unsigned char packetok = 0x08 ;

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

//  �����ļ����͵����� ����һ�����������ʽ  
struct fileHeader 
{
	char friName [MAXNAMELEN]; 
	char fileName [MAXNAMELEN];
	int  fileId ;
	int  packNum ;
};

//  file ����
struct fileData{
	char friName[MAXNAMELEN];
	int fileId ;
	int count ;
	char data[MAXDATALEN];
};


//  ��֤��¼
struct loginData{
	char username [MAXNAMELEN];
	char passwd [MAXPASSWDLEN];
};

struct changePwdData {
	char newPasswd [MAXPASSWDLEN];
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
    
    int gameId ; 
    bool ready ; 

    // turn == (id == index1)
    bool turn ; 
    // 1 Ϊ��ս��  2 Ϊ����ս�� Ϊclientinfo���±�
    int index1 , index2 ;

    ChessBoard *board1 , *board2 ;

    gameInfo(int id1 , int id2 , int gid){
        index1 = id1 ; 
        index2 = id2 ;
        gameId = gid ;
        ready = false ;
        turn = false;
        board1 = board2 = NULL ;
    }

};


//  �����Ǽ�����Ϣ���ĵĸ�ʽ ���Խ�msgǿ������ת�� �� fileData * datap = (fileData *) msg ;  

int socketSend (int cfd , const Packet & packet);

int socketRecv(int cfd , Packet & packet);

int getPacketLen(const Packet & packet);

unsigned char getMainType (const Packet & packet);

unsigned char getSubType (const Packet & packet );

int getFileSize(const char * filePath);

//   clinet   �������е��β�int cfd ����client���Լ���socket

//  client�˽������ݰ�  �����Լ���cfd ��  д���ݰ��ı���
int clientRecv(int cfd , Packet & packet );

//  client�� ���͵�¼����  �� �����û���������
int sndLogin(int cfd , const char * username , const char * passwd );

//  client�˷����ı���Ϣ�� idΪָ����ʽ���û�����  
//  �û�����������Ϊ "/name1/name2/name3/name4/end"  ������Ҫ���͸�name1-4
//  �� "/@all"   ������Ҫ���͸�������Ա
int sndText(int cfd , const char * id , const char * text );

//  client�˷���jpg  �� ����ָ��ͼƬ�ļ���λ��
int sndJPG (int cfd , const char * id , const char * jpgpath );

// client�˷���gif  �� �β�����ͬ��
int sndGIF(int cfd , const char * id , const char * gifpath);

//  ����˺��ѷ����ļ� �� �β�����ͬ��
int sndFile (int cfd , const char * id , const char * filepath);

// client�� �״ε�¼�ĸ�������  ����һ���µ�����
int firstChangePwd(int cfd , const char * newPwd );


int sndFileKind(int cfd , const char * id , unsigned char subType, const char * filepath);

// server 