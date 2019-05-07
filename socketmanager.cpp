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
    myCreateGame=nullptr;
    myGameGui=nullptr;
    startRequestWaiting=new QMessageBox(QMessageBox::Icon::Information,"waiting","等待对方回应...",QMessageBox::StandardButton::Cancel);
    hintMessage_s=nullptr;
    connect(this, SIGNAL(signal_surrender()), this, SLOT(slot_surrender()),Qt::QueuedConnection);
    connect(this, SIGNAL(signal_win()), this, SLOT(slot_win()),Qt::QueuedConnection);
    connect(this, SIGNAL(signal_lose()), this, SLOT(slot_lose()),Qt::QueuedConnection);
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
    if(hintMessage_s!=nullptr){
        delete hintMessage_s;
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
    name=topLogin->getUserName();
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
    qDebug()<<"connect success";
    if(startRequestWaiting){
        startRequestWaiting->close();
        startRequestWaiting->setResult(0);

    }
}
void socketManager::end_game(){
    if(myCreateGame!=nullptr){
        myCreateGame->close();
        myCreateGame=nullptr;
    }
    if(myGameGui!=nullptr){
        myGameGui->close();
        myGameGui=nullptr;
    }
    myFriendList->setEnabled(true);
}
void socketManager::slot_surrender(){
    qDebug()<<"对方退出游戏";
    QMessageBox::warning(nullptr,"warining","对方退出了游戏！");
    end_game();
}
void socketManager::slot_win(){
    qDebug()<<"胜利";
    QMessageBox::warning(nullptr,"warining","胜利！");
    end_game();
}
void socketManager::slot_lose(){
    qDebug()<<"胜利";
    QMessageBox::warning(nullptr,"warining","失败！");
    end_game();
}


void socketManager::solve_start_locate(){
    myFriendList->setEnabled(false);
    if(myCreateGame==nullptr){
        myCreateGame=new CreateGame;
        game_name=name + " VS " + op_name;
        myCreateGame->setTitle(game_name);
        myCreateGame->show();
    }

}
void socketManager::return_friendlist(){//返回列表界面
    myFriendList->setEnabled(true);
}
void socketManager::solve_play_locate(Packet *p){
    unmaskLocateResult *resPtr=(unmaskLocateResult *)p->msg;
    if(resPtr->result){
        myGameGui->setPlane(0,resPtr->x1,resPtr->y1,resPtr->x2,resPtr->y2);
    }
}
void socketManager::solve_resPlay_locate(Packet *p){
    unmaskLocateResult *resPtr=(unmaskLocateResult *)p->msg;
    if(resPtr->result){
        myGameGui->setPlane(1,resPtr->x1,resPtr->y1,resPtr->x2,resPtr->y2);
    }
}
void socketManager::solve_play_unmask(Packet *p){
    unmaskPointResult *resPtr=(unmaskPointResult *)p->msg;
    myGameGui->setPoint(0,resPtr->x,resPtr->y,resPtr->result);
}
void socketManager::solve_resPlay_unmask(Packet *p){
    unmaskPointResult *resPtr=(unmaskPointResult *)p->msg;
    myGameGui->setPoint(1,resPtr->x,resPtr->y,resPtr->result);
}
void socketManager::solve_packet(Packet *myPacket){
    qDebug()<<int(myPacket->header.mainType)<<"   "<<int(myPacket->header.subType);
    QMessageBox *hintMessage=nullptr;
    switch(myPacket->header.mainType){
        case mt::resLogin:{ //server回应登录
            switch(myPacket->header.subType){
                case sbt::failed:
                    break;
                case sbt::changepwd:
                    chgpasswd();
                    break;
                case sbt::pwderror:
                    hintMessage= new QMessageBox(QMessageBox::Icon::Warning,"warning","密码错误");
                    break;
                case sbt::repeaton:
                    hintMessage= new QMessageBox(QMessageBox::Icon::Warning,"warning","重复上线");
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
                    qDebug()<<"sssssss";
                    emit this->myFriendList->signal_new_game(myPacket->msg);
                    break;
                case sbt::success: //暂时不会产生,只会发送
                    //solve_friendlist(myPacket->msg);
                    break;
                case sbt::deny:
                    qDebug()<<"deny to recv";
                    emit this->myFriendList->signal_stop_connect();
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
                    qDebug()<<"deny to sender";
                    hintMessage= new QMessageBox(QMessageBox::Icon::Warning,"warning","对方拒绝了对战");
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
                    hintMessage= new QMessageBox(QMessageBox::Icon::Warning,"warning","ID不存在");
                    break;
                case sbt::idOffline:
                    solve_connect_failure();
                    hintMessage= new QMessageBox(QMessageBox::Icon::Warning,"warning","对方已下线");
                    break;
                case sbt::idPlaying:
                    solve_connect_failure();
                    hintMessage= new QMessageBox(QMessageBox::Icon::Warning,"warning","对方已开始游戏");
                    break;
                default:
                    break;
            }
            break;
        }
        case mt::askGame:{
            switch(myPacket->header.subType){
                case sbt::locate:
                    solve_start_locate();
                    break;
                case sbt::turn:
                    myGameGui->setStatus("你的回合");
                    break;
                case sbt::wait:
                    myGameGui->setStatus("对方回合");
                    break;
                default:
                    break;
            }
            break;
        }
        case mt::play:{
            switch(myPacket->header.subType){
                case sbt::locate:
                    solve_play_locate(myPacket);
                    break;
                case sbt::unmask:
                    solve_play_unmask(myPacket);
                    break;
                default:
                    break;
            }
            break;
        }
        case mt::resPlay:{
            switch(myPacket->header.subType){
                case sbt::win:
                    emit signal_win();
                    break;
                case sbt::surrender:
                    qDebug()<<"recv surrender";
                    emit signal_surrender();
                    break;
                case sbt::lose:
                    emit signal_lose();
                    break;
                case sbt::idOffline:
                    emit signal_surrender();
                    break;
                case sbt::locate:
                    solve_resPlay_locate(myPacket);
                    break;
                case sbt::unmask:
                    solve_resPlay_unmask(myPacket);
                    break;
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }
    if(hintMessage!=nullptr){
        if(hintMessage_s!=nullptr)delete hintMessage_s;
        hintMessage->setModal(false);
        hintMessage->show();
        hintMessage_s=hintMessage;
    }
}
void socketManager::ClientRecvData(){
    qDebug() << "receive data";
    static QByteArray current_byte_array;
    QByteArray temp_array;
    static Packet recv_packet;
    while(clientSocket->bytesAvailable()>0){
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

}
void socketManager::ClientDisconnected(){
    if(clientSocket->isOpen())
        clientSocket->close();

}


int fillPacketHeader(packetHeader & header , unsigned char mainType , unsigned char resType , unsigned short msgLen)
{
    header.mainType = mainType ;
    header.subType = resType;
    header.length = htons(msgLen+HEADERLEN);
    return 0;
}
