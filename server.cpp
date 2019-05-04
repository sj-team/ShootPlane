
#include "server.h"


const bool g_offline_snd = true ;

bool debug = false;

/* TODO

1. 
is
*/

Server::Server()
{
    clientList.clear();
    loginList.clear();
    nameIndex.clear();
    cfdIndex.clear();
    gameList.clear();

    dataBase = new SERVER_MYSQL();

    serverInit();

}

Server::~Server()
{
    clientList.clear();
    loginList.clear();
    nameIndex.clear();
    cfdIndex.clear();
    delete dataBase ;



}


void Server::initClientSetup()
{
    vector <string > nameList ; 

    dataBase->get_userlist(nameList);
    //  sql  
    int count = 0 ;

    for (auto i =0 ; i<nameList.size();i++)
    {
        clientList.push_back(ClientInfo(nameList[i]));
        nameIndex.insert(make_pair(nameList[i],i));
    }

    cout << "Total user " << nameList.size()<<endl;

    return ;
}



void Server::serverInit()
{
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		myExit();

	max_fd = server_fd;
	//memset(clientList, 0, sizeof(clientList));
	FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);

	FD_SET(server_fd, &read_fds);

    setReusePort(server_fd );

	if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
		myExit();

	if (listen(server_fd, MAX_LISTEN)==-1)
		myExit();

	
	setNonBlock(server_fd);



    // 客户端配置初始化
    initClientSetup();
}


void Server::close_cfd(int cfd )
{
    auto p = cfdIndex.find(cfd);
    if(p != cfdIndex.end())
    {
        clientList[p->second].cfd = -1 ;
        cout <<"tell off line "<<endl;
        cfdIndex.erase(p);
        user_offline( p->second );
        
    }
    
    FD_CLR( cfd , &read_fds);
    FD_CLR (cfd , &write_fds);
    close(cfd);  
}

void Server::add_cfd (int cfd )
{
    FD_SET(cfd, &read_fds);
	FD_SET(cfd, &write_fds);
    max_fd = max_fd > cfd ? max_fd : cfd;
}

// 不成功删除 loginList
void Server::removeLogin (vector<loginAction>::iterator i)
{    
    //close_cfd (loginList[i].cfd);
    //loginList.erate(loginList.begin()+i);
    close_cfd(i->cfd);
    loginList.erase(i);
    cout <<"erase ok !\n";
}

void Server::removeGame( int gid )
{
    auto iter = gameList.begin() + gid ;

    if(! iter->ready ){
        sndResponse(clientList[iter->index1].cfd,mt::resConnect,sbt::deny);
        sndResponse(clientList[iter->index2].cfd,mt::connect,sbt::deny);
    }

    clientList[iter->index1].gameId = -1;
    clientList[iter->index2].gameId = -1 ;
    if (iter->board1)
        delete iter->board1 ;
    if (iter->board2)
        delete iter->board2 ;
    clientList[iter->index1].my_board = clientList[iter->index1].oppo_board = NULL ;
    clientList[iter->index2].my_board = clientList[iter->index2].oppo_board = NULL ;
    gameList.erase(iter);
    
}

void Server::gameTurn(int gid )
{
    auto iter = gameList.begin() + gid ; 

    if ( iter->turn){
        sndResponse(clientList[iter->index1].cfd , mt::askGame , sbt::turn);
        sndResponse(clientList[iter->index2].cfd , mt::askGame , sbt::wait);
       // send
    }
    else{
        sndResponse(clientList[iter->index1].cfd , mt::askGame , sbt::wait);
        sndResponse(clientList[iter->index2].cfd , mt::askGame , sbt::turn);
    }

    //if (turn == (id == index1))
}


// judge whether game is over or not 
// if end game , send msg to palyer 
void Server::endGame( int gid , bool turn)
{

    // TODO  : tell result ;

    removeGame(gid);
}

