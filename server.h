#include <fstream>
#include <string>
#include <sys/prctl.h>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <sys/prctl.h>
#include "common/common.h"
#include "mysql/sql.h"
#include "common/mypacket.h"
#include "XML/XmlLog.h"

#define MAX_CONNECT 1000
#define MAX_LISTEN 1000
const bool flag_block = true;

using namespace std ;



class Server 
{
private :

    int max_fd, server_fd;
    fd_set read_fds, write_fds;

    int senderIndex ;

    vector<ClientInfo> clientList;

    vector<loginAction> loginList;

    vector<gameInfo> gameList ;

    // name �� clientList �±��map
    map<string, int> nameIndex;
    
    // cfd �� clientList  �±��map
    map<int, int> cfdIndex;

    SERVER_MYSQL * dataBase;

    XmlLog  mylog ; 


    bool newConnect ();

    void solveLogin(int index);

    void removeLogin (vector<loginAction>::iterator i);

    void tell_clinet_onoffline (int index ,bool isOnline);

    void close_cfd(int cfd );

    void add_cfd (int cfd );

    void initClientSetup();

    //server ����packet
    int serverRecv(int cfd , Packet & packet);

    //server �������ݣ�ת����Ϣ��
    int serverSend (int cfd ,const Packet & packet );

    //server ���ͻ�Ӧ��ֻ�б�ͷ��Ϣ��û��msg��
    int sndResponse(int cfd , unsigned char maintype ,unsigned char subtype , const char * name =NULL );

    //   ���յ��� txt �� ��������Ϸ����ߵ��û�����
    int alterPack( Packet &  desPack , Packet & srcPack , const char * srcId );

    int alterTxtPack(Packet &desPack, Packet &srcPack, const char *srcId);

    int alterFileHeaderPack(Packet &desPack, Packet &srcPack, const char *srcId);

    int alterFileDataPack(Packet &desPack, Packet &srcPack, const char *srcId);

    void solveMsg(const int index );

    int sndOneMsg(int index ,const char * rcvName , const Packet & packet );

    void user_online ( int index );

    void user_offline( int index );

    void user_leave( int index );

    void removeGame( int gid );

    void endGame( int gid , bool turn);

    // tell user wait or play
    void gameTurn(int gid );

  public:

    Server();

    void initClinetSetup();

    void serverInit();

    void run();

    ~Server();


    
};

