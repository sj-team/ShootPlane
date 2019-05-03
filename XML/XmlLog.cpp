#include "XmlLog.h"


static void setText(TiXmlElement*& element, const char* str)
{
    TiXmlText* text = new TiXmlText(str);
    element->LinkEndChild(text);
}

static void setLinkNode(TiXmlElement*& father,TiXmlElement*& child, const char*str)
{
    father->LinkEndChild(child);
    setText(child, str);
}

static char* itoa(int id)
{
    char *tmp = new char[32];
    sprintf(tmp,"%d",id);
    return tmp;
}

XLLogin::XLLogin()
{
    id = 1;
}

XLLogin::~XLLogin()
{
    doc.Clear();
}

void XLLogin::setLogin()
{
    xXLLogin = new TiXmlElement("Login");
    doc.LinkEndChild(xXLLogin);
}

void XLLogin::setItem(const logBuf &buffer)
{
    TiXmlElement* xItem = new TiXmlElement("Item");
    xXLLogin->LinkEndChild(xItem);
    xItem->SetAttribute("id", itoa(id++));

    TiXmlElement* xIp = new TiXmlElement("Ip");
    TiXmlElement* xUsername = new TiXmlElement("Username");
    TiXmlElement* xType = new TiXmlElement("Type");
    TiXmlElement* xTime = new TiXmlElement("Time");

    setLinkNode(xItem, xIp, buffer.ip);

    setLinkNode(xItem, xUsername, buffer.username);

    setLinkNode(xItem, xType, buffer.type);

    setLinkNode(xItem, xTime, buffer.time);
}





XLDataTransform::XLDataTransform()
{
    id = 1;
}

XLDataTransform::~XLDataTransform()
{
    doc.Clear();
}

void XLDataTransform::setDataTransform()
{
    xXLDataTransform = new TiXmlElement("DataTransform");
    doc.LinkEndChild(xXLDataTransform);
}

void XLDataTransform::setItem(const dataBuf &buffer)
{
    TiXmlElement* xItem = new TiXmlElement("Item");
    xXLDataTransform->LinkEndChild(xItem);
    xItem->SetAttribute("id", itoa(id++));

    TiXmlElement* xSndIP = new TiXmlElement("SendIP");
    TiXmlElement* xSndUsername = new TiXmlElement("xSndUsername");
    TiXmlElement* xRecvIP = new TiXmlElement("RecieveIP");
    TiXmlElement* xRecvUsername = new TiXmlElement("xRecvUsername");
    TiXmlElement* xType = new TiXmlElement("Type");
    TiXmlElement* xTime = new TiXmlElement("Time");

    setLinkNode(xItem, xSndIP, buffer.sndIp);

    setLinkNode(xItem, xSndUsername, buffer.sndUsername);

    setLinkNode(xItem, xRecvIP, buffer.recvIp);

    setLinkNode(xItem, xRecvUsername, buffer.recvUsername);

    setLinkNode(xItem, xType, buffer.type);

    setLinkNode(xItem, xTime, buffer.time);
}






XmlLog::XmlLog()
{
    xlLogin = new XLLogin;
    xlLogin->setLogin();
    xlDataTransform = new XLDataTransform;
    xlDataTransform->setDataTransform();
    initMap();
}

XmlLog::~XmlLog()
{
    delete xlLogin;
    delete xlDataTransform;
    mapType.clear();
}

void XmlLog::_writeLogin(const unsigned char subType, const struct sockaddr_in &sockAddr, const char* username)
{
    gettimeofday(&timePresent, NULL);  
    strftime(timeBuf, sizeof(timeBuf)-1, "%Y-%m-%d %H:%M:%S", localtime(&timePresent.tv_sec));  

    logBuf buf;
    strcpy(buf.ip, inet_ntoa(sockAddr.sin_addr));
    strcpy(buf.username, username);
    strcpy(buf.type, mapType[subType].c_str());
    strcpy(buf.time, timeBuf);
    strcat(buf.time, "s");
    
    xlLogin->setItem(buf);
    saveLog();

}