void Server::user_online ( int cfdindex )
{

    auto i = loginList.begin() + cfdindex ;
    cfdIndex.insert(make_pair( i->cfd , i->index));

    int index = i->index ;
    int cfd = i->cfd ; 

    clientList[index].cfd = cfd ;
    clientList[index].sockaddr = i->sockaddr;
    clientList[index].status = cstate::online;
    tell_clinet_onoffline (index , true);
    string name = clientList[index].name ;
    loginList.erase(i);



    // 好友列表
    string friMsg ;
    Packet packet ;
    cout << "get-friList"<<endl;
    mylog._writeLogin("get-friList",clientList[index].sockaddr,clientList[index].name.c_str());
    for ( auto p = clientList.begin();p!=clientList.end();p++)
    {
        if(p->cfd == cfd )
            continue ;
        if(friMsg.length() + 2 + MAXNAMELEN >= MAXDATALEN)
        {
            packet.fillPacket(mt::resConf,sbt::friList,friMsg.c_str(),friMsg.length());
            serverSend(cfd , packet);
            friMsg = "";
        }
        if(p->cfd  <0 )
            friMsg +=cstate::offline;
        else
            friMsg +=cstate::online;

        friMsg +=p->name;
        friMsg +='|';

    }

    if(1)
    {
        packet.fillPacket(mt::resConf, sbt::friList, friMsg.c_str(), friMsg.length());
        serverSend(cfd, packet);
        friMsg = "";
    }
    
    // 窗口颜色

    int color = dataBase->get_color(name.c_str() );

    mylog._writeLogin("set-themecolor",clientList[index].sockaddr,clientList[index].name.c_str());

    if(color == -1 )
        color = 10 ; 
    // send color packet 
    if(1)
    {
        packet.fillPacket(mt::resConf , sbt::winTheme , &color , sizeof(color));
        serverSend(cfd , packet );
    }

    int fontcolor = dataBase->get_fontcolor(name.c_str());
    if(fontcolor == -1 )    
        fontcolor = 10 ;
    if(1)
    {
        packet.fillPacket(mt::resConf , sbt::fontColor , &fontcolor , sizeof(fontcolor));
        serverSend(cfd , packet );
    }

    int fontSize = dataBase->get_fontsize(name.c_str());
    if(fontSize == -1 )
        fontSize = 20;

    if(1)
    {
        packet.fillPacket(mt::resConf , sbt::fontSize , &fontcolor , sizeof(fontcolor));
        serverSend(cfd , packet );
    }

    


        //离线重传
    if(g_offline_snd)
    {
        int i = 0;
        for(auto pack = clientList[index].offlinePacks.begin(); pack!=clientList[index].offlinePacks.end() ; pack ++ , i++)
        {
            serverSend(cfd , *pack);
            
            int sndIndex = cfdIndex[cfd];
            int rcvIndex = nameIndex[(*pack).msg];

            mylog.writeNorm(&clientList[sndIndex],&clientList[rcvIndex], & (clientList[index].offlinePacks[i]));
        }
        clientList[index].offlinePacks.clear();
    }

    //TODO  conf friList 


}

void Server::user_leave( int index )
{
    close_cfd (clientList[index].cfd);
    clientList[index].cfd = -1 ;
}

void Server::user_offline( int index )
{
    if (clientList[index].gameId !=-1)
        removeGame(clientList[index].gameId);

    clientList[index].status = cstate::offline;
    tell_clinet_onoffline(index , false);

    close_cfd (clientList[index].cfd);
    clientList[index].cfd = -1 ;

}


