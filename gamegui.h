#ifndef GAMEGUI_H
#define GAMEGUI_H

#include<QMainWindow>

#include<QWidget>
#include<QLabel>
#include<QMouseEvent>
#include<QDebug>
#include<QDir>
#include<QListWidgetItem>

#include "tools.h"

namespace Ui {
class GameGui;
}

class GameGui : public QWidget
{
    Q_OBJECT

public:
    explicit GameGui(QWidget *parent = nullptr);
    ~GameGui();

    //鼠标按下事件
    void mousePressEvent(QMouseEvent *event);
    //鼠标释放事件
    void mouseReleaseEvent(QMouseEvent *event);
    //鼠标移动事件
    void mouseMoveEvent(QMouseEvent *event);
    //最小化及关闭
    bool eventFilter(QObject *object, QEvent *e);
    //放置初始飞机，用于显示对方游戏进度
    void setInitPlane(QString str);
    //放置一架飞机（在游戏中，只有猜测一架飞机并猜中之后才显示）
    void setPlane(bool isMyGuessResult, int x1, int y1, int x2,int y2);
    //放置一个点（未命中，击中机头，击中机身）
    void setPoint(bool isMyGuessResult, int x,int y,const uchar status);

private slots:

    void on_radioButton_down_toggled(bool checked);

    void on_radioButton_up_toggled(bool checked);

    void on_radioButton_left_toggled(bool checked);

    void on_radioButton_right_toggled(bool checked);

    void on_radioButton_toggled(bool checked);

    void showText(QString str);

    void on_pushButton_clicked();

    void on_tableWidget_itemSelectionChanged();

private:
    Ui::GameGui *ui;

    QPoint move_point; //移动的距离

    bool mouse_press; //鼠标按下
};

#endif // GAMEGUI_H
