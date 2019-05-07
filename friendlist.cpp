#include "friendlist.h"
#include "ui_friendlist.h"
#include "socketmanager.h"
#include <QMessageBox>
#include <Qtimer>
extern socketManager *socketManagerW;
bool operator<(const FriendList::friendData &a,const FriendList::friendData &b){
    if(a.status==b.status){
        return a.name<b.name;
    }
    return a.status>b.status;
}
FriendList::FriendList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FriendList)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint| Qt::WindowSystemMenuHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose,true);

    ui->label_close->installEventFilter(this);
    ui->label_min->installEventFilter(this);
    ui->listWidget->setDragDropMode(QListWidget::InternalMove);
    connect(this, SIGNAL(signal_new_game(QString)), this, SLOT(solve_recv_newgame(QString)),Qt::QueuedConnection);
    connect(this, SIGNAL(signal_stop_connect()), this, SLOT(slot_stop_connect()),Qt::QueuedConnection);
    /*
    for(int j = 0;j<55;j++)
    {
        int stat=j%3;
        if(stat==1)stat=0x10;
        if(stat==2)stat=0x100;
        addFriend(QString("好友")+QString::number(j),stat);
//        qDebug()<<QString("好友")+QString::number(j);
    }*/
}
void FriendList::slot_stop_connect(){
    if(socketManagerW->recvMessage){
        socketManagerW->recvMessage->setResult(QMessageBox::StandardButton::Close);
        socketManagerW->recvMessage->close();
    }
}
FriendList::~FriendList()
{
    delete ui;
}

void FriendList::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton&&event->x()<=341 &&event->y()<51)
     {
         this->setMouseTracking(true);
          mouse_press = true;
          //鼠标相对于窗体的位置（或者使用event->globalPos() - this->pos()）
          move_point = event->pos();
     }

}

void FriendList::mouseReleaseEvent(QMouseEvent *event)
{
    //设置鼠标为未被按下
    mouse_press = false;

}

void FriendList::mouseMoveEvent(QMouseEvent *event)
{
        if(mouse_press)
        {
            QPoint move_pos = event->globalPos();
           //移动主窗体位置
           this->move(move_pos - move_point);
        }
}


bool FriendList::eventFilter(QObject *object, QEvent *e)
{
    if(this->isEnabled()==false)
        return false;
    if(e->type()==QEvent::MouseButtonPress&&object==ui->label_close)
    {
        socketManagerW->myFriendList=nullptr;
         close();
    }
    else if(e->type()==QEvent::MouseButtonPress&&object==ui->label_min){
        showMinimized();
        qDebug()<<"最小化";
    }



    return false;
}

void FriendList::addFriend(QString name, int status)
{
    int pos=0;
    int cnt=friendDataList.size();
    for(auto i=friendDataList.begin();i!=friendDataList.end();i++,pos++){
        if(i->name==name){
            friendDataList.erase(i);
            ui->listWidget->takeItem(pos);
            break;
        }
    }
    pos=0;
    friendData tempFirendData=friendData{name,status};
    for(auto i=friendDataList.begin();i!=friendDataList.end();i++,pos++){
        if(*i<tempFirendData)
            continue;
        else
            break;
    }
    QListWidgetItem * item = new QListWidgetItem(name);
    item->setFont(QFont("Microsoft YaHei Light",16));
    item->setBackground(QColor(226,225,228));
    if(status==friendData::stat_online) //在线
        item->setForeground(QBrush(QColor(Qt::black)));
    else if(status==friendData::stat_gaming) //游戏中
        item->setForeground(QBrush(QColor(Qt::red)));
    else //离线
        item->setForeground(QBrush(QColor(Qt::lightGray)));

    friendDataList.insert(pos,tempFirendData);
    ui->listWidget->insertItem(pos,item);

}
void FriendList::testend(){
    if(socketManagerW->startRequestWaiting){
        socketManagerW->startRequestWaiting->close();
        socketManagerW->startRequestWaiting->setResult(0);

    }
}
void FriendList::solve_recv_newgame(QString name){
    socketManagerW->op_name=name;
   socketManagerW->recvMessage =new QMessageBox("new game",QString(name)+" 向你发起对战，是否接受？",QMessageBox::Icon::Question,QMessageBox::StandardButton::Yes,QMessageBox::StandardButton::No,QMessageBox::StandardButton::NoButton);
    //myFriendList->setEnabled(false);
    int res=socketManagerW->recvMessage->exec();
    delete socketManagerW->recvMessage;
    socketManagerW->recvMessage=nullptr;
    Packet p;
    strcpy(p.msg,name.toStdString().c_str());
    if(res==QMessageBox::Close){
        qDebug()<<"cancel";
    }
    else if(res==QMessageBox::Yes){//接受
        qDebug()<<"accept";
        fillPacketHeader(p.header,mt::resConnect,sbt::accept,MAXNAMELEN);
    }else{//拒绝
         qDebug()<<"deny";
        fillPacketHeader(p.header,mt::resConnect,sbt::deny,MAXNAMELEN);
    }
    socketManagerW->send_data(&p,MAXNAMELEN+HEADERLEN);
}



void FriendList::on_listWidget_itemDoubleClicked(QListWidgetItem* item)
{
    for(auto i:friendDataList){
        if(i.name==item->text()){
            if(i.status==i.stat_offline){
                QMessageBox::information(nullptr,"warning","用户未登录",0,0);
                return;
            }else if(i.status==i.stat_gaming){
                QMessageBox::information(nullptr,"warning","用户在游戏中",0,0);
                return;
            }
            break;
        }
    }
    qDebug()<<"send msg to usr"<<item->text();
    Packet p;
    socketManagerW->op_name=item->text();
    std::string temp_str=item->text().toStdString();
    strcpy(p.msg,temp_str.c_str());
    fillPacketHeader(p.header,mt::connect,sbt::request,MAXNAMELEN);
    socketManagerW->send_data(&p,MAXNAMELEN+HEADERLEN);
    int res=socketManagerW->startRequestWaiting->exec();
    if(res==0){
        qDebug()<<"connect success";
        fillPacketHeader(p.header,mt::connect,sbt::success,MAXNAMELEN);
        strcpy(p.msg,temp_str.c_str());
        socketManagerW->send_data(&p,MAXNAMELEN+HEADERLEN);
        //QMessageBox::warning(nullptr,"test","success");
    }else{
        if(res==QMessageBox::Cancel){
            qDebug()<<"cancelled";
            fillPacketHeader(p.header,mt::resConnect,sbt::deny,MAXNAMELEN);
            strcpy(p.msg,temp_str.c_str());
            socketManagerW->send_data(&p,MAXNAMELEN+HEADERLEN);
        }
       qDebug()<<"connect failure err: "<< res;
    }

}

void FriendList::setUserName(QString str)
{
    ui->lineEdit_userName->setText(str);
}

void FriendList::setLastLoginTime(QString str)
{
    ui->lineEdit_lastLoginTime->setText(str);
}

void FriendList::setCurrentOnlineTime(QString str)
{
    ui->lineEdit_currentOnlineTime->setText(str);
}

void FriendList::setIpAddress(QString str)
{
    ui->lineEdit_ipAddress->setText(str);
}

void FriendList::setPort(QString str)
{
    ui->lineEdit_port->setText(str);
}

void FriendList::setStatus(QString str)
{
    ui->lineEdit_status->setText(str);
}