// index 是 loginList 的index
void Server::solveLogin(int index )
{
    auto i = loginList.begin() + index ;
    int cfd = i->cfd ; 
    Packet p ;


    if(serverRecv(cfd , p )==-1)
    {
        removeLogin (i);
        return ;
    }

    //  如果不是login类型  则错误
    if(!p.isMainType(mt::login))
    {
        cout <<"not login "<<endl ;
        mylog._writeLogin( "login-type-error", i->sockaddr , i->username.c_str());
        removeLogin(i);
        return ;
    }
    if(i->state == sbt::request &&p.isSubType(sbt::request))
    {
        loginData * datap = (loginData *) p.msg;


        cout <<"username and passwd "<<endl;
        cout <<datap->username <<endl ;
        cout << datap->passwd <<endl;


        //  用户名不存在
        if(nameIndex.find(datap->username)==nameIndex.end())
        {
            cout <<"not exit user name  "<<endl ;
            sndResponse(cfd , mt::resLogin, sbt::idNotExit);
            mylog._writeLogin( "username-not-exsit", i->sockaddr , i->username.c_str());
            removeLogin(i);
            return ;
        }

        // 密码错误
        if(dataBase->check_user(datap->username , datap->passwd) <= 0)
        {
            cout <<"not correct passwd  "<<endl ;
            sndResponse(cfd , mt::resLogin,sbt::pwderror);
            removeLogin(i);
            mylog._writeLogin( "passwd-uncorrect" , i->sockaddr , i->username.c_str());
            return ;
        }
        // 
        else 
        {
            cout <<"passwd ok "<<endl ;
            i->username = datap->username ;
            i->index = nameIndex[i->username];

            if(dataBase->need_set_passwd(datap->username))
            {
                cout << "change passwd "<<endl;
                i->state = sbt::changepwd;
                sndResponse(cfd , mt::resLogin , sbt::changepwd);
                mylog._writeLogin( "change-passwd" , i->sockaddr , i->username.c_str());
                return ;
            }
            else if ( clientList[i->index].cfd >0)
            {
                // debug
                cout << i->username <<"重复登录"<<endl;
                mylog._writeLogin( "login-repeat-off" , clientList[i->index].sockaddr , i->username.c_str());
                sndResponse(clientList[i->index].cfd , mt::login , sbt::repeatoff);
                close_cfd (clientList[i->index].cfd);
                clientList[i->index].cfd = -1 ;

                mylog._writeLogin( "login-repeat-on" , i->sockaddr , i->username.c_str());
                sndResponse(cfd , mt::resLogin , sbt::repeaton);
                i->state = sbt::success;

            }
            else 
            {
                i->state = sbt::success;
                sndResponse(cfd , mt::resLogin , sbt::success);
                mylog._writeLogin( "login-success" , i->sockaddr , i->username.c_str());
            }

        }
    }
    else if (i->state == sbt::changepwd && p.isSubType(sbt::changepwd))
    {
        changePwdData * datap = (changePwdData *)p.msg ;
        cout <<"更改密码为"<<datap->newPasswd<<endl;
        if(dataBase->set_passwd(i->username.c_str() , datap->newPasswd)==-1 )
        {
            mylog._writeLogin( "change-passwd-error" , i->sockaddr , i->username.c_str());
            sndResponse(cfd , mt::resLogin,sbt::pwdChangeErr);
            removeLogin(i);
            return ;
        }
        else 
        {
            mylog._writeLogin( "change-passwd-ok" , i->sockaddr , i->username.c_str());
            sndResponse(cfd , mt::resLogin , sbt::success );
            i->state = sbt::success;
        }

    }
    else if (i->state == sbt::success && p.isSubType(sbt::success))
    {
        mylog._writeLogin( "client-on" , i->sockaddr , i->username.c_str());
        cout <<"success login "<<endl;
        //clientList[i->index].cfd = cfd ;
        user_online ( index );
        //loginList.erase(i);
    }

    cout <<"state "<<hex <<(int) i->state <<' '<< endl ;
    return ;

}

