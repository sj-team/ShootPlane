#ifndef MYPACHET_H
#define MYPACHET_H
#include <iostream>
#include <fstream>
#include <time.h>

#include <stdio.h>


#include <stdlib.h>
#include <string.h>



#ifdef  WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32")
//unsigned short htons(unsigned short data);
//unsigned short ntohs(unsigned short data);
#else
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
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
#include <sys/prctl.h>
#endif

using namespace std ;
#define MAXDATALEN 2048
#define MAXNAMELEN 32
#define MAXPASSWDLEN 32

#define HEADERLEN 4

#define SNDALL "/@all"
#define MAXBEATCTR 5
#define BEATMSEC 1000
namespace cstate
{
    const char gaming = '2';
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

    const unsigned char connect = 0x31 ;
    const unsigned char play = 0x32;
    const unsigned char resConnect = 0x41 ;
    const unsigned char resPlay = 0x42 ;


    const unsigned char askGame = 0x51 ;
    const unsigned char init = 0x52;

    const unsigned char resLogin = 0x71;
    const unsigned char resSend = 0X72;
    const unsigned char resConf = 0X81 ;
    const unsigned char resFileHead = 0x73;
    const unsigned char beat = 0xff;
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
    const unsigned char tellPlaying = 0x02;
    const unsigned char tellEnd = 0x03;

    const unsigned char offlineSnd = 0x07 ;
    const unsigned char packetok = 0x08 ;

    const unsigned char accept = 0x02 ;
    const unsigned char deny = 0x03 ;
    const unsigned char locate = 0x01 ;
    const unsigned char turn = 0x02 ;
    const unsigned char wait = 0x00 ;

    const unsigned char surrender = 0xf0;
    //const unsigned char surrender = 0x10;
    const unsigned char unmask = 0x02;

    const unsigned char win = 0x03;
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
struct locateData{
    unsigned char p1_x1,p1_y1,p1_x2,p1_y2;
    unsigned char p2_x1,p2_y1,p2_x2,p2_y2;
    unsigned char p3_x1,p3_y1,p3_x2,p3_y2;
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
    unsigned char score;

};


struct unmaskLocateResult{
    unsigned char x1 , y1 ;
    unsigned char x2 , y2 ;
    unsigned char result ;
    unsigned char score;
};


#endif
