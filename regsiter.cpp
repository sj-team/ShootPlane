#include "regsiter.h"
#include "ui_regsiter.h"

Regsiter::Regsiter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Regsiter)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose,true);

    ui->label_close->installEventFilter(this);

    setWindowFlags(Qt::FramelessWindowHint| Qt::WindowSystemMenuHint);

    this->setAutoFillBackground(true); // 这句要加上, 否则可能显示不出背景图.
    QPalette palette = this->palette();
    QPixmap pix;

    //qDebug()<<"begin"<<QDir::currentPath()+"/img/login_bg.jpg";
    //if(pix.load(QDir::currentPath()+"/img/login_bg.jpg"))
    if(pix.load("../client/img/login_bg.jpg"))
    {
        palette.setBrush(QPalette::Window,
                         QBrush(pix.scaled( // 缩放背景图.
                                 this->size(),
                                Qt::IgnoreAspectRatio,
                                Qt::SmoothTransformation))); // 使用平滑的缩放方式
       this->setPalette(palette); // 至此, 已给widget加上了背景图.
        //qDebug()<<"success"<<QDir::currentPath()+"/img/login_bg.jpg";
    }
    else {
        qDebug()<<"failed"<<QDir::currentPath()+"/img/login_bg.jpg";
    }
}


Regsiter::~Regsiter()
{
    delete ui;
}

bool Regsiter::eventFilter(QObject *object, QEvent *e)
{
    if(e->type()==QEvent::MouseButtonPress&&object==ui->label_close)
    {
         close();
    }
    return false;
}

bool Regsiter:: try_regsiter()
{
    QString username = ui->lineEdit_userName->text();
    QString pwd = ui->lineEdit_pwd->text();
    QString pwd2 = ui->lineEdit_pwd2->text();

    if(username.isEmpty())
    {
        QMessageBox::warning(nullptr,"warning","请输入用户名");
        return false;
    }

    if(pwd.isEmpty())
    {
        QMessageBox::warning(nullptr,"warning","请输入密码");
        return false;
    }

    if(pwd2.isEmpty())
    {
        QMessageBox::warning(nullptr,"warning","请确认密码");
        return false;
    }

    if(pwd.compare(pwd2))
    {
        QMessageBox::warning(nullptr,"warning","密码不一致");
        return false;
    }
    return true;
}

void Regsiter::on_pushButton_clicked()
{
    if(try_regsiter())
    {
        close();
    }
}
