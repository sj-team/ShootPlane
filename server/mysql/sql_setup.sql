drop DATABASE if EXISTS db1651025; 
create DATABASE db1651025;
use db1651025;

CREATE TABLE userlist(
id varchar(32) PRIMARY KEY,
passwd varchar(32),
log_time DATETIME 
);

CREATE table settings(
id varchar(32) PRIMARY KEY,
set_fontcolor INTEGER,
set_color INTEGER,
set_msgnum INTEGER,
set_fontsize INTEGER
);


CREATE TABLE msgs(
from_id VARCHAR(32),
to_id VARCHAR(32),
msg_time DATETIME ,
msg varchar(140),
key id_time(from_id,to_id,msg_time)
);


insert into userlist values("1",MD5('1'),'1970-1-1 00:00:00');
insert into settings values("1",10,50,20,20);
insert into userlist values("2",MD5('2'),'1970-1-1 00:00:00');
insert into settings values("2",10,50,20,20);
insert into userlist values("3",MD5('3'),'1970-1-1 00:00:00');
insert into settings values("3",10,50,20,20);
insert into userlist values("4",MD5('4'),'1970-1-1 00:00:00');
insert into settings values("4",10,50,20,20);
insert into userlist values("5",MD5('5'),'1970-1-1 00:00:00');
insert into settings values("5",10,50,20,20);


insert into userlist values("mlc",MD5('mlc'),'1970-1-1 00:00:00');
insert into settings values("mlc",10,50,20,20);
insert into userlist values("jhl",MD5('jhl'),'1970-1-1 00:00:00');
insert into settings values("jhl",10,50,20,20);
insert into userlist values("wh",MD5('wh'),'1970-1-1 00:00:00');
insert into settings values("wh",10,50,20,20);
insert into userlist values("oyxw",MD5('oyxw'),'1970-1-1 00:00:00');
insert into settings values("oyxw",10,50,20,20);
insert into userlist values("test",MD5('test'),'1970-1-1 00:00:00');
insert into settings values("test",10,50,20,20);