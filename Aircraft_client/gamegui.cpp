#include "gamegui.h"
#include "ui_gamegui.h"

#include "errbox.h"

#include "mytablewidget.h"

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
