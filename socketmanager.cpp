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
    beatTimeRecv=0;
    beatTimeSend=nullptr;

    QFile file("./setting.json");
    if(file.open(QIODevice::ReadOnly))
    {
        QByteArray allData = file.readAll();
        file.close();

        QJsonParseError json_error;
        QJsonDocument jsonDoc(QJsonDocument::fromJson(allData, &json_error));

        if(json_error.error != QJsonParseError::NoError)
        {
            qDebug() << "json error!";
        }
        else
        {
            QJsonObject rootObj = jsonDoc.object();
            QStringList keys = rootObj.keys();
            qDebug()<<"load ip_address = "<<rootObj["ip"].toString();
            qDebug()<<"load port = "<<rootObj["port"].toString();
            ip_address = rootObj["ip"].toString();
            port = rootObj["port"].toString();
        }
    }
    else {
        qDebug() << "could't open projects json";
    }

    connect(this, SIGNAL(signal_surrender()), this, SLOT(slot_surrender()),Qt::QueuedConnection);
    connect(this, SIGNAL(signal_win()), this, SLOT(slot_win()),Qt::QueuedConnection);
    connect(this, SIGNAL(signal_lose()), this, SLOT(slot_lose()),Qt::QueuedConnection);
}
socketManager::~socketManager()
{
    if(topLogin!=nullptr)
        delete topLogin;
    if(myFriendList!=nullptr)
        delete myFriendList;
    if(myChangePwd!=nullptr)
        delete myChangePwd;
    if(startRequestWaiting!=nullptr)
        delete startRequestWaiting;
    if(hintMessage_s!=nullptr)
        delete hintMessage_s;
    if(myCreateGame!=nullptr)
        delete myCreateGame;
    if(myGameGui!=nullptr)
        delete myGameGui;
    if(beatTimeSend!=nullptr)
        delete beatTimeSend;
    delete clientSocket;
}
void socketManager::chgpasswd(){

    myChangePwd=new ChangePwd(topLogin);
    myChangePwd->setUserName(topLogin->getUserName());
    myChangePwd->setOldPwd(topLogin->getPwd());
    myChangePwd->show();
}
void socketManager::closeall(){
    if(myFriendList!=nullptr){
        delete myFriendList;
        myFriendList=nullptr;
    }
    if(myChangePwd!=nullptr){
        delete myChangePwd;
        myChangePwd=nullptr;
    }
    if(hintMessage_s!=nullptr){
        delete hintMessage_s;
        hintMessage_s=nullptr;
    }
    if(myCreateGame!=nullptr){
        delete myCreateGame;
        myCreateGame=nullptr;
    }
    if(myGameGui!=nullptr){
        delete myGameGui;
        myGameGui=nullptr;
    }

}
void socketManager::slot_beat(){
    packetHeader ph;
    fillPacketHeader(ph,mt::beat,sbt::myDefault,0);
    send_data(&ph,sizeof(ph));
    beatTimeRecv--;
    if(beatTimeRecv<=0){
        clientSocket->close();
        //beatTimeSend->stop();
    }
    if(myFriendList)
    {
        myFriendList->setCurrentOnlineTime(QString::number(myFriendList->online_time++));
    }
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
    myFriendList->setUserName(name);
    myFriendList->setIpAddress(ip_address);
    myFriendList->setPort(port);
    myFriendList->setStatus("我在线上");
    myFriendList->show();
}
void socketManager::send_data(void *data, int len){
    Packet* mp=(Packet*)data;
    if(mp->header.mainType!=mt::beat)qDebug()<<int(mp->header.mainType)<<"   "<<int(mp->header.subType)<<" "<<(mp->msg)<<" "<<(mp->msg+32);
    if(!clientSocket->isOpen()){
        beatTimeRecv=MAXBEATCTR;
        clientSocket->connectToHost(QHostAddress(ip_address), port.toInt(nullptr,10));
        bool state=clientSocket->waitForConnected(1000);
        if(!state){
            if(!myFriendList)
                QMessageBox::warning(nullptr,"warning","连接服务器失败");
            clientSocket->close();
            return;
        }
        connect(clientSocket, SIGNAL(readyRead()), this, SLOT(ClientRecvData()));
        connect(clientSocket, SIGNAL(disconnected()), this, SLOT(ClientDisconnected()));
        beatTimeSend=new QTimer;
        connect(beatTimeSend, SIGNAL(timeout()), this, SLOT(slot_beat()));
        beatTimeSend->start(BEATMSEC);
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
    }
    game_name=name + " VS " + op_name;
    myCreateGame->setTitle(game_name);
    myCreateGame->show();

}
void socketManager::return_friendlist(){//返回列表界面
    myFriendList->setEnabled(true);
}
void socketManager::solve_play_locate(Packet *p){
    unmaskLocateResult *resPtr=(unmaskLocateResult *)p->msg;
    myGameGui->setScore(0,resPtr->score);
    if(resPtr->result){
        myGameGui->setPlane(0,resPtr->x1,resPtr->y1,resPtr->x2,resPtr->y2);
    }
    else {
        myGameGui->appendGameLog(0,resPtr->x1,resPtr->y1,resPtr->x2,resPtr->y2,0);
    }
}
void socketManager::solve_resPlay_locate(Packet *p){
    unmaskLocateResult *resPtr=(unmaskLocateResult *)p->msg;

    myGameGui->setScore(1,resPtr->score);
    if(resPtr->result){
        myGameGui->setPlane(1,resPtr->x1,resPtr->y1,resPtr->x2,resPtr->y2);
    }
    else {
        myGameGui->appendGameLog(1,resPtr->x1,resPtr->y1,resPtr->x2,resPtr->y2,0);
    }
}
void socketManager::solve_play_unmask(Packet *p){
    unmaskPointResult *resPtr=(unmaskPointResult *)p->msg;
    myGameGui->setScore(0,resPtr->score);
    myGameGui->setPoint(0,resPtr->x,resPtr->y,resPtr->result);
}
void socketManager::solve_resPlay_unmask(Packet *p){
    unmaskPointResult *resPtr=(unmaskPointResult *)p->msg;
    myGameGui->setScore(1,resPtr->score);
    myGameGui->setPoint(1,resPtr->x,resPtr->y,resPtr->result);
}
void socketManager::solve_repeat_off(){
    qDebug()<<"repeatoff";
    QMessageBox::warning(nullptr,"warning","同一个账号重复登录，自动下线");
    closeall();
    topLogin->show();
}

