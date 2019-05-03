#include "mypacket.h"

int socketSend (int cfd , const Packet & packet);

int socketRecv(int cfd , Packet & packet);

fd_set getSet(int cfd)
{
	fd_set  fds ;
	FD_ZERO(&fds);
	FD_SET(cfd , &fds);
	return fds;
}


int socketSend (int cfd , const Packet & packet)
{
	int msgLen = getPacketLen(packet);
	int totalLen = 0;
	int sndLen ;

	fd_set fds , fds_cpy ;
	fds = getSet(cfd);

	timeval timeOut ;

	char * buff = (char *) &packet ;
	while(totalLen <msgLen)
	{
		setTime(timeOut , 5 , 0 );
		fds_cpy = fds ;

		switch(select(cfd+1, NULL , &fds_cpy,NULL,&timeOut))
		{
			case -1 :
				cerr<<"select error "<<endl;
				return -1 ;
			case 0 :
				cerr<<"timeOut error"<<endl;
				return -1;
			default :
				sndLen = send(cfd, buff + totalLen, msgLen - totalLen, 0);
				if (sndLen <= 0)
				{
					cout << strerror(errno) << endl;
					cerr << "socket snd msg error !\n";
					return -1;
				}
				else
					totalLen += sndLen;
		}

	}
/* 	cout <<"mt sbt "<<hex<< (int)packet.header.mainType <<' '<<(int)packet.header.subType<<endl;
	cout << "send ok  snd len = "<< totalLen << endl ; */
	return 0;
}

int socketRecv(int cfd , Packet & packet)
{

	int msgLen = HEADERLEN;
	int totalLen = 0;
	int recvNum;

	char * buff = (char *)& packet ;

	fd_set fdset;
	FD_ZERO(&fdset);
	timeval timeOut ;

	while(totalLen <msgLen)
	{
		FD_SET(cfd , &fdset) ;
		setTime(timeOut , 5 ,0 );

		switch(select(cfd+1, &fdset, NULL, NULL, &timeOut))
		{
			case -1:
				cerr << "select error " << endl;
				return -1;
			case 0:
				cerr << "timeOut error" << endl;
				return -1;
			default:
				recvNum = recv(cfd, buff + totalLen, msgLen - totalLen, 0);
				if (recvNum <= 0)
				{
					cout << strerror(errno) << endl;
					cerr << "0 socket recv msg error !\n ";
					return -1;
				}
				else
					totalLen += recvNum;
				
				if(totalLen == HEADERLEN)
				{
					msgLen = getPacketLen(packet);
				}
		}
	}

/* 	cout <<"mt sbt "<<hex<<(int) packet.header.mainType <<' '<<(int)packet.header.subType<<endl;
	cout << "recv ok totalLen  "<< dec<<totalLen <<endl; */
	return 0;
}


int getPacketLen(const Packet & packet)
{
	return ntohs(packet.header.length);
}


int getFileSize(const char * filePath)
{
	FILE * file = fopen(filePath,"r");
	int size ;
	if(file)
	{
		fseek(file,0,SEEK_END);
		size = ftell(file);
		fclose(file);
		return size ;
	}
	
	return -1 ;
}

int getFileName( const char * filePath)
{
	int i ;
	for(i=strlen(filePath);i>=0;i--)
		if(filePath[i]=='\\'||filePath[i]=='/')
			break;
	return i+1;

}

int sndFileKind(int cfd , const char * id , unsigned char subType, const char * filepath)
{

	Packet p ;

	// send fileHeader 
	fillPacketHeader(p.header,mt::sndFileHead, subType ,sizeof(fileHeader));
	fileHeader * headp = (fileHeader *)p.msg;
	int fileSize = getFileSize(filepath);
	cout <<filepath<<"*****"<<fileSize<<endl;
	if(fileSize <0)
		return -1 ;
	
	strcpy(headp->friName,id);
	strcpy(headp->fileName,filepath+getFileName(filepath));
	int fileId ; 
	// TODO    ¼ÆËãidÖµ

	
	headp->fileId = fileId ;
	int packNum = fileSize / MAXDATALEN + (fileSize % MAXDATALEN)!=0;
	headp->packNum =  htonl(packNum);

	socketSend(cfd,p);
	
	// send fileData 
	fillPacketHeader(p.header,mt::sndFile,sbt::myDefault,sizeof(fileData));

	FILE * filep = fopen(filepath , "r");
	fileData * datap = (fileData *)p.msg ; 

	strcpy(datap->friName , id );
	datap->fileId = htonl(fileId) ; 
	datap->count = htonl(0) ;


	for(int i =0 ; i< packNum ; i++)
	{
		cout <<"packet count = "<<i<<endl;
		fread(datap->data,MAXDATALEN,1,filep);

		//  TEST 
		if(i==packNum-1 && (fileSize % MAXDATALEN))
			p.header.length = htonl(fileSize % MAXDATALEN + 40 );
		
		datap->count = htonl(i);
		socketSend(cfd , p);
	}

	return 0;

}


/////////////////////////////////////////////////////////////////////////////

int fillPacketHeader(packetHeader & header , unsigned char mainType , unsigned char resType , unsigned short msgLen)
{
	header.mainType = mainType ;
	header.subType = resType;
	header.length = htons(msgLen+HEADERLEN);
}

int sndLogin(int cfd , const char * username , const char * passwd )
{
	int ulen = strlen(username);
	int plen = strlen(passwd);
	if(ulen >32 || plen >32)
		return -1 ;
	
	Packet p ;
	
	fillPacketHeader(p.header,mt::login,sbt::request , sizeof(loginData));


	cout <<"login len = "<< getPacketLen(p)<<endl ; 
	loginData * datap = (loginData *) p.msg ;

	strcpy(datap->username,username);
	strcpy(datap->passwd, passwd);

	return socketSend ( cfd , p);

}


//   clinet 

int clientRecv(int cfd , Packet & packet )
{
	return socketRecv (cfd , packet );
}

int sndText(int cfd , const char * ids  , int idNum  , const char * text )
{
	Packet p ;
	
	int idLen = idNum * 32 ; 
	memcpy(p.msg , ids ,idLen);

	int textLen = strlen(text);
	
	memcpy(p.msg + idLen ,  text , textLen );

	fillPacketHeader(p.header, mt::sndTxt, (unsigned char) idNum , idLen + textLen );

	socketSend(cfd, p);

	return 0 ;
}

int sndJPG (int cfd , const char * id , const char * jpgpath )
{

	return sndFileKind(cfd, id , sbt::jpg , jpgpath);

}

int sndGIF(int cfd , const char * id , const char * gifpath)
{

	return sndFileKind(cfd, id , sbt::gif , gifpath);

}

int sndFile (int cfd , const char * id , const char * filepath)
{

	return sndFileKind (cfd , id , sbt::file , filepath);
}


int firstChangePwd(int cfd , const char * newPwd )
{
	Packet p ;
	int msgLen = strlen(newPwd);
	
	strcpy(p.msg,newPwd);
	
	fillPacketHeader(p.header,mt::login,sbt::changepwd,msgLen);
	
	return socketSend(cfd, p);
}

// server 


