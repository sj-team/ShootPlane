#include "XmlLog.h"

using namespace std;

int main()
{
    XmlLog log;

    Packet packet;
    packet.header.subType = sbt::request;

    ClientInfo c1("jiahaolin"), c2("menglingchen");
    c1.sockaddr.sin_addr.s_addr = inet_addr("192.168.80.230");
    c2.sockaddr.sin_addr.s_addr = inet_addr("192.168.80.111");

    log.writeNorm(&c1, NULL, &packet);
    sleep(1);
    c1.name = "wanghan";
    log.writeNorm(&c1, NULL, &packet);
    c1.sockaddr.sin_addr.s_addr = inet_addr("10.60.17.56");
    packet.header.subType = sbt::idNotExit;
    sleep(1);
    log.writeNorm(&c1, &c2, &packet);
    c2.name = "xiaowei";
    sleep(1);
    log.writeNorm(&c1, NULL, &packet);
    log.writeNorm(&c1, &c2, &packet);
    packet.header.subType = sbt::tellOffline;
    sleep(1);

    log.saveLog();

    return 0;
}