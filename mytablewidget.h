#ifndef MYTABLEWIDGET_H
#define MYTABLEWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QColor>
#include <QDebug>
#include <vector>

enum orient{None, Left,Right,Up,Down};

namespace bs {
    const uchar empty = 0x00;
    const uchar head = 0x01;
    const uchar body = 0x02;
    const uchar unkown = 0x03;
    const uchar err = 0x99;
}

class MyTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit MyTableWidget(QWidget *parent = nullptr);

    //设置是否可修改（显示对方/自己）
    void setChangeAble(bool c);

    void setisGame(bool c);

    //设置飞机朝向
    void setOriented(orient n);

    //判断当前鼠标位置和朝向能否放置一个飞机
    bool isLegal();
    //判断当前鼠标位置和朝向放置一个飞机是否冲突
    bool isCovered();

    //将之前之前浮现过的擦除
    void clearHover();

    //浮现一个飞机
    void airHover(int row,int col);

    //给出机头坐标和方向，给定一个二维数组，将飞机的10个点写入cur_airdata
    //要求已经检查过坐标合法
    void getAircraft(int x,int y,orient ori);

    void clearAirCraft(QString);

    //显示一架飞机并保存到map
    void showPlane();

    //设置显示一架飞机（用于游戏中看对方进度和自己进度）
    void setPlane(QString str);
    void setPlane(int x1,int y1,int x2,int y2);

    //设置一个点的显示
    void setPoint(int x,int y,const uchar status);

signals:
    void send(QString str);

protected:
    void leaveEvent(QEvent *event);//鼠标移出事件

public slots:
    void cellEntered(int,int);//鼠标移动事件，返回item的行和列号
    void cellClicked(int,int);


private:
    //是否可修改（用于区分对方、自己游戏）
    bool changeAble;

    bool isGame;

    const int choosed = 10;

    //棋盘
    int map[10][10];
    //当前飞机数量
    int num=0;
    //当前飞机朝向
    orient cur_oriented;
    //之前飞机朝向
    orient pre_oriented;

    //当前飞机坐标
    int cur_airdata[10][2];
    //之前飞机坐标
    int pre_airdata[10][2];

    //鼠标悬浮颜色/选中颜色
    QColor hovered = QColor(255, 255, 127);

    QColor airColor[3] = {QColor(225,108,150),QColor(140,194,105),QColor(33,119,184)};

    QColor myGray[3]={QColor(177,177,177),QColor(222,222,222),QColor(157,157,157)};

    //背景颜色
    QColor bg = QColor(255,255,255);

    //猜测后返回的颜色，随机写的，要改
    QColor miss = QColor(0,0,0);
    QColor head = QColor(54,203,87);
    QColor body = QColor(77,99,123);

    //QColor myhover[10];

    QString cur_str;

};

#endif // MYTABLEWIDGET_H