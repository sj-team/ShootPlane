#include "mainwindow.h"
#include "login.h"
#include "setting.h"
#include "friendlist.h"
#include "gamegui.h"
#include "creategame.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
   Login w;
    //FriendList w;
//    Setting w;
    //GameGui w;
   //CreateGame w;

    w.show();

    return a.exec();
}
