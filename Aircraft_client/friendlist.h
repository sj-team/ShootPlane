#ifndef FRIENDLIST_H
#define FRIENDLIST_H

#include<QMainWindow>

#include<QWidget>
#include<QLabel>
#include<QMouseEvent>
#include<QDebug>
#include<QDir>
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

    //鼠标按下事件
    void mousePressEvent(QMouseEvent *event);
    //鼠标释放事件
    void mouseReleaseEvent(QMouseEvent *event);
    //鼠标移动事件
    void mouseMoveEvent(QMouseEvent *event);
    //最小化及关闭
    bool eventFilter(QObject *object, QEvent *e);

    void addFriend(QString name,bool isOnline);

private:
    Ui::FriendList *ui;

    QPoint move_point; //移动的距离

    bool mouse_press; //鼠标按下

    int i=0;

private slots:
  void on_listWidget_itemDoubleClicked(QListWidgetItem* item);
};

#endif // FRIENDLIST_H