void Server::solveMsg(const int index )
{

    Packet srcPacket , desPacket ;
    
    int cfd = clientList[index].cfd;
    const char * fromName = clientList[index].name.c_str() ;

    if(serverRecv( cfd , srcPacket) <0)
    {
        return ;
    }


    // we nben xiao xi qun fa
    if(srcPacket.isGroupSnd() || srcPacket.isType(mt::sndTxt , sbt::groupChat))
    {
        cout <<"sovle msg group snd "<<endl ; 
        vector <string> cfdList ; 


        if(srcPacket.isType(mt::sndTxt , sbt::groupChat))
        {
            for(auto j = clientList.begin();j!=clientList.end();j++)
            {
                if(j->cfd <0 || j->cfd == cfd )
                    continue;
                cfdList.push_back(j->name);
            }
        }
        else
        {
            int num = srcPacket.header.subType;
            for (int i = 0; i < num; i++)
            {
                cfdList.push_back(srcPacket.msg + i * 32);
            }
        }


        alterTxtPack(desPacket , srcPacket , fromName );

        for(auto i = cfdList.begin();i!=cfdList.end();i++)
        {

            sndOneMsg(index , (*i).c_str(), desPacket );
        }
        


    }
    else if (srcPacket.isMainType(mt::conf))
    {
        
        switch (srcPacket.header.subType)
        {
            case (sbt::winTheme):
            {
                int value ;
                memcpy(&value , srcPacket.msg , 4 );

                dataBase -> set_color(clientList[index].name.c_str() , value );

                cout <<"set winTheme !"<<endl;

                break;

            }
                //int setColor = 
                //dataBase->set_settings( clientList[index].name , )
        
            case (sbt::hisNum):
            {
                int value ;
                memcpy(&value , srcPacket.msg , 4 );
                cout << "set hisNum "<<endl;
                dataBase -> set_msgnum(clientList[index].name.c_str() , value );
                break ;
            }

            case (sbt::hisMsg):
            {
                vector<string> hisMsgs;
                const char *srcName = clientList[index].name.c_str();
                const char *desName = srcPacket.msg ;

                int num;
                
                num = dataBase->get_msgnum(clientList[index].name.c_str());

                if(num <=0)
                {
                    cout <<"default is null "<<endl;
                    num = 100 ;
                }

                cout <<"num = "<<num<<endl;

                int totalNum = dataBase->get_msglist(srcName, desName, num, hisMsgs);


                string msgbuff = "";

                for (int i = 0; i < hisMsgs.size() && i < num; i++)
                {
                    if (msgbuff.length() + hisMsgs[i].length() +10  >= MAXDATALEN)
                    {
                        desPacket.fillPacket(mt::resConf, sbt::hisMsg, msgbuff.c_str(), msgbuff.length());
                        serverSend(cfd, desPacket);
                        msgbuff = "";
                    }
                    msgbuff += hisMsgs[i];
                    msgbuff += '|';
                }

                if (1)
                {
                    desPacket.fillPacket(mt::resConf, sbt::hisMsg, msgbuff.c_str(), msgbuff.length());
                    serverSend(cfd, desPacket);
                    msgbuff = "";
                }

                break;
            }

            case (sbt::fontColor):
            {
                int value ;
                memcpy(&value , srcPacket.msg , 4 );
                cout << "set font color "<<endl;
                dataBase -> set_fontcolor(clientList[index].name.c_str() , value );
                break ;
            }

            case (sbt::fontSize):
             {
                int value ;
                memcpy(&value , srcPacket.msg , 4 );
                cout << "set font size "<<endl;
                dataBase -> set_fontsize(clientList[index].name.c_str() , value );
                break ;
             }
            
            default:
                break;
        }
    }

    // solve start a game 
    else if (srcPacket.isMainType(mt::connect)){
        
        if(srcPacket.isSubType(sbt::request)){
            string oppo_name (srcPacket.msg);
        

            int gid = gameList.size();


            if(nameIndex.find(oppo_name)==nameIndex.end())
            {
                cout <<"name not exit "<<endl;
                sndResponse(clientList[index].cfd , mt::resSend , sbt::idNotExit , oppo_name.c_str() );
                return ;
            }

            int oppo_index = nameIndex[oppo_name];
            
            if(clientList[oppo_index].status == cstate::playing){
                cout <<"can't  start a game | playing "<<endl;
                sndResponse(clientList[index].cfd,mt::resSend,sbt::idPlaying);
                return ;
            }
            if (clientList[oppo_index].status == cstate::offline){
                cout <<"can't  start a game | offline "<<endl;
                sndResponse(clientList[index].cfd,mt::resSend,sbt::idOffline);
                return ;
            }
            
            gameList.push_back(gameInfo(index , oppo_index ));
            clientList[index].gameId = gid ;
            clientList[oppo_index].gameId = gid ; 

            
            sndResponse(clientList[oppo_index].cfd,mt::connect,sbt::request,clientList[index].name.c_str());
        }

        else if (srcPacket.isSubType(sbt::deny)){
            int gid = clientList[index].gameId;
            int oppo_index = gameList[gid].index2 ;
            removeGame(gid);

            sndResponse(clientList[oppo_index].cfd, mt::resConnect, sbt::deny);            
        }
        else if (srcPacket.isSubType(sbt::success)){
            
            int gid = clientList[index].gameId ;
            
            int cfd1 = clientList[gameList[gid].index1].cfd ;
            int cfd2 = clientList[gameList[gid].index2].cfd;

            auto iter = gameList.begin() + gid ;
            iter ->board1 = new ChessBoard(iter->index1);
            iter ->board2 = new ChessBoard(iter->index2);
            clientList[iter->index1].my_board = clientList[iter->index2].oppo_board= iter->board1 ;
            clientList[iter->index1].oppo_board = clientList[iter->index2].my_board= iter->board2;

            sndResponse(cfd1, mt::askGame, sbt::locate);
            sndResponse(cfd2, mt::askGame, sbt::locate);
            
        }
                        
    }

    else if (srcPacket.isMainType(mt::resConnect)){
        string oppo_name (srcPacket.msg);
        int oppo_index = nameIndex[oppo_name];
        if (srcPacket.isSubType(sbt::accept)){
            
            sndResponse(clientList[oppo_index].cfd, mt::resConnect, sbt::accept);
        }
        else if (srcPacket.isSubType(sbt::deny)){

            removeGame(clientList[index].gameId);
            sndResponse(clientList[oppo_index].cfd, mt::resConnect, sbt::deny);
        }
    }

    // TODO
    else if (srcPacket.isMainType(mt::init)){
        if (srcPacket.isSubType(sbt::locate)){
            
            int gid = clientList[index].gameId;
            
        
            ChessBoard * board ; 
            if (index == gameList[gid].index1){
                board = gameList[gid].board1 = new ChessBoard(index );
            }
            else if (index == gameList[gid].index2){
                board = gameList[gid].board2 = new ChessBoard(index);
            }
            // TODO : new board 
            bool flag = true ; 

            if (flag = false){
                
            }



            // 成功设了3架飞机
            if(gameList[gid].board1->getnum() == g_plane_num && gameList[gid].board2->getnum()== g_plane_num){
                gameList[gid].ready = true ;
                gameList[gid].turn = true ; 

                gameTurn(gid);
            }
        }
    
    }

    // TODO
    else if (srcPacket.isMainType(mt::play)){

        bool flag = true ;
        int gid = clientList[index].gameId ; 
        if (srcPacket.isSubType(sbt::unmask)){

        }
        else if (srcPacket.isSubType(sbt::locate)){

        }

        if (flag){

            gameList[gid].turn = !gameList[gid].turn;
            gameTurn(gid);
        }

    }


    //else if(srcPacket.isMainType(mt::sndTxt)||srcPacket.isMainType(mt::sndFile)||srcPacket.isMainType(mt::sndFileHead)||srcPacket.isMainType(mt::resFileHead))
    else
    {
        //cout <<"--------sovle msg  snd "<<endl ;
        char rcvName [32];
        memcpy(rcvName , srcPacket.msg,32);
        alterTxtPack(desPacket , srcPacket , fromName );
        //memcpy(srcPacket.msg, fromName , 32);
        sndOneMsg(index , rcvName , desPacket);
        

    }


/*     else 
    {
        mylog._writeLogin( "parsing-header-error" , clientList[index].sockaddr , clientList[index].name.c_str());   
        cout <<"parsing packet header type error !"<<endl ;
        sleep(1);
    } */

    return ;
    


}

