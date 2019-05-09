#include "changepwd.h"
#include "ui_changepwd.h"
#include "socketmanager.h"
#include "common/mypacket.h"
extern socketManager *socketManagerW;

ChangePwd::ChangePwd(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChangePwd)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowFlags(Qt::FramelessWindowHint| Qt::WindowSystemMenuHint);
    ui->label_close->installEventFilter(this);

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

ChangePwd::~ChangePwd()
{
    delete ui;
}

bool ChangePwd::eventFilter(QObject *object, QEvent *e)
{
    if(e->type()==QEvent::MouseButtonPress&&object==ui->label_close)
    {

        if(socketManagerW->clientSocket->isOpen())
            socketManagerW->clientSocket->close();
        socketManagerW->myChangePwd=nullptr;
        close();
    }
    return false;
}

bool ChangePwd::try_changepwd()
{
    QString username = ui->lineEdit_userName->text();
    QString oldpwd = ui->lineEdit_oldPwd->text();
    QString newpwd = ui->lineEdit_newPwd->text();
    QString newpwd2 = ui->lineEdit_newPwd2->text();

    if(username.isEmpty())
    {
        QMessageBox::warning(nullptr,"warning","请输入用户名");
        return false;
    }

    if(oldpwd.isEmpty())
    {
        QMessageBox::warning(nullptr,"warning","请输入原密码");
        return false;
    }

    if(newpwd.isEmpty())
    {
        QMessageBox::warning(nullptr,"warning","请输入新密码");
        return false;
    }

    if(newpwd2.isEmpty())
    {
        QMessageBox::warning(nullptr,"warning","请确认新密码");
        return false;
    }

    if(newpwd.compare(newpwd2))
    {
        QMessageBox::warning(nullptr,"warning","新密码不一致");
        return false;
    }

    //连接服务器修改
    static changePwdData myChangePwdData;
    std::string str_temp;
    str_temp=newpwd.toStdString();
    strcpy(myChangePwdData.newPasswd,str_temp.c_str());
    Packet myPacket;
    myPacket.fillPacket(mt::login,sbt::changepwd,&myChangePwdData,sizeof(myChangePwdData));
    //根据连接服务器的结果输出
    socketManagerW->send_data(&myPacket,myPacket.getLen());
    return true;
}

void ChangePwd::on_pushButton_clicked()
{
    try_changepwd();
}

void ChangePwd::setUserName(QString name)
{
    ui->lineEdit_userName->setText(name);
    ui->lineEdit_userName->setReadOnly(true);
}

void ChangePwd::setOldPwd(QString pwd)
{
    ui->lineEdit_oldPwd->setText(pwd);
    ui->lineEdit_oldPwd->setReadOnly(true);
}
