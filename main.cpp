#include "socketmanager.h"
#include <QApplication>
socketManager *socketManagerW;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    socketManagerW=new socketManager;

    socketManagerW->topLogin->show();

//    GameGui w;
//    CreateGame w;
//    w.show();


    return a.exec();
}
