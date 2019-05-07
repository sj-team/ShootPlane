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
    CreateGame *myCreateGame;
    GameGui *myGameGui;
    QMessageBox *hintMessage_s;
    QMessageBox *recvMessage;
    QString name;
    QString op_name;
    QString game_name;
    void send_data(void* data,int len); //发送消息
    void solve_packet(Packet* myPacket);
    void chgpasswd();
    void closeall();
    void login_success();
    void solve_friendlist(QString flist);
    void solve_updfriendlist(Packet *p);
    void solve_connect_failure();
    void solve_connect_success();
    void solve_start_locate();
    void return_friendlist();
    void end_game();
    void solve_play_locate(Packet *p);
    void solve_play_unmask(Packet *p);
    void solve_resPlay_locate(Packet *p);
    void solve_resPlay_unmask(Packet *p);

signals:
    void signal_surrender();
    void signal_win();
    void signal_lose();
public slots:
    void slot_surrender();
    void ClientRecvData();
    void ClientDisconnected();
    void slot_win();
    void slot_lose();
};



#endif // MAINSOCKET_H