void socketManager::solve_beat(){
    beatTimeRecv=MAXBEATCTR;
}

void socketManager::solve_packet(Packet *myPacket){
    if(myPacket->header.mainType!=mt::beat)qDebug()<<int(myPacket->header.mainType)<<"   "<<int(myPacket->header.subType);
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
                    qDebug()<<"password error";
                    hintMessage= new QMessageBox(QMessageBox::Icon::Warning,"warning","密码错误");
                    break;
                case sbt::repeaton:
                    hintMessage= new QMessageBox(QMessageBox::Icon::Warning,"warning","重复上线");
                    login_success();
                    break;
                case sbt::success:
                    login_success();
                    break;
                case sbt::repeatoff:
                    solve_repeat_off();
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
                    myGameGui->setCurOprator(name);
                    break;
                case sbt::wait:
                    myGameGui->setStatus("对方回合");
                    myGameGui->setCurOprator(op_name);
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
        case mt::beat:
            solve_beat();
            break;
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
    qDebug()<<"disconnect";
    if(beatTimeSend){
        delete beatTimeSend;
        beatTimeSend=nullptr;
    }
    if(clientSocket->isOpen()){
        clientSocket->close();
    }
    if(myFriendList){
        myFriendList->setStatus("离线");
        QMessageBox::warning(nullptr,"warning","与服务器断开连接！");

        closeall();
        topLogin->show();
    }


}


int fillPacketHeader(packetHeader & header , unsigned char mainType , unsigned char resType , unsigned short msgLen)
{
    header.mainType = mainType ;
    header.subType = resType;
    header.length = htons(msgLen+HEADERLEN);
    return 0;
}
