#include "socketmanager.h"
#include "common/mypacket.h"
#ifdef  WIN32
//#include <winsock2.h>
//#pragma comment(lib, "ws2_32")
unsigned short htons(unsigned short data){
   unsigned short ans;
   char *p=(char*)(&data);
   char *q=(char*)(&ans);
   q[0]=p[1];
   q[1]=p[0];
   return ans;
}
unsigned short ntohs(unsigned short data){
   unsigned short ans;
   char *p=(char*)(&data);
   char *q=(char*)(&ans);
   q[0]=p[1];
   q[1]=p[0];
   return ans;
}
#endif
socketManager::socketManager(QObject *parent):QObject(parent){
    clientSocket=new QTcpSocket();
    topLogin=new Login;
    myFriendList=nullptr;
    myChangePwd=nullptr;
    startRequestWaiting=new QMessageBox(QMessageBox::Icon::Information,"waiting","等待对方回应...",QMessageBox::StandardButton::Cancel);

}
socketManager::~socketManager()
{
    if(topLogin!=nullptr){
        delete topLogin;
    }
    if(myFriendList!=nullptr){
        delete myFriendList;
    }
    if(myChangePwd!=nullptr){
        delete myChangePwd;
    }
    if(startRequestWaiting!=nullptr){
        delete startRequestWaiting;
    }
    clientSocket->disconnect();
    delete clientSocket;
}
void socketManager::chgpasswd(){

    myChangePwd=new ChangePwd(topLogin);
    myChangePwd->setUserName(topLogin->getUserName());
    myChangePwd->setOldPwd(topLogin->getPwd());
    myChangePwd->show();
}
void socketManager::closeall(){
    //topLogin->hide();

}
void socketManager::login_success(){
    if(myChangePwd!=nullptr){
        myChangePwd->close();
    }
    packetHeader successHeader;
    fillPacketHeader(successHeader,mt::login,sbt::success,0);
    send_data(&successHeader,sizeof successHeader);
    topLogin->close();
    myFriendList = new FriendList;
    myFriendList->show();
}
void socketManager::send_data(void *data, int len){
    Packet* mp=(Packet*)data;
    qDebug()<<int(mp->header.mainType)<<"   "<<int(mp->header.subType)<<" "<<(mp->msg)<<" "<<(mp->msg+32);
    if(!clientSocket->isOpen()){
        clientSocket->connectToHost(QHostAddress("10.60.102.252"), 20202);
        connect(clientSocket, SIGNAL(readyRead()), this, SLOT(ClientRecvData()));
        connect(clientSocket, SIGNAL(disconnected()), this, SLOT(ClientDisconnected()));
    }
     clientSocket->write((char*)data,len);
}

void socketManager::solve_friendlist(QString fliststr){  //加载列表
    if(myFriendList==nullptr)return;
    auto flist=fliststr.split("|");
    for(auto i:flist){
        if(i.length()<=1)continue;

        QString name=i.mid(1);
        qDebug()<<"update friendlist:"<<name<<" "<<i[0];
        int stat=FriendList::friendData::stat_offline;
        if(i[0]==cstate::online)
            stat=FriendList::friendData::stat_online;
        else if(i[0]==cstate::gaming)
            stat=FriendList::friendData::stat_gaming;
        else if(i[0]==cstate::offline)
            stat=FriendList::friendData::stat_offline;
        myFriendList->addFriend(name,stat);
    }
}
void socketManager::solve_updfriendlist(Packet *p){  //更新列表
    if(myFriendList==nullptr)return;
    int stat=FriendList::friendData::stat_offline;
    if(p->header.subType==sbt::tellOnline)
        stat=FriendList::friendData::stat_online;
    else if(p->header.subType==sbt::tellEnd)
        stat=FriendList::friendData::stat_online;
    else if(p->header.subType==sbt::tellPlaying)
        stat=FriendList::friendData::stat_gaming;
    else if(p->header.subType==sbt::tellOffline)
        stat=FriendList::friendData::stat_offline;
    myFriendList->addFriend(p->msg,stat);

}
void socketManager::solve_connect_failure(){
    if(startRequestWaiting){
        startRequestWaiting->close();
        startRequestWaiting->setResult(1);
    }
}

