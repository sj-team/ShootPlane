#include "./lib/tinyxml.h"
#include <iostream>
#include <string>

using namespace std;

enum State {FAILURE, SUCCESS};

State saveXML()
{
    TiXmlDocument doc;

    TiXmlElement* root = new TiXmlElement("root");
    doc.LinkEndChild(root);

    TiXmlElement* element1 = new TiXmlElement("Element1");
    root->LinkEndChild(element1);

    element1->SetAttribute("attribute1", "some value");


    TiXmlElement* element2 = new TiXmlElement("Element2");  ///元素
    root->LinkEndChild(element2);

    element2->SetAttribute("attribute2", "2");
    element2->SetAttribute("attribute3", "3");


    TiXmlElement* element3 = new TiXmlElement("Element3");
    element2->LinkEndChild(element3);

    element3->SetAttribute("attribute4", "4");

    TiXmlText* text = new TiXmlText("Some text.");  ///文本
    element2->LinkEndChild(text);


    bool success = doc.SaveFile("writeXmlTest.xml");
    doc.Clear();

    if(success)
        return SUCCESS;
    else
        return FAILURE;
}

int main(int argc, char* argv[])
{
    if(saveXML() == FAILURE)
        return 1;
    return 0;
}