#include "creategame.h"
#include "ui_creategame.h"

# include <QMessageBox>

#include "mytablewidget.h"
#include "socketmanager.h"
extern socketManager *socketManagerW;
CreateGame::CreateGame(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CreateGame)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint| Qt::WindowSystemMenuHint);
    setAttribute(Qt::WA_TranslucentBackground);
//    setAttribute(Qt::WA_DeleteOnClose,true);

    ui->label_close->installEventFilter(this);
    ui->label_min->installEventFilter(this);

    ui->tableWidget->setChangeAble(true);
    ui->tableWidget->setisGame(false);

    connect(ui->tableWidget,SIGNAL(send(QString)),this,SLOT(showAirText(QString)));
}

CreateGame::~CreateGame()
{
    delete ui;
}

void CreateGame::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton&&event->x()<=700 &&event->y()<51)
     {
         this->setMouseTracking(true);
          mouse_press = true;
          //鼠标相对于窗体的位置（或者使用event->globalPos() - this->pos()）
          move_point = event->pos();
     }

}

void CreateGame::mouseReleaseEvent(QMouseEvent *event)
{
    //设置鼠标为未被按下
    mouse_press = false;

}

void CreateGame::mouseMoveEvent(QMouseEvent *event)
{
        if(mouse_press)
        {
            QPoint move_pos = event->globalPos();
           //移动主窗体位置
           this->move(move_pos - move_point);
        }
}

bool CreateGame::eventFilter(QObject *object, QEvent *e)
{
    if(e->type()==QEvent::MouseButtonPress&&object==ui->label_close)
    {
        socketManagerW->myChangePwd=nullptr;
        //send exit
        socketManagerW->return_friendlist();
        //surrender
        packetHeader ph;
        qDebug()<<"surrender";
        fillPacketHeader(ph,mt::play,sbt::surrender,0);
        socketManagerW->send_data(&ph,HEADERLEN);
        close();
    }
    else if(e->type()==QEvent::MouseButtonPress&&object==ui->label_min){
        showMinimized();
        qDebug()<<"最小化";
    }

    return false;
}

void CreateGame::on_pushButton_undo_clicked()
{
    //setButtonColoBack();
    if(!aircraft3.isEmpty())//已添加三架飞机
    {
        //清除table中的颜色
        ui->tableWidget->clearAirCraft(aircraft3);

        aircraft3 = "";
        //qDebug()<<"3-"<<aircraft3.size()<<aircraft3;
        ui->lineEdit_3->setText(aircraft3);
        return;
    }
    else if(!aircraft2.isEmpty())
    {
        //清除table中的颜色
        ui->tableWidget->clearAirCraft(aircraft2);

        aircraft2 = "";
        ui->lineEdit_2->setText(aircraft2);

        return;
    }
    else if(!aircraft1.isEmpty())
    {
        //清除table中的颜色
        ui->tableWidget->clearAirCraft(aircraft1);

        aircraft1 = "";
        ui->lineEdit_1->setText(aircraft1);

        return;
    }

}

void CreateGame::on_pushButton_send_clicked()
{
    if(aircraft3.isEmpty())
    {
        QMessageBox::warning(nullptr,"warning","请添加3架飞机");
        return;
    }
    //此处发送三个飞机坐标
    qDebug()<<aircraft1<<aircraft2<<aircraft3;
    locateData locdat;
    std::string tempStr=aircraft1.toStdString();
    locdat.p1_x1=tempStr[0]-'A';
    locdat.p1_y1=tempStr[1]-'0';
    locdat.p1_x2=tempStr[3]-'A';
    locdat.p1_y2=tempStr[4]-'0';
    tempStr=aircraft2.toStdString();
    locdat.p2_x1=tempStr[0]-'A';
    locdat.p2_y1=tempStr[1]-'0';
    locdat.p2_x2=tempStr[3]-'A';
    locdat.p2_y2=tempStr[4]-'0';
    tempStr=aircraft3.toStdString();
    locdat.p3_x1=tempStr[0]-'A';
    locdat.p3_y1=tempStr[1]-'0';
    locdat.p3_x2=tempStr[3]-'A';
    locdat.p3_y2=tempStr[4]-'0';
    Packet p;
    p.fillPacket(mt::init,sbt::locate,&locdat,sizeof(locdat));
    socketManagerW->send_data(&p,HEADERLEN+sizeof(locdat));
    close();
    socketManagerW->myFriendList->setEnabled(false);
    socketManagerW->myGameGui=new GameGui;

    socketManagerW->myGameGui->setInitPlane(aircraft1);
    socketManagerW->myGameGui->setInitPlane(aircraft2);
    socketManagerW->myGameGui->setInitPlane(aircraft3);
    socketManagerW->myGameGui->show();
}

void CreateGame::on_radioButton_down_toggled(bool checked)
{
    if(checked)
    {
        //qDebug()<<"choose down";
        ui->tableWidget->setOriented(orient(Down));

    }
}

void CreateGame::on_radioButton_up_toggled(bool checked)
{
    if(checked)
    {
        //qDebug()<<"choose down";
        ui->tableWidget->setOriented(orient(Up));
    }
}

void CreateGame::on_radioButton_left_toggled(bool checked)
{
    if(checked)
    {
        //qDebug()<<"choose down";
        ui->tableWidget->setOriented(orient(Left));
    }
}

void CreateGame::on_radioButton_right_toggled(bool checked)
{
    if(checked)
    {
        //qDebug()<<"choose down";
        ui->tableWidget->setOriented(orient(Right));
    }
}

void CreateGame::showAirText(QString str)
{
    if(aircraft1.isEmpty())
    {
        aircraft1 = str;
        ui->lineEdit_1->setText(str);
        //qDebug()<<"create"<<aircraft1;
        return;
    }

    if(aircraft2.isEmpty())
    {
        aircraft2 = str;
        ui->lineEdit_2->setText(str);
        //qDebug()<<"create"<<aircraft1;
        return;
    }

    if(aircraft3.isEmpty())
    {
        aircraft3 = str;
        ui->lineEdit_3->setText(str);
        //qDebug()<<"create"<<aircraft1;
        return;
    }

}

void CreateGame::setTitle(QString str)
{
    ui->label_title->setText(str);
}
