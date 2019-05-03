#ifndef MYTABLEWIDGET_H
#define MYTABLEWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QColor>
#include <QDebug>
#include <vector>

enum orient{None, Left,Right,Up,Down};

class MyTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit MyTableWidget(QWidget *parent = nullptr);

    //设置是否可修改（显示对方/自己）
    void setChangeAble(bool c);

    //设置飞机朝向
    void setOriented(orient n);

    //判断当前鼠标位置和朝向能否放置一个飞机
    bool isLegal();

    //将之前之前显示过的擦除
    void clearHover();

    //浮现一个飞机
    void airHover(int row,int col);

    //给出机头坐标和方向，给定一个二维数组，将飞机的10个点写入cur_airdata
    //要求已经检查过坐标合法
    void getAircraft(int x,int y,orient ori);

    void clearAirCraft(QString);
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

    //背景颜色
    QColor bg = QColor(255,255,255);

};

#endif // MYTABLEWIDGET_H
