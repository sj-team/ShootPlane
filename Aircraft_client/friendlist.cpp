#include "friendlist.h"
#include "ui_friendlist.h"

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
    for(int j = 0;j<15;j++)
    {
        addFriend(QString("好友")+QString::number(j),j%2==0);
//        qDebug()<<QString("好友")+QString::number(j);
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
    if(e->type()==QEvent::MouseButtonPress&&object==ui->label_close)
    {
         close();
    }
    else if(e->type()==QEvent::MouseButtonPress&&object==ui->label_min){
        showMinimized();
        qDebug()<<"最小化";
    }



    return false;
}

void FriendList::addFriend(QString name, bool isOnline)
{

    int count = ui->listWidget->count();
    for(int i=0;i<count;i++)
    {
        if(QString::compare(ui->listWidget->item(i)->text(),name)==0)//存在用户，修改状态
        {
            if(isOnline)
            {
                ui->listWidget->item(i)->setForeground(QBrush(QColor(Qt::black)));
            }
            else
            {
                ui->listWidget->item(i)->setForeground(QBrush(QColor(Qt::lightGray)));
            }
            return ;
        }

    }

    //原本不存在某个用户

    QListWidgetItem * item = new QListWidgetItem(name);
    item->setFont(QFont("Microsoft YaHei Light",16));
    item->setBackground(QColor(226,225,228));
    if(isOnline)
    {
        item->setForeground(QBrush(QColor(Qt::black)));
    }
    else
    {
        item->setForeground(QBrush(QColor(Qt::lightGray)));
    }


    ui->listWidget->addItem(item);

}

void FriendList::on_listWidget_itemDoubleClicked(QListWidgetItem* item)
{
    qDebug()<<"点击了"<<item->text();
}
