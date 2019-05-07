#include "gamegui.h"
#include "ui_gamegui.h"

#include "errbox.h"

#include "mytablewidget.h"
#include "socketmanager.h"
extern socketManager *socketManagerW;
GameGui::GameGui(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GameGui)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint| Qt::WindowSystemMenuHint);
    setAttribute(Qt::WA_TranslucentBackground);
//    setAttribute(Qt::WA_DeleteOnClose,true);

    ui->label_close->installEventFilter(this);
    ui->label_min->installEventFilter(this);

    ui->tableWidget->setChangeAble(true);
    ui->tableWidget->setisGame(true);

    ui->tableWidget_2->setisGame(false);
    setUserName(socketManagerW->name);
    setOpName(socketManagerW->op_name);
    setTitle(socketManagerW->game_name);
    setStatus("等待中");
    //此处要修改为creategame中的三个坐标
//    QString tmp_str[3]={"B2-E2","D4-D7","G5-G2"};
//    for(int i=0;i<3;i++)
//    {

//        setInitPlane(tmp_str[i]);
//        setPlane(true,1,2,4,2);
//    }

//    setPoint(true,3,7,0);
//    setPoint(true,5,7,1);
//    setPoint(true,3,9,2);
//    setPoint(true,4,6,1);


    connect(ui->tableWidget,SIGNAL(send(QString)),this,SLOT(showText(QString)));
}

GameGui::~GameGui()
{

    delete ui;
}

void GameGui::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton&&event->x()<=ui->label_min->x() &&event->y()<51)
     {
         this->setMouseTracking(true);
          mouse_press = true;
          //鼠标相对于窗体的位置（或者使用event->globalPos() - this->pos()）
          move_point = event->pos();
     }

}

void GameGui::mouseReleaseEvent(QMouseEvent *event)
{
    //设置鼠标为未被按下
    mouse_press = false;

}

void GameGui::mouseMoveEvent(QMouseEvent *event)
{
        if(mouse_press)
        {
            QPoint move_pos = event->globalPos();
           //移动主窗体位置
           this->move(move_pos - move_point);
        }
}

