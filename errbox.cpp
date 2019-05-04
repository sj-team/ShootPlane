#include "errbox.h"
#include "ui_errbox.h"

ErrBox::ErrBox(const QString data,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ErrBox)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose,true);

    ui->label->setText("  ERROR: "+data);
    qDebug()<<"ERROR: "+data;

    //setWindowFlags(Qt::FramelessWindowHint| Qt::WindowSystemMenuHint);
    //ui->label_close->installEventFilter(this);

//    this->setAutoFillBackground(true); // 这句要加上, 否则可能显示不出背景图.
//    QPalette palette = this->palette();
//    QPixmap pix;

//    //qDebug()<<"begin"<<QDir::currentPath()+"/img/login_bg.jpg";
//    if(pix.load(QDir::currentPath()+"/img/login_bg.jpg"))
//    {
//        palette.setBrush(QPalette::Window,
//                         QBrush(pix.scaled( // 缩放背景图.
//                                 this->size(),
//                                Qt::IgnoreAspectRatio,
//                                Qt::SmoothTransformation))); // 使用平滑的缩放方式
//       this->setPalette(palette); // 至此, 已给widget加上了背景图.
//        //qDebug()<<"success"<<QDir::currentPath()+"/img/login_bg.jpg";
//    }
//    else {
//        qDebug()<<"failed"<<QDir::currentPath()+"/img/login_bg.jpg";
//    }
}

ErrBox::~ErrBox()
{
    delete ui;
}

bool ErrBox::eventFilter(QObject *object, QEvent *e)
{
    if(e->type()==QEvent::MouseButtonPress&&object==ui->buttonBox)
    {
         close();
    }
    return false;
}
