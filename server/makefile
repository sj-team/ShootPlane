TARGET := serverd

RELY= server.cpp common/mypacket.cpp common/board.cpp common/common.cpp mysql/sql.cpp XML/lib/tinyxml.cpp XML/lib/tinystr.cpp XML/lib/tinyxmlerror.cpp XML/lib/tinyxmlparser.cpp XML/XmlLog.cpp

serverd: main.cpp  $(RELY)
	g++ -std=c++11 -o serverd main.cpp  $(RELY) -L /usr/lib64/mysql -lmysqlclient -I /usr/include/mysql


clean :
	rm -f $(TARGET) *.o *.txt
	rm -rf serverout
