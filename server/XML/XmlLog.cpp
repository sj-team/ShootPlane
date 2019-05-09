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

newXmlLog :: newXmlLog(const char * _fileName)
{
    if (_fileName)
        fileName = _fileName;
    else
        fileName = "ActionLog.xml";

    id = 0;
    xmlTotal = new TiXmlElement("Action");
    doc.LinkEndChild(xmlTotal);
}

newXmlLog :: ~newXmlLog()
{
    doc.Clear();
}

void newXmlLog::writeLogin(const sockaddr_in & sockaddr , const char * name , const char * msg )
{
    gettimeofday(&timePresent, NULL); 
    strftime(timeBuf, sizeof(timeBuf)-1, "%Y-%m-%d %H:%M:%S", localtime(&timePresent.tv_sec));  
    
    TiXmlElement* xItem = new TiXmlElement("Action");
    xmlTotal ->LinkEndChild(xItem);
    xItem ->SetAttribute("id",itoa(id++));

    TiXmlElement *loginItem = new TiXmlElement("LoginAction");
    xItem ->LinkEndChild(loginItem);    

    TiXmlElement* xIp = new TiXmlElement("Ip");
    TiXmlElement* xUsername = new TiXmlElement("Username");
    TiXmlElement* xType = new TiXmlElement("Type");
    TiXmlElement* xTime = new TiXmlElement("Time");    


    setLinkNode(loginItem, xIp, inet_ntoa(sockaddr.sin_addr));
    setLinkNode(loginItem, xUsername, name);
    setLinkNode(loginItem, xType, msg);
    setLinkNode(loginItem, xTime, timeBuf);

    doc.SaveFile(fileName.c_str());
   
}

void newXmlLog::writeLogin(const ClientInfo & cinfo , const char * msg )
{

    writeLogin(cinfo.sockaddr , cinfo.name.c_str() , msg );

}

void newXmlLog::writeConnect (const ClientInfo & client , const ClientInfo &oppo_client , const char * msg )
{
    gettimeofday(&timePresent, NULL); 
    strftime(timeBuf, sizeof(timeBuf)-1, "%Y-%m-%d %H:%M:%S", localtime(&timePresent.tv_sec));  
    
    TiXmlElement* xItem = new TiXmlElement("Action");
    xmlTotal ->LinkEndChild(xItem);
    xItem ->SetAttribute("id",itoa(id++));
    TiXmlElement *connectItem = new TiXmlElement("ConnectGameAction");
    xItem ->LinkEndChild(connectItem);  

    TiXmlElement* xSndIP = new TiXmlElement("SendIP");
    TiXmlElement* xSndUsername = new TiXmlElement("xSndUsername");
    TiXmlElement* xRecvIP = new TiXmlElement("RecieveIP");
    TiXmlElement* xRecvUsername = new TiXmlElement("xRecvUsername");
    TiXmlElement* xType = new TiXmlElement("Type");
    TiXmlElement* xTime = new TiXmlElement("Time");
  

    setLinkNode(connectItem, xSndIP, inet_ntoa(client.sockaddr.sin_addr));
    setLinkNode(connectItem, xSndUsername, client.name.c_str());
    setLinkNode(connectItem, xRecvIP, inet_ntoa(oppo_client.sockaddr.sin_addr));
    setLinkNode(connectItem, xRecvUsername, oppo_client.name.c_str());
    setLinkNode(connectItem, xType, msg);
    setLinkNode(connectItem, xTime, timeBuf);
    
    doc.SaveFile(fileName.c_str());

}

void newXmlLog::writeGame(const ClientInfo & client , const ClientInfo & oppo_client , const Packet & packet)
{
    gettimeofday(&timePresent, NULL); 
    strftime(timeBuf, sizeof(timeBuf)-1, "%Y-%m-%d %H:%M:%S", localtime(&timePresent.tv_sec));  
    
    TiXmlElement* xItem = new TiXmlElement("Action");
    xmlTotal ->LinkEndChild(xItem);
    xItem ->SetAttribute("id",itoa(id++));
    TiXmlElement *gameItem = new TiXmlElement("PlayGameAction");
    xItem ->LinkEndChild(gameItem);  

    TiXmlElement* xSndIP = new TiXmlElement("SendIP");
    TiXmlElement* xSndUsername = new TiXmlElement("xSndUsername");
    TiXmlElement* xRecvIP = new TiXmlElement("RecieveIP");
    TiXmlElement* xRecvUsername = new TiXmlElement("xRecvUsername");
    TiXmlElement* xResult = new TiXmlElement("xResult");
    TiXmlElement* xType = new TiXmlElement("Type");
    TiXmlElement* xTime = new TiXmlElement("Time");
  

    setLinkNode(gameItem, xSndIP, inet_ntoa(client.sockaddr.sin_addr));
    setLinkNode(gameItem, xSndUsername, client.name.c_str());
    setLinkNode(gameItem, xRecvIP, inet_ntoa(oppo_client.sockaddr.sin_addr));
    setLinkNode(gameItem, xRecvUsername, oppo_client.name.c_str());

    const char * msg ="";
    if(!packet.isMainType(mt::resPlay))
        return ;
    if(packet.isSubType(sbt::unmask)){
        unmaskPointResult * msg_packet = (unmaskPointResult *) packet.msg;
        char pos_msg [32] ;
        const char * result = "";
        sprintf(pos_msg , "Point:%c%d",'A'+msg_packet->x,msg_packet->y);
        setLinkNode(gameItem, xType, pos_msg);
        if(msg_packet->result==bs::head)
            result ="head";
        else if (msg_packet->result==bs::body)
            result="body";
        else if (msg_packet->result==bs::empty)
            result="empty";
        sprintf(pos_msg , "Result:%s,Score:%d",result,msg_packet->score);
        setLinkNode(gameItem, xResult, pos_msg);
    }
    else if (packet.isSubType(sbt::locate)){
        unmaskLocateResult * msg_packet = (unmaskLocateResult *) packet.msg;
        char pos_msg [32] ;
        const char * result = "";
        sprintf(pos_msg , "Location:%c%d-%c%d",'A'+msg_packet->x1,msg_packet->y1,'A'+msg_packet->x2,msg_packet->y2);
        setLinkNode(gameItem, xType, pos_msg);
        if(msg_packet->result)
            result ="Bingo";
        else 
            result="Miss";
        sprintf(pos_msg , "Result:%s,Score:%d",result,msg_packet->score);
        setLinkNode(gameItem, xResult, pos_msg);
    }
    else if (packet.isSubType(sbt::surrender)){
        setLinkNode(gameItem, xType, "Surrender");
    }

    setLinkNode(gameItem, xTime, timeBuf);

    doc.SaveFile(fileName.c_str());

}
