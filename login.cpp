#include "login.h"
#include "ui_login.h"

#include "setting.h"
#include "changepwd.h"
#include "regsiter.h"

#include "friendlist.h"

#include <string>
#include"socketmanager.h"
extern socketManager *socketManagerW;

Login::Login(QWidget *parent) :
   QMainWindow(parent),
   ui(new Ui::Login)
{
   ui->setupUi(this);
//    this->setAttribute(Qt::WA_DeleteOnClose,true);
   setWindowFlags(Qt::FramelessWindowHint| Qt::WindowSystemMenuHint);

//    this->setAttribute(Qt::WA_TranslucentBackground);

    ui->label_close->installEventFilter(this);
    ui->label_min->installEventFilter(this);
    ui->label_setting->installEventFilter(this);

    //this->setAutoFillBackground(true); // 这句要加上, 否则可能显示不出背景图.
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

Login::~Login()
{
   delete ui;
}


void Login::mousePressEvent(QMouseEvent *event)
{
   if(event->button() == Qt::LeftButton)
    {
        this->setMouseTracking(true);
         mouse_press = true;
         //鼠标相对于窗体的位置（或者使用event->globalPos() - this->pos()）
         move_point = event->pos();
         //qDebug()<<"移动1";
         //qDebug()<<event->source();
    }

}

void Login::mouseReleaseEvent(QMouseEvent *event)
{
   //设置鼠标为未被按下
       mouse_press = false;

}

void Login::mouseMoveEvent(QMouseEvent *event)
{
   //若鼠标左键被按下
 //  qDebug()<<"mouse_press="<<event->globalPos();
       if(mouse_press)
       {
           i++;
           //鼠标相对于屏幕的位置
           QPoint move_pos = event->globalPos();
            //qDebug()<<"移动"<<i;
          //移动主窗体位置
          this->move(move_pos - move_point);
       }
}

void Login::showEvent(QShowEvent *event)
{
   mouse_press=false;
  // qDebug()<<event;
}

bool Login::eventFilter(QObject *object, QEvent *e)
{
   if(e->type()==QEvent::MouseButtonPress&&object==ui->label_close)
   {
        close();
        exit(1);
   }
   else if(e->type()==QEvent::MouseButtonPress&&object==ui->label_min){
       showMinimized();
       qDebug()<<"最小化";

   }
   else if(e->type()==QEvent::MouseButtonPress&&object==ui->label_setting){
       Setting *s = new Setting(this);
       s->show();
       qDebug()<<"设置";

   }


   return false;
}

bool Login::try_login()
{
   QString username = ui->lineEdit_username->text();
   QString pwd = ui->lineEdit_pwd->text();

   if(username.isEmpty())
   {
       QMessageBox::warning(nullptr,"warning","用户名不能为空");
       return false;
   }

   if(pwd.isEmpty())
   {
       QMessageBox::warning(nullptr,"warning","密码不能为空");
       return false;
   }

   //根据连接服务器的结果输出
   static loginData myLoginData;
   std::string str_temp;
   str_temp=username.toStdString();
   strcpy(myLoginData.username,str_temp.c_str());
   str_temp=pwd.toStdString();
   strcpy(myLoginData.passwd,str_temp.c_str());
   Packet myPacket;
   myPacket.fillPacket(mt::login,sbt::request,&myLoginData,sizeof(myLoginData));
   //根据连接服务器的结果输出
   socketManagerW->send_data(&myPacket,myPacket.getLen());

//    FriendList *f = new FriendList();
//    f->show();
//    close();

   return true;
}

void Login::on_pushButton_login_clicked()
{
   try_login();
   qDebug()<<"登录";
}

void Login::on_pushButton_regsiter_clicked()
{
   Regsiter * s = new Regsiter(this);
   s->show();
   qDebug()<<"注册";
}

void Login::on_pushButton_changepwd_clicked()
{
   ChangePwd * s = new ChangePwd(this);
   s->show();
   qDebug()<<"修改密码";
}


QString Login::getPwd()
{
    return ui->lineEdit_pwd->text();;
}

QString Login::getUserName()
{
    return ui->lineEdit_username->text();
}