void Server::tell_clinet_onoffline (int index ,bool isOnline)
{
    int cfd = clientList[index].cfd ;
    const char * name = clientList[index].name.c_str();
    Packet packet ;
    
    //strcpy(packet.msg , name );
    int len = strlen(name)+1 ;

    if(isOnline)
        packet.fillPacket(mt::updateList,sbt::tellOnline, name ,len);
    else   
        packet.fillPacket(mt::updateList,sbt::tellOffline, name ,len);

    for(auto i = clientList.begin() ; i!=clientList.end();i++)
    {
        if(i->cfd <0 || i->cfd ==cfd)
            continue ;
        serverSend(i->cfd, packet);
        cout << "told cfd "<<i->cfd <<endl;
    }

}


bool Server::newConnect()
{
	struct sockaddr_in client_addr;
	socklen_t len_client_addr = sizeof(client_addr);

	int cfd = accept(server_fd, (struct sockaddr *)&client_addr, &len_client_addr);
    if(cfd == -1){
        cerr<<"beyond ablility !\n";
        return false;
    }

    loginList.push_back(loginAction(cfd,client_addr));



    cout <<"accpet ok !\n"<<endl;

    setNonBlock(cfd);

    add_cfd(cfd);

	return true ;
}


void Server::run()
{

	// create_daemon();

	timeval wait_time;

    fd_set rfd_cpy , wfd_cpy ;

    while (true) {
        setTime(wait_time, 1,0);
        rfd_cpy = read_fds;
        wfd_cpy = write_fds ;


        switch (select(max_fd + 1, &rfd_cpy, NULL, NULL, &wait_time))
        {
            case -1:
                cerr<<"select error ! "<<strerror(errno)<<"max ="<<max_fd<<endl;

                break;
            case 0:
                //cerr<<"select time out !\n";
                break;
            default:

                if (FD_ISSET(server_fd, &rfd_cpy)){
                    while(newConnect()==true)
                    {
                        ;
                    }
                }

                // login 
                for (auto i = 0;i < loginList.size();i++)
                    if(FD_ISSET(loginList[i].cfd,&rfd_cpy ))
                    {

                        cout <<"sovle login "<<loginList[i].cfd << endl ;
                        solveLogin(i);
                        FD_CLR(loginList[i].cfd , &rfd_cpy ); //避免重复recv
                    }
                
                // send data 
                for (auto i = 0 ; i < clientList.size();i++)
                    if(clientList[i].cfd <= -1 ||  !FD_ISSET(clientList[i].cfd , &rfd_cpy))
                        continue ;
                    else 
                    {
                        cout <<"slove msg "<< loginList[i].cfd << endl ;
                        solveMsg(i);
                    }
        }

    }


}

