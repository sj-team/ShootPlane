#ifndef CHANGEPWD_H
#define CHANGEPWD_H

#include<QWidget>
#include<QMouseEvent>
#include<QDebug>
#include<QDir>

#include "tools.h"

namespace Ui {
class ChangePwd;
}

class ChangePwd : public QWidget
{
    Q_OBJECT

public:
    explicit ChangePwd(QWidget *parent = nullptr);
    ~ChangePwd();

    //最小化及关闭
    bool eventFilter(QObject *object, QEvent *e);
    bool try_changepwd();

    void setUserName(QString name);
    void setOldPwd(QString pwd);

private slots:
    void on_pushButton_clicked();

private:
    Ui::ChangePwd *ui;
};

#endif // CHANGEPWD_H
