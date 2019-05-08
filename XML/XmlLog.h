#include "./lib/tinyxml.h"
#include "../common/common.h"
#include "../common/mypacket.h"
// #include "../Server/server.h"
#include <iostream>
#include <cstdlib>
#include <map>
#include <sys/time.h>
#include <string>

class newXmlLog{
private:
    TiXmlDocument doc;
    TiXmlElement* xmlTotal;
    int id;
    string fileName ;
    timeval timePresent ;
    char timeBuf[32];
public:
    newXmlLog(const char * _fileName=NULL);
    ~newXmlLog();
    void writeLogin(const sockaddr_in & sockaddr , const char * name , const char * msg );
    void writeLogin(const ClientInfo & cinfo , const char * msg );
    void writeConnect (const ClientInfo & client , const ClientInfo &oppo_client , const char * msg );
    void writeGame(const ClientInfo & client , const ClientInfo & oppo_client , const Packet & packet );


};
