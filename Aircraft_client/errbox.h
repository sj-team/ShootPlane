#ifndef ERRBOX_H
#define ERRBOX_H

#include <QDialog>
#include<QMouseEvent>
#include<QDebug>
#include<QDir>

namespace Ui {
class ErrBox;
}

class ErrBox : public QDialog
{
    Q_OBJECT

public:
    explicit ErrBox(const QString data, QWidget *parent = nullptr);
    ~ErrBox();
    //最小化及关闭
    bool eventFilter(QObject *object, QEvent *e);

private:
    Ui::ErrBox *ui;
};

#endif // ERRBOX_H
