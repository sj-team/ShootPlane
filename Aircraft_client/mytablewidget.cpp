#include "mytablewidget.h"

MyTableWidget::MyTableWidget(QWidget *parent) : QTableWidget(parent)
{
    this->setMouseTracking(true);

    this->setRowCount(10);
    this->setColumnCount(10);
    QStringList headerlabels;
    QStringList rowlabels;
    char a = 'A';
    for(int i=0;i<10;i++)
    {
        headerlabels<<QString::number(i);
        rowlabels<<QString(a+i);
    }

    this->setHorizontalHeaderLabels(headerlabels);
    this->setVerticalHeaderLabels(rowlabels);


    for(int i=0;i<10;i++)
    {
        this->setColumnWidth(i,2);
        this->setRowHeight(i,1);
    }

    num = 0;
    for(int i=0;i<10;i++)
        for(int j=0;j<10;j++)
        {
            map[i][j]=0;
            this->setItem(i,j,new QTableWidgetItem);
        }

    cur_oriented = Up;
    pre_airdata[0][0]=-1;

    this->setEditTriggers(QAbstractItemView::NoEditTriggers);    //禁止编辑表格
    this->setFocusPolicy(Qt::NoFocus);
    connect(this,SIGNAL(cellEntered(int,int)),this,SLOT(cellEntered(int,int)));
    connect(this,SIGNAL(cellClicked(int,int)),this,SLOT(cellClicked(int,int)));

}

void MyTableWidget::leaveEvent(QEvent *event)
{
    if(!changeAble)
        return;

    clearHover();
    pre_airdata[0][0]=-1;
}

//鼠标移动到某个cell中的事件
void MyTableWidget::cellEntered(int row, int column)
{
     clearSelection ();
    if(!changeAble)
        return;
    //将之前之前显示过的擦除
    clearHover();

    //渲染一个飞机
    airHover(row,column);

}

void MyTableWidget::cellClicked(int row,int column)
{
    clearSelection ();

    if(!changeAble)
        return;

    getAircraft(row,column,cur_oriented);

    if(isLegal()==0)
        return;

    if(num > 2)
        return;

    num++;

    for(int i=0;i<10;i++)
    {
       // qDebug()<<"cur_airdata"<<cur_airdata[i][0]<<cur_airdata[i][1];
        map[cur_airdata[i][0]][cur_airdata[i][1]] = num;
        item(cur_airdata[i][0],cur_airdata[i][1])->setBackgroundColor(airColor[num-1]);
    }


    item(cur_airdata[0][0],cur_airdata[0][1])->setText(QString::number(num)+"#");
    item(cur_airdata[0][0],cur_airdata[0][1])->setTextAlignment(Qt::AlignCenter);

    QString str = QString('A'+cur_airdata[0][0])+QString::number(cur_airdata[0][1])+
            '-'+QString('A'+cur_airdata[8][0])+QString::number(cur_airdata[8][1]);
    //qDebug()<<"table"<<str;

    emit send(str);

}

void MyTableWidget::setChangeAble(bool c)
{
    changeAble=c;
}

void MyTableWidget::setOriented(orient n)
{
    cur_oriented = n;
}

bool MyTableWidget::isLegal()
{

    for(int i=0;i<10;i++)
    {
        //检查是否能放置一个飞机
        if(cur_airdata[i][0]<0||cur_airdata[i][0]>9||cur_airdata[i][1]<0||cur_airdata[i][1]>9)
        {
            return false;
        }
        //检查是否存在飞机冲突
        if(map[cur_airdata[i][0]][cur_airdata[i][1]] != 0)
        {
            return false;
        }
    }
    return true;
}

