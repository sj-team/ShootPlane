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
#include <QTimer>
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
    QString ip_address = "10.60.102.252";
    QString port = "20202";

    int beatTimeRecv;
    QTimer *beatTimeSend;
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
    void solve_repeat_off();
    void solve_beat();

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
    void slot_beat();
};



#endif // MAINSOCKET_H