int Server::serverRecv(int cfd , Packet & packet)
{
    if(socketRecv(cfd , packet)<0)
    {
        cout <<"remove cfd "<<cfd  <<endl;
        close_cfd(cfd);
        return -1 ;
    }



	return 0;
}

int Server::serverSend (int cfd , const Packet & packet )
{
    if(socketSend (cfd , packet )<0)
    {
        cout <<"remove cfd because of send error "<< cfd <<endl;
        close_cfd(cfd);
        return -1 ;
    }
	return 0;
}

int Server::sndResponse(int cfd , unsigned char maintype ,unsigned char subtype ,const char * name )
{
	Packet p ;
    int len = 0 ;
    if(name != NULL)
    {
        len = strlen(name ) +1 ;
        strcpy( p.msg , name );
    }
	fillPacketHeader(p.header,maintype, subtype, len );


	return socketSend(cfd , p);
}


int Server::alterPack( Packet &  desPack , Packet & srcPack , const char * srcId )
{

    if(0)
        cout << debug <<endl ;
    
	if(srcPack.isMainType(mt::sndTxt))
		alterTxtPack(desPack , srcPack , srcId );
	else if (srcPack.isMainType(mt::sndFileHead))
		alterFileHeaderPack(desPack , srcPack , srcId );
	else if (srcPack.isMainType(mt::sndFile))
		alterFileDataPack(desPack , srcPack , srcId );
	else 
		return -1 ;  //不需要转发
	
	return 0 ;
}

int Server::alterTxtPack( Packet &  desPack ,  Packet & srcPack , const char * srcId )
{
    int idNum ;
    if(srcPack.isMainType(mt::sndTxt))
    {
        idNum = (int)srcPack.header.subType;
        if (idNum == 0)
        {
            memcpy(&desPack, &srcPack, sizeof(srcPack));
            return 0;
        }
    }
    else 
    {
        idNum = 1;

       // cout <<"-------------------------file snd ---------------"<<endl;

/*         timeval timeOut ;
        setTime(timeOut , 0 , 5 );
        select (1 , NULL,NULL,NULL, &timeOut ); */


    }

	int idLen = idNum * MAXNAMELEN ;
	
	int txtLen = getPacketLen(srcPack) - idLen -4;
	
    //TODO
    char newsrcId[MAXNAMELEN] ;

    strcpy(newsrcId , srcId );
    strcat (newsrcId , "|");

    memcpy (desPack.msg , newsrcId ,32);
	memcpy(desPack.msg+MAXNAMELEN , srcPack.msg + idLen ,txtLen);


	fillPacketHeader(desPack.header,srcPack.header.mainType,srcPack.header.subType,txtLen +MAXNAMELEN );

	return 0;
}