void MyTableWidget::airHover(int row, int col)
{
    //根据机头和朝向获取一个飞机形状
    getAircraft(row,col,cur_oriented);

    //判断合法，包括相对map位置和冲突
    if(isLegal()==0)
        return;

    pre_oriented=cur_oriented;

    for(int i=0;i<10;i++)
    {
       // qDebug()<<"cur_airdata"<<cur_airdata[i][0]<<cur_airdata[i][1];
        pre_airdata[i][0]=cur_airdata[i][0];
        pre_airdata[i][1]=cur_airdata[i][1];
        item(cur_airdata[i][0],cur_airdata[i][1])->setBackgroundColor(hovered);
    }


}

void MyTableWidget::getAircraft(int x, int y, orient ori)
{
    if(ori == Left)
    {
        cur_airdata[0][0]=x;
        cur_airdata[0][1]=y;
        for(int i=1;i<6;i++)
        {
            cur_airdata[i][0] = x-3+i;
            cur_airdata[i][1] = y+1;
        }
        cur_airdata[6][0]=x;
        cur_airdata[6][1]=y+2;
        for(int i=7;i<10;i++)
        {
            cur_airdata[i][0]=x-8+i;
            cur_airdata[i][1]=y+3;
        }
        return;
    }
    if(ori == Right)
    {
        cur_airdata[0][0]=x;
        cur_airdata[0][1]=y;
        for(int i=1;i<6;i++)
        {
            cur_airdata[i][0] = x-3+i;
            cur_airdata[i][1] = y-1;
        }
        cur_airdata[6][0]=x;
        cur_airdata[6][1]=y-2;
        for(int i=7;i<10;i++)
        {
            cur_airdata[i][0]=x-8+i;
            cur_airdata[i][1]=y-3;
        }
        return;
    }
    if(ori == Up)
    {
        cur_airdata[0][0]=x;
        cur_airdata[0][1]=y;
        for(int i=1;i<6;i++)
        {
            cur_airdata[i][0] = x+1;
            cur_airdata[i][1] = y-3+i;
        }
        cur_airdata[6][0]=x+2;
        cur_airdata[6][1]=y;
        for(int i=7;i<10;i++)
        {
            cur_airdata[i][0]=x+3;
            cur_airdata[i][1]=y-8+i;
        }
        return;
    }
    if(ori == Down)
    {
        cur_airdata[0][0]=x;
        cur_airdata[0][1]=y;
        for(int i=1;i<6;i++)
        {
            cur_airdata[i][0] = x-1;
            cur_airdata[i][1] = y-3+i;
        }
        cur_airdata[6][0]=x-2;
        cur_airdata[6][1]=y;
        for(int i=7;i<10;i++)
        {
            cur_airdata[i][0]=x-3;
            cur_airdata[i][1]=y-8+i;
        }
        return;
    }
}

void MyTableWidget::clearHover()
{
    if(pre_airdata[0][0] != -1)
    {
        for(int i=0;i<10;i++)
        {
            if(map[pre_airdata[i][0]][pre_airdata[i][1]]==0)
            {
                //qDebug()<<"cur_airdata"<<cur_airdata[i][0]<<cur_airdata[i][1];
                item(pre_airdata[i][0],pre_airdata[i][1])->setBackgroundColor(bg);
            }

        }
    }
}

void MyTableWidget::clearAirCraft(QString str)
{
    //qDebug()<<str<<str.size();
    if(str.size()!=5)
        return;

    const char *p=str.toLocal8Bit().data();

    int x1 = p[0]-'A';
    int y1 = p[1]-'0';
    int x2 = p[3]-'A';
    int y2 = p[4]-'0';

    num--;

    if(x1==x2)
    {
        if(y1<y2)
            getAircraft(x1,y1,orient(Left));
        else
            getAircraft(x1,y1,orient(Right));
    }
    else
    {
        if(x1<x2)
            getAircraft(x1,y1,orient(Up));
        else
            getAircraft(x1,y1,orient(Down));

    }

    for(int i=0;i<10;i++)
    {
        map[cur_airdata[i][0]][cur_airdata[i][1]] = 0;
        item(cur_airdata[i][0],cur_airdata[i][1])->setBackgroundColor(bg);
    }

    item(cur_airdata[0][0],cur_airdata[0][1])->setText("");

}
