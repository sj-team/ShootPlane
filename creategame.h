#ifndef CREATEGAME_H
#define CREATEGAME_H

#include<QMainWindow>

#include<QWidget>
#include<QLabel>
#include<QMouseEvent>
#include<QDebug>
#include<QDir>
#include<QListWidgetItem>

#include "tools.h"

namespace Ui {
class CreateGame;
}

//enum orient{None,Left,Right,Up,Down};

class CreateGame : public QWidget
{
    Q_OBJECT

public:
    explicit CreateGame(QWidget *parent = nullptr);
    ~CreateGame();

    //鼠标按下事件
    void mousePressEvent(QMouseEvent *event);
    //鼠标释放事件
    void mouseReleaseEvent(QMouseEvent *event);
    //鼠标移动事件
    void mouseMoveEvent(QMouseEvent *event);
    //最小化及关闭
    bool eventFilter(QObject *object, QEvent *e);




private slots:
    void on_pushButton_undo_clicked();

    void on_pushButton_send_clicked();

    void on_radioButton_down_toggled(bool checked);

    void on_radioButton_up_toggled(bool checked);

    void on_radioButton_left_toggled(bool checked);

    void on_radioButton_right_toggled(bool checked);

    void showAirText(QString str);

private:
    Ui::CreateGame *ui;

    QPoint move_point; //移动的距离

    bool mouse_press; //鼠标按下

    QString aircraft1;
    QString aircraft2;
    QString aircraft3;

};

#endif // CREATEGAME_H
