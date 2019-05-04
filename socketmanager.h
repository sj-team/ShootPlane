#ifndef MAINSOCKET_H
#define MAINSOCKET_H
#include <QObject>
#include "login.h"
#include "mainwindow.h"
#include "login.h"
#include "setting.h"
#include "friendlist.h"
#include "gamegui.h"
#include "creategame.h"
#include "changepwd.h"
#include "common/mypacket.h"
#include <QtNetwork/QtNetwork>

class socketManager : public QObject
{
    Q_OBJECT
public:
    explicit socketManager(QObject *parent = nullptr);
    ~socketManager();
    QTcpSocket *clientSocket;
    //
    Login *topLogin;
    ChangePwd *myChangePwd;
    FriendList *myFriendList;
    QMessageBox *startRequestWaiting;
    void send_data(void* data,int len); //发送消息
    void solve_packet(Packet* myPacket);
    void chgpasswd();
    void closeall();
    void login_success();
    void solve_friendlist(QString flist);
    void solve_updfriendlist(Packet *p);
    void solve_recv_newgame(char *name);
    void solve_connect_failure();
    void solve_connect_success();
signals:

public slots:
    void ClientRecvData();
    void ClientDisconnected();
};



#endif // MAINSOCKET_H
