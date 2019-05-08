#ifndef FRIENDLIST_H
#define FRIENDLIST_H

#include<QMainWindow>

#include<QWidget>
#include<QLabel>
#include<QMouseEvent>
#include<QDebug>
#include<QDir>
#include <QLinkedList>
#include <QList>
#include<QListWidgetItem>
#include "tools.h"

namespace Ui {
class FriendList;
}

class FriendList : public QWidget
{
    Q_OBJECT

public:
    explicit FriendList(QWidget *parent = nullptr);
    ~FriendList();
    struct friendData{
        QString name;
        int status;
        const static int stat_offline=0;
        const static int stat_gaming=0x10;
        const static int stat_online=0x100;
    };
    QList<friendData> friendDataList;

    int online_time;
    int online_num;
    int gaming_num;
    int offline_num;

    //鼠标按下事件
    void mousePressEvent(QMouseEvent *event);
    //鼠标释放事件
    void mouseReleaseEvent(QMouseEvent *event);
    //鼠标移动事件
    void mouseMoveEvent(QMouseEvent *event);
    //最小化及关闭
    bool eventFilter(QObject *object, QEvent *e);

    void addFriend(QString name,int status);

    void updInfo();


    //设置用户名
    void setUserName(QString str);
    //设置当前在线时间
    void setCurrentOnlineTime(QString str);
    //设置当前状态
    void setStatus(QString str);
    //设置ip地址
    void setIpAddress(QString str);
    //设置端口
    void setPort(QString str);
private:
    Ui::FriendList *ui;

    QPoint move_point; //移动的距离

    bool mouse_press; //鼠标按下

    int i=0;

private slots:
  void on_listWidget_itemDoubleClicked(QListWidgetItem* item);
  void testend();
signals:
    void signal_new_game(QString name);
    void signal_stop_connect();
public slots:
  void solve_recv_newgame(QString name);
  void slot_stop_connect();
};

bool operator<(const FriendList::friendData &a,const FriendList::friendData &b);
#endif // FRIENDLIST_H
