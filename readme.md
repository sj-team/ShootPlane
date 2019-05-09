# Server

## Run

./server --port xxxx --ip xxxxxx  
**ATENTION: --ip option is not recommended!**

**注意: --ip选项不是必要的，缺省为0.0.0.0**


# 报文头设计

## **整体封包设计**
* maintype：1字节，代表报文主类型
* subtype：1字节，代表对主类型的修饰
* length：2字节，代表报文的总长度
* msg ：最长为2048字节，报文的信息，可根据不同的报文做类型转换


# 报文设计

## **心跳包**
行为|maintype|subtype|
---|:--:|---:
client向server发心跳包|0xff(beat)|0x00(无)
server向client发心跳包|0xff(beat)|0x00(无)

#### 报文信息(心跳包)
无

## **client登录**
行为|maintype|subtype|
---|:--:|---:
请求登录|0x11(login)|0x00(request)
登录成功（通知server）|0x11(login)|0x01(success)
首次改密|0x11(login)|0x02(changepwd)

#### 报文信息(请求登录)

* 32位用户名
* 32位密码

#### 报文信息(登录成功)

* 无内容，只有报文头

#### 报文信息(首次改密)

* 32位密码


## **client上线下线对战修改好友列表信息**

#### 报文头  

行为|maintype|subtype|
---|:--:|:---:
通知上线|0x22(updateList)| 0x01(tellOnline)
通知下线|0x22(updateList)| 0x00(tellOffline)
通知进入游戏|0x22(updateList)| 0x02(tellPlaying)
通知结束|0x22(updateList)| 0x03(tellEnd)
#### 报文信息

* 最长32字节的字符串，包含尾零 （发出广播的用户名）

</br>


## **client更改配置信息**

#### 报文头  

行为|maintype|subtype|
---|:--:|:---:
更改主题|0x21(conf)| 0x01(winTheme)
更改历史长度|0x21(conf)| 0x03(hisNum)
更改字体颜色|0x21(conf)| 0x04 (fontColor)
更改字体大小|0x21(conf)| 0x05 (fontSize)
#### 报文信息（更改主题、历史长度、颜色、字体大小）

* 4字节整数


## **server回应登录**

行为|maintype|subtype|
---|:--:|:---:
验证失败|0x71(resLogin)|0x00(failed)
验证成功|0x71(resLogin)|0x01(success)
首次登录（强制改密）|0x71(resLogin)|0x02(changepwd)
密码错误|0x71(resLogin)|0x03(pwderror)
重复登录（强制下线）|0x71(resLogin)|0x04(repeatoff)
重复登录（新上线）|0x71(resLogin)|0x05(repeaton)
#### 报文内容
* 无


## **server向client发送配置信息**
#### 报文头  

行为|maintype|subtype|
---|:--:|:---:
窗口主题|0x81(resConf)|0x01(winTheme)
好友列表|0x81(resConf)|0x02(friList)
历史长度|0x81(resConf)| 0x03(hisNum)
#### 报文内容
* 4字节整数

## **server回应转发状态**

#### 报文头  

行为|maintype|subtype|
---|:--:|:---:
成功转发|0x72(resSend)|0x01(success)
好友名称不存在|0x72(resSend)|0xfe(idNotExit)
好友未上线|0x72(resSend)|0xfd(idOffline)
好友正在游戏中|0x72(resSend)|0x07(idPlaying)

#### 报文内容
* char name[32] （好友名称）

## **建立对局**
行为|maintype|subtype|
---|:--:|---:
请求对战|0x31(connect)|0x00(request)
接收对战（被挑战者发）|0x41(resConnect)|0x02(accept)
拒绝对战（被挑战者发）|0x41(resConnect)|0x03(deny)
对战成功（对战成功后，挑战者告诉服务器收到）|0x31(connect)|0x01(success)
请求放置(server向两位玩家发送，要求放置飞机)|0x51(askGame)|0x01(locate)
请求重新放置(放置飞机失败，server请求client重新放置)|0x51(askGame)|0x10(relocate)
请求猜棋(server向玩家发送，要求猜棋子)|0x51(askGame)|0x02(turn)
请求猜棋(server向玩家发送，要求猜棋子)|0x51(askGame)|0x00(wait)
放置信息(client向server发送，放置飞机的信息)|0x52(init)|0x01(locate)
</br>

#### 报文信息(请求对战)

* 32位用户名

#### 报文信息(放置信息)

* 3对双坐标 ， 每个坐标为 （unsigned char y ， unsigned char x）;


#### 报文信息(对战成功，请求放置,接收对战,拒绝对战,请求猜棋等)

* 无内容，只有报文头

## **游戏过程**
行为|maintype|subtype|
---|:--:|---:
猜点|0x32(play)|0x02(unmask)
猜飞机位置|0x32(play)|0x01(locate)
认输|0x32(play)|0xf0(surrender)
反馈猜点（server反馈）|0x42(resPlay)|0x02(unmask)
反馈猜位置（server反馈）|0x42(resPlay)|0x01(locate)
反馈输（server反馈）|0x42(resPlay)|0x03(win)
反馈赢（server反馈）|0x42(resPlay)|0x04(lose)

