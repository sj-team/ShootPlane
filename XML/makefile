TEST := readXmlTest writeXmlTest
TARGET := test XmlLog.cpp
LIB := ./lib/tinyxml.cpp ./lib/tinystr.cpp ./lib/tinyxmlerror.cpp ./lib/tinyxmlparser.cpp

main: $(TARGET)

test: test.cpp $(LIB) XmlLog.cpp
	g++ -std=c++11 -o $@ $^ -lrt

readXmlTest: xmlParser.cpp $(LIB)
	g++ -std=c++11 -o $@ $^ -lrt

writeXmlTest: xmlParser.cpp $(LIB)
	g++ -std=c++11 -o $@ $^ -lrt

clean :
	rm -f $(TARGET)