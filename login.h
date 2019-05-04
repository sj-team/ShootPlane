#ifndef LOGIN_H
#define LOGIN_H

#include<QMainWindow>

#include<QWidget>
#include<QLabel>
#include<QMouseEvent>
#include<QDebug>
#include<QDir>

#include "tools.h"


namespace Ui {
class Login;
}

class Login : public QMainWindow
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();
    QPoint move_point; //移动的距离

    bool mouse_press; //鼠标按下
    //鼠标按下事件
    void mousePressEvent(QMouseEvent *event);
    //鼠标释放事件
    void mouseReleaseEvent(QMouseEvent *event);
    //鼠标移动事件
    void mouseMoveEvent(QMouseEvent *event);
    //最小化及关闭
    bool eventFilter(QObject *object, QEvent *e);

    bool try_login();

    QString getUserName();

    QString getPwd();

private slots:
     void on_pushButton_login_clicked();

     void on_pushButton_regsiter_clicked();

     void on_pushButton_changepwd_clicked();


protected:
     void showEvent(QShowEvent *event);

private:
    Ui::Login *ui;
    int i = 0;

};

#endif // LOGIN_H