</br>

#### 报文信息(猜点)

* 两个unsigned char类型坐标 x,y

#### 报文信息(猜飞机位置)

* 两对unsigned char类型坐标 x1,y1 ,x2,y2

#### 报文信息(反馈猜点)

* 两个unsigned char类型坐标x,y
* unsigned char 类型，结果
* unsigned char 类型，当前分数

#### 报文信息(反馈猜飞机位置)
* 两对unsigned char类型坐标x1,y1,x2,y2
* unsigned char 类型，结果
* unsigned char 类型，当前分数

#### 报文信息(反馈输、反馈赢、认输)
* 无内容，仅有报文头


# 代码实现


```C
 namespace mt
 {
	const unsigned char beat = 0xff;
	 // client  send 
	const unsigned char login = 0x11;
	const unsigned char sndFileHead = 0x12 ;
	const unsigned char sndFile = 0x13;
	const unsigned char sndTxt = 0X14;

	const unsigned char conf = 0x21 ;
	const unsigned char updateList = 0x22 ;

	const unsigned char resLogin = 0x71;
	const unsigned char resSend = 0X72;
	const unsigned char resConf = 0X81 ;
	const unsigned char resFileHead =0x73;
 
	const unsigned char connect = 0x31;
	const unsigned char resConnect = 0x41;
	const unsigned char askGame = 0x51;
	const unsigned char init = 0x52 ;

	const unsigned char play = 0x32 ;
	const unsigned char resPlay = 0x42;

 };

namespace sbt
{
	const unsigned char myDefault = 0x00 ;

	const unsigned char request = 0x00 ;
	const unsigned char success = 0x01 ;
	const unsigned char changepwd = 0x02 ;

	const unsigned char groupChat = 0x00 ;
	const unsigned char file = 0x01;
	const unsigned char jpg = 0x02;
	const unsigned char gif = 0x03 ;

	const unsigned char failed = 0x00;
	const unsigned char pwderror = 0x03 ;
	const unsigned char repeatoff = 0x04 ;
	const unsigned char repeaton = 0x05 ;
	const unsigned char pwdChangeErr = 0x06;

	const unsigned char idNotExit = 0xfe;
	const unsigned char idOffline = 0xfd;
	const unsigned char idPlaying = 0xfc;

	const unsigned char winTheme = 0x01;
	const unsigned char friList = 0x02 ;
	const unsigned char hisNum = 0x03 ;
	const unsigned char fontColor = 0x04 ;
	const unsigned char fontSize = 0x05;
	const unsigned char hisMsg = 0x06 ;

	const unsigned char tellOnline = 0x01 ;
	const unsigned char tellOffline = 0x00 ;
	const unsigned char tellPlaying = 0x02 ;
	const unsigned char tellEnd = 0x03 ;


	const unsigned char offlineSnd = 0x07 ;
	const unsigned char packetok = 0x08 ;

	// mt::connect || mt::resConnect || mt::askInit
 	//const unsigned char request 
	const unsigned char accept = 0x02 ;
	const unsigned char deny = 0x03 ;
	const unsigned char locate = 0x01;
	const unsigned char wait = 0x00 ;
	const unsigned char turn = 0x02 ;
	const unsigned char relocate = 0x10 ;
	

	const unsigned char unmask = 0x02;
	const unsigned char win = 0x03 ;
	const unsigned char lose = 0x04;
	const unsigned char surrender = 0xf0;


};

// 报文头
struct packetHeader{
	unsigned char mainType ;
	unsigned char subType ;
	unsigned short length;

};


//  认证登录
struct loginData{
	char username [MAXNAMELEN];
	char passwd [MAXPASSWDLEN];
};

struct changePwdData {
	char newPasswd [MAXPASSWDLEN];
};

// locate 设置初始飞机
struct locateData {
	unsigned char p1_x1 , p1_y1 , p1_x2 , p1_y2 ;
	unsigned char p2_x1 , p2_y1 , p2_x2 , p2_y2 ;
	unsigned char p3_x1 , p3_y1 , p3_x2 , p3_y2 ;
};


//client向server发，猜点信息
struct unmaskPointData{
	unsigned char x ;
	unsigned char y ;
};

//client向server发，猜飞机信息
struct unmaskLocateData{
	unsigned char x1 , y1 ;
	unsigned char x2 , y2 ;
};

//server向client发，猜点反馈
struct unmaskPointResult{
	unsigned char x ;
	unsigned char y ;
	unsigned char result ;
	unsigned char score ;
};

//server向client发，猜飞机反馈
struct unmaskLocateResult{
	unsigned char x1 , y1 ;
	unsigned char x2 , y2 ;
	unsigned char result ;
	unsigned char score ;
};


```