bool GameGui::eventFilter(QObject *object, QEvent *e)
{
    if(e->type()==QEvent::MouseButtonPress&&object==ui->label_close)
    {
        socketManagerW->myGameGui=nullptr;
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

void GameGui::on_radioButton_toggled(bool checked)
{
    if(checked)
    {
        //qDebug()<<"choose down";
        ui->tableWidget->setOriented(orient(None));
    }
}

void GameGui::on_radioButton_down_toggled(bool checked)
{
    if(checked)
    {
        //qDebug()<<"choose down";
        ui->tableWidget->setOriented(orient(Down));

    }
}

void GameGui::on_radioButton_up_toggled(bool checked)
{
    if(checked)
    {
        //qDebug()<<"choose down";
        ui->tableWidget->setOriented(orient(Up));
    }
}

void GameGui::on_radioButton_left_toggled(bool checked)
{
    if(checked)
    {
        //qDebug()<<"choose down";
        ui->tableWidget->setOriented(orient(Left));
    }
}

void GameGui::on_radioButton_right_toggled(bool checked)
{
    if(checked)
    {
        //qDebug()<<"choose down";
        ui->tableWidget->setOriented(orient(Right));
    }
}

void GameGui::showText(QString str)
{
    ui->lineEdit->setText(str);
}

void GameGui::setInitPlane(QString str)
{
    ui->tableWidget_2->setPlane(str);
    appendGameLog(ui->lineEdit_uerName->text()+"设置了初始飞机"+str);
}

void GameGui::setPlane(bool isMyGuessResult, int x1, int y1, int x2, int y2)
{
    if(isMyGuessResult)
    {
        ui->tableWidget->setPlane(x1,y1,x2,y2);
    }
    else
    {
        ui->tableWidget_2->setPlane(x1,y1,x2,y2);
    }
    appendGameLog(isMyGuessResult,x1,y1,x2,y2,true);
}

void GameGui::setPoint(bool isMyGuessResult, int x, int y, const uchar status)
{
    if(isMyGuessResult)
    {
        ui->tableWidget->setPoint(x,y,status);
    }
    else
    {
        ui->tableWidget_2->setPoint(x,y,status);
    }
    appendGameLog(isMyGuessResult,x,y,status);
}


void GameGui::on_pushButton_clicked()
{
    if(ui->lineEdit_curStatus->text()!=QString("你的回合")){
        QMessageBox::warning(nullptr,"warning","不是你的回合");
        return;
    }

    //发送坐标信息
    qDebug()<<"发送坐标信息: "<<ui->lineEdit->text();
    std::string text=ui->lineEdit->text().toStdString();
    ui->tableWidget->clearAirCraft(ui->lineEdit->text());
    Packet p;
    if(text[0]==text[3] && text[1]==text[4]){
        fillPacketHeader(p.header,mt::play,sbt::unmask,2);
        p.msg[0]=text[0]-'A';
        p.msg[1]=text[1]-'0';
    }else{
        fillPacketHeader(p.header,mt::play,sbt::locate,4);
        p.msg[0]=text[0]-'A';
        p.msg[1]=text[1]-'0';
        p.msg[2]=text[3]-'A';
        p.msg[3]=text[4]-'0';
    }
    socketManagerW->send_data(&p,p.getLen());

    ui->tableWidget->cur_str="";
//    appendGameLog(ui->lineEdit->text());
}


void GameGui::on_tableWidget_itemSelectionChanged()
{
    ui->tableWidget->clearSelection ();
}

void GameGui::setTitle(QString str)
{
    ui->label_title->setText(str);
}

void GameGui::on_pushButton_sur_clicked()
{
    //投降按钮
    socketManagerW->myGameGui=nullptr;
    socketManagerW->return_friendlist();
    //surrender
    packetHeader ph;
    qDebug()<<"surrender";
    fillPacketHeader(ph,mt::play,sbt::surrender,0);
    socketManagerW->send_data(&ph,HEADERLEN);
    close();
}

void GameGui::setUserName(QString str)
{
    ui->lineEdit_uerName->setText(str);
}

void GameGui::setOpName(QString str)
{
    ui->lineEdit_opName->setText(str);
}

void GameGui::setCurOprator(QString str)
{
    ui->lineEdit_curPlayer->setText(str);
}

void GameGui::setStatus(QString str)
{
    ui->lineEdit_curStatus->setText(str);
}

void GameGui::appendGameLog(QString str)
{
    QTime current_time =QTime::currentTime();
    QString data = current_time.toString()+"\n"+str;

    QListWidgetItem * item = new QListWidgetItem(data);
    ui->listWidget->addItem(item);
}

void GameGui::appendGameLog(bool isMyGuessResult, int x,int y,const uchar status)
{
    QTime current_time =QTime::currentTime();
    QString data = current_time.toString()+"\n";

    if(isMyGuessResult)
    {
        data += ui->lineEdit_uerName->text();
    }
    else
    {
        data += ui->lineEdit_opName->text();
    }
    data += " 猜测了点";
    data += QString('A'+x);
    data += QString('0'+y);
    data += ", 结果为";
    if(status == bs::empty)
    {
        data += "未命中";
    }
    else if(status == bs::head)
    {
        data += "命中机头";
    }
    else if(status == bs::body)
    {
        data += "命中机身";
    }

    QListWidgetItem * item = new QListWidgetItem(data);
    ui->listWidget->addItem(item);
}

void GameGui::appendGameLog(bool isMyGuessResult, int x1, int y1, int x2,int y2, bool isSuccess)
{
    QTime current_time =QTime::currentTime();
    QString data = current_time.toString()+"\n";

    if(isMyGuessResult)
    {
        data += ui->lineEdit_uerName->text();
    }
    else
    {
        data += ui->lineEdit_opName->text();
    }
    data += " 猜测了整架飞机(";
    data += QString('A'+x1);
    data += QString('0'+y1);
    data += ",";
    data += QString('A'+x2);
    data += QString('0'+y2);
    data += "), 结果为";
    if(isSuccess)
    {
        data += "命中";
    }
    else
    {
        data += "未命中";
    }

    QListWidgetItem * item = new QListWidgetItem(data);
    ui->listWidget->addItem(item);
}