int Server::alterFileHeaderPack(Packet & desPack , Packet & srcPack , const char * srcId )
{
	memcpy( &desPack , & srcPack , sizeof(desPack));
	

	fileHeader * fhp = (fileHeader *) desPack.msg;
	strcpy(fhp->friName , srcId );

	return 0;
}

int Server::alterFileDataPack (Packet & desPack , Packet & srcPack , const char * srcId )
{
	memcpy( &desPack , & srcPack , sizeof(desPack));
	
	fileData * fdp = (fileData *) desPack.msg;
	strcpy(fdp->friName , srcId );

	return 0;
}

// 发送一个数据包
int Server::sndOneMsg(int index ,const char * rcvName , const Packet & packet )
{
    sockaddr_in toaddr = clientList[nameIndex[rcvName]].sockaddr ;
    int cfd = clientList[index].cfd ;
    const char * fromName = clientList[index].name.c_str();

    if(nameIndex.find(rcvName)==nameIndex.end())
    {
        cout <<"name not exit "<<endl;
        sndResponse(cfd , mt::resSend , sbt::idNotExit , rcvName );
    }
    //cout << "recv name =" <<rcvName <<endl ;
    int rcvIndex = nameIndex[rcvName];
    int tocfd = clientList[rcvIndex].cfd;
        
    if(tocfd <0)
    {
        if(!g_offline_snd) {
            cout << "sovle msg friend not online " << endl;
            mylog._writeDataTransform("not-online-reject", clientList[index].sockaddr , toaddr, clientList[index].name.c_str() , rcvName );
            sndResponse(cfd, mt::resSend, sbt::idOffline, rcvName);
        }
        else {
            cout <<"offline snd "<<endl;
            mylog._writeDataTransform("offline-snd", clientList[index].sockaddr , toaddr, clientList[index].name.c_str() , rcvName );
            clientList[rcvIndex].offlinePacks.push_back(packet);
            sndResponse(cfd, mt::resSend, sbt::offlineSnd, rcvName);
            if(packet.header.mainType==mt::sndTxt)
            {
                dataBase->sql_update_msg(fromName , rcvName , packet.msg+32);   
            }
        }
    }
    else if (tocfd != cfd ) 
    {
/*         cout <<"----------"<<clientList[index].name<<' '<< rcvName <<"----------------"<< endl ;
        cout <<"--ready  snd "<<endl ; */
        serverSend(tocfd , packet);
        
        if(packet.header.mainType==mt::sndTxt)
        {
            sndResponse(cfd , mt::resSend , sbt::success , rcvName );
            mylog._writeDataTransform("txt-pack-send", clientList[index].sockaddr , toaddr, clientList[index].name.c_str() , rcvName );
            dataBase->sql_update_msg(fromName , rcvName, packet.msg+32);
        }
        else if (packet.header.mainType == mt::sndFile )
        {   
            if(packet.header.subType == sbt::myDefault)
            {
                char logmsg [32];
                int count ;
                memcpy(&count , packet.msg+32+16 ,4 );
                sprintf(logmsg , "file-snd%d",count);

                sndResponse(cfd , mt::resFileHead , sbt::packetok , rcvName );
                //mylog._writeDataTransform(logmsg, clientList[index].sockaddr , toaddr, clientList[index].name.c_str() , rcvName );
            }
            else if(packet.header.subType == sbt::success)
            {
                mylog._writeDataTransform("file-snd-ok", clientList[index].sockaddr , toaddr, clientList[index].name.c_str() , rcvName );
            }
            else if (packet.header.subType == sbt::failed)
            {
                 mylog._writeDataTransform("file-reject", clientList[index].sockaddr , toaddr, clientList[index].name.c_str() , rcvName );
            }
        }
    }

    return 0 ;

}
