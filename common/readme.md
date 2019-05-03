# 报文头设计

* maintype：1字节，代表报文主类型
* subtype：1字节，代表对主类型的修饰
* length：2字节，代表报文的总长度
* msg ：最长为2048字节，报文的信息，可根据不同的报文做类型转换



# 报文设计

## **client登录**
行为|maintype|subtype|
---|:--:|---:
请求登录|0x11(login)|0x00(request)
登录成功（通知server）|0x11(login)|0x01(success)
首次改密|0x11(login)|0x02(changepwd)

</br>

#### 报文信息(请求登录)

* 32位用户名
* 32位密码

#### 报文信息(登录成功)

* 无

#### 报文信息(首次改密)


* 32位密码

## **client向好友发送文本**

#### 报文头  

行为|maintype|subtype|
---|:--:|:---:
发送文本消息|0x14(sndTxt)| x(代表发送信息用户名数量)

#### 报文信息

* x 个 32字节的char型 （上线或下线的用户名 name1 name2 name3 ...）
* 最长为 MAXLENGTH 的数据

#### 注：
如果为群发信息，则x为0，报文信息直接为发送的数据

</br>

## **client上线下线修改好友列表信息**

#### 报文头  

行为|maintype|subtype|
---|:--:|:---:
通知上线|0x22(updateList)| 0x01(tellOnline)
通知下线|0x22(updateList)| 0x00(tellOffline)

#### 报文信息

* 32字节的char型 （上线或下线的用户名）

</br>


## **client更改配置信息**

#### 报文头  

行为|maintype|subtype|
---|:--:|:---:
更改主题|0x21(conf)| 0x01(winTheme)
更改历史长度|0x21(conf)| 0x03(hisNum)
更改字体颜色|0x21(conf)| 0x04 (fontColor)
更改字体大小|0x21(conf)| 0x05 (fontSize)
#### 报文信息（更改土体、历史长度、颜色、字体大小）

* 4字节整数



## **client向好友发送文件**
### 阶段1：发送消息头报文，得到文件id和分成多少个包（在这个阶段完成打开文件指针，建立id到文件指针的关系）；其中id为通过md5散列得到的16字节的值

#### 报文头  （发送者给接收者）

行为|maintype|subtype|
---|:--:|:---:
发送文件|0x12(sndFileHead)|0x01(file)
发送jpg|0x12(sndFileHead)|0x02(jpg)
发送gif|0x12(sndFileHead)|0x03(gif)

#### 报文内容
* 32字节的char型 （用户名） // 发包为接收者的用户名，收包为发送者的用户名，server端做处理
* 32字节的char型 （文件名）
* 16字节类型 id （代表文件编号）
* int类型 count （代表一共几个包）

#### 报文头  （接收者回复发送者）
行为|maintype|subtype|
---|:--:|:---:
选择接收|0x73(resFileHead)|0x01(success)
拒绝接收|0x73(resFileHead)|0x00(failed)

#### 报文内容
* 16字节文件id

### 阶段2：按照数据包发送分包之后的数据


#### 报文头  

行为|maintype|subtype|
---|:--:|:---:
发送文件类型信息|0x13(sndFile)|0x00(filedata)

#### 报文内容
* 32字节char类型（接收者用户名） //
* 16字节 id （代表文件编号） // 仅通过用户名判断
* int类型 count （代表第几个包）
* 最长为 MAXLENGTH 的数据

### 阶段3：发送文件传输结束标志 （通知传输文件结束）

#### 报文头  

行为|maintype|subtype|
---|:--:|:---:
发送文件类型信息|0x13(sndFile)|0x01(success)

#### 报文内容
* 16字节 id （代表文件编号）

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
离线转发|0x72(resSend)|0x07(offlineSnd)

#### 报文内容
* char name[32] （好友名称）

# 代码实现


```
namespace cstate 
{
	const char online = '1' ;
	const char offline = '0' ;
};

 namespace mt
 {
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

	const unsigned char winTheme = 0x01;
	const unsigned char friList = 0x02 ;
	const unsigned char hisNum = 0x03 ;
	const unsigned char fontColor = 0x04 ;
	const unsigned char fontSize = 0x05;
	const unsigned char hisMsg = 0x06 ;

	const unsigned char tellOnline = 0x01 ;
	const unsigned char tellOffline = 0x02 ;
	const unsigned char offlineSnd = 0x07 ;

};




struct packetHeader{
	unsigned char mainType ;
	unsigned char subType ;
	unsigned short length;
};

struct TxtData
{
	char friName [MAXNAMELEN ];
	char txtMsg [MAXDATALEN];
};

//  传输文件类型的数据 ，第一个包是这个格式  
struct fileHeader 
{
	char friName [MAXNAMELEN]; 
	char fileName [MAXNAMELEN];
	int  fileId ;
	int  packNum ;
};

//  file 类型
struct fileData{
	char friName[MAXNAMELEN];
	int fileId ;
	int count ;
	char data[MAXDATALEN];
};


//  认证登录
struct loginData{
	char username [MAXNAMELEN];
	char passwd [MAXPASSWDLEN];
};

struct changePwdData {
	char newPasswd [MAXPASSWDLEN];
};

```