void socketManager::solve_connect_success(){
    if(startRequestWaiting){
        startRequestWaiting->close();
        startRequestWaiting->setResult(0);
    }
}

void socketManager::solve_packet(Packet *myPacket){
    qDebug()<<int(myPacket->header.mainType)<<"   "<<int(myPacket->header.subType);
    switch(myPacket->header.mainType){
        case mt::resLogin:{ //server回应登录
            switch(myPacket->header.subType){
                case sbt::failed:
                    break;
                case sbt::changepwd:
                    chgpasswd();
                    break;
                case sbt::pwderror:
                    QMessageBox::warning(nullptr,"warning","密码错误");
                    break;
                case sbt::repeaton:
                    //break;
                    ;
                case sbt::success:
                    login_success();
                    break;
                case sbt::repeatoff:
                    break;
                default:
                    break;
            }
            break;
        }
        case mt::resConf:{
            switch(myPacket->header.subType){
                case sbt::friList:
                    solve_friendlist(myPacket->msg);
                    break;
                default:
                    break;
            }
            break;
        }
        case mt::updateList:{
            solve_updfriendlist(myPacket);
            break;
        }
        case mt::connect:{
            switch(myPacket->header.subType){
                case sbt::request:
                    solve_recv_newgame(myPacket->msg);
                    break;
                case sbt::success: //暂时不会产生,只会发送
                    //solve_friendlist(myPacket->msg);
                    break;
                default:
                    break;
            }
            break;
        }
        case mt::resConnect:{
            switch(myPacket->header.subType){
                case sbt::accept:
                    solve_connect_success();
                    break;
                case sbt::deny:
                    solve_connect_failure();
                    QMessageBox::warning(nullptr,"warning","对方拒绝了对战");
                    break;
                default:
                    break;
            }
            break;
        }
        case mt::resSend:{
            switch(myPacket->header.subType){
                case sbt::idNotExit:
                    solve_connect_failure();
                    QMessageBox::warning(nullptr,"warning","ID不存在");
                    break;
                case sbt::idOffline:
                    solve_connect_failure();
                    QMessageBox::warning(nullptr,"warning","对方已下线");
                    break;
                case sbt::idPlaying:
                    solve_connect_failure();
                    QMessageBox::warning(nullptr,"warning","对方已开始游戏");
                    break;
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }
}
void socketManager::ClientRecvData(){
    qDebug() << "receive data";
    static QByteArray current_byte_array;
    QByteArray temp_array;
    static Packet recv_packet;
    current_byte_array += clientSocket->readAll();
    static int next_readbyte=HEADERLEN;
    static int status=0;
    while(current_byte_array.length()>=next_readbyte){
        if(status==0){
            temp_array = current_byte_array.left(next_readbyte);
            current_byte_array.remove(0, next_readbyte);
            memcpy(&recv_packet.header,temp_array.data(),next_readbyte);
            status=1;
            next_readbyte=recv_packet.getLen()-HEADERLEN;
        }else if(status==1){
            temp_array = current_byte_array.left(next_readbyte);
            current_byte_array.remove(0, next_readbyte);
            memcpy(&recv_packet.msg,temp_array.data(),next_readbyte);
            status=0;
            next_readbyte=HEADERLEN;
            solve_packet(&recv_packet);
        }

    }
}
void socketManager::ClientDisconnected(){
    if(clientSocket->isOpen())
        clientSocket->close();

}


void socketManager::solve_recv_newgame(char *name){
    int res=QMessageBox::information(nullptr,"new game",QString(name)+" 向你发起对战，是否接受？",QMessageBox::StandardButton::Yes,QMessageBox::StandardButton::No);
    Packet p;
    strcpy(p.msg,name);
    if(res==QMessageBox::Yes){//接受
        fillPacketHeader(p.header,mt::resConnect,sbt::accept,MAXNAMELEN);
    }else{//拒绝
        fillPacketHeader(p.header,mt::resConnect,sbt::deny,MAXNAMELEN);
    }
    send_data(&p,MAXNAMELEN+HEADERLEN);
}

int fillPacketHeader(packetHeader & header , unsigned char mainType , unsigned char resType , unsigned short msgLen)
{
    header.mainType = mainType ;
    header.subType = resType;
    header.length = htons(msgLen+HEADERLEN);
    return 0;
}
