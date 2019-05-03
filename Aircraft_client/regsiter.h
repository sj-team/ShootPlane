#ifndef REGSITER_H
#define REGSITER_H

#include<QWidget>
#include<QMouseEvent>
#include<QDebug>
#include<QDir>

#include "tools.h"

namespace Ui {
class Regsiter;
}

class Regsiter : public QWidget
{
    Q_OBJECT

public:
    explicit Regsiter(QWidget *parent = nullptr);
    ~Regsiter();

    //最小化及关闭
    bool eventFilter(QObject *object, QEvent *e);
    bool try_regsiter();

private slots:
    void on_pushButton_clicked();

private:
    Ui::Regsiter *ui;
};

#endif // REGSITER_H