void XmlLog::_writeLogin(const char * msg , const struct sockaddr_in &sockAddr, const char* username)
{
    cout <<"-------------"<<msg<<endl;
    gettimeofday(&timePresent, NULL);  
    strftime(timeBuf, sizeof(timeBuf)-1, "%Y-%m-%d %H:%M:%S", localtime(&timePresent.tv_sec));  

    logBuf buf;

/*     strcpy(buf.ip, "i.sin_addr)");
    strcpy(buf.username, "username");
    strcpy(buf.type, "msg");
    strcpy(buf.time, "timeBuf");
    strcat(buf.time, "s"); */


    strcpy(buf.ip, inet_ntoa(sockAddr.sin_addr));
    strcpy(buf.username, username);
    strcpy(buf.type, msg);
    strcpy(buf.time, timeBuf);
    strcat(buf.time, "s");
    
    xlLogin->setItem(buf);
    saveLog();
    cout <<"-------------end "<<endl;
}

void XmlLog::_writeDataTransform(const unsigned char subType, const struct sockaddr_in &sndSock, const struct sockaddr_in &recvSock, const char* sndUsername, const char* recvUsername)
{
    gettimeofday(&timePresent, NULL);  
    strftime(timeBuf, sizeof(timeBuf)-1, "%Y-%m-%d %H:%M:%S", localtime(&timePresent.tv_sec));  

    dataBuf buf;
    strcpy(buf.sndIp, inet_ntoa(sndSock.sin_addr));
    strcpy(buf.sndUsername, sndUsername);
    strcpy(buf.recvIp, inet_ntoa(recvSock.sin_addr));
    strcpy(buf.recvUsername, recvUsername);
    strcpy(buf.type, mapType[subType].c_str());
    strcpy(buf.time, timeBuf);
    strcat(buf.time, "s");
    
    xlDataTransform->setItem(buf); 
    saveLog();
}


void XmlLog::_writeDataTransform(const  char * msg, const struct sockaddr_in &sndSock, const struct sockaddr_in &recvSock, const char* sndUsername, const char* recvUsername)
{
    gettimeofday(&timePresent, NULL);  
    strftime(timeBuf, sizeof(timeBuf)-1, "%Y-%m-%d %H:%M:%S", localtime(&timePresent.tv_sec));  

    dataBuf buf;
    strcpy(buf.sndIp, inet_ntoa(sndSock.sin_addr));
    strcpy(buf.sndUsername, sndUsername);
    strcpy(buf.recvIp, inet_ntoa(recvSock.sin_addr));
    strcpy(buf.recvUsername, recvUsername);
    strcpy(buf.type, msg);
    strcpy(buf.time, timeBuf);
    strcat(buf.time, "s");
    
    xlDataTransform->setItem(buf); 
    saveLog();
}


void XmlLog::writeNorm(const ClientInfo* sndClient, const ClientInfo* recvClient, const Packet* pack)
{
    if(recvClient == NULL)
        _writeLogin(pack->header.subType, sndClient->sockaddr, sndClient->name.c_str());
    else{
        _writeDataTransform(pack->header.subType, sndClient->sockaddr, recvClient->sockaddr, sndClient->name.c_str(), recvClient->name.c_str());
    }
}

void XmlLog::writeError(const ClientInfo* sndClient, unsigned char erroType)
{

}

bool XmlLog::saveLog()
{
    bool flag1 = xlLogin->doc.SaveFile("log_in.xml");

    bool flag2 = xlDataTransform->doc.SaveFile("data_transform.xml");

    if(flag1 && flag2 )
        return true;
    return false;
}

void XmlLog::initMap()
{
    mapType[sbt::failed] = "验证失败";
    mapType[sbt::success] = "验证成功";
    mapType[sbt::pwderror] = "密码错误";
    mapType[sbt::repeaton] = "新上线重复登陆";
    mapType[sbt::repeatoff] = "强制下线重复登陆";
    //mapType[sbt::sndTxt] = "发送文本";
    mapType[sbt::file] = "发送文件";

    mapType[sbt::friList] = "未定义mapType";
    mapType[sbt::hisNum] = "未定义mapType";
    mapType[sbt::myDefault] = "未定义mapType";
    mapType[sbt::tellOffline] = "未定义mapType";
    mapType[sbt::tellOnline] = "未定义mapType";
    mapType[sbt::winTheme] = "未定义mapType";
}