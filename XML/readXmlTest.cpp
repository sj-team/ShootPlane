#include "./lib/tinyxml.h"
#include <iostream>
#include <string>

using namespace std;

enum State {FAILURE, SUCCESS};

State loadXML()
{
    TiXmlDocument doc;
    if(!doc.LoadFile("test.xml")) {
        cerr << doc.ErrorDesc() << endl;
        return FAILURE;
    }

    TiXmlElement* root = doc.FirstChildElement();
    if(root == NULL) {
        cerr << "Failed to load file: No root element." << endl;
        doc.Clear();
        return FAILURE;
    }

    for(TiXmlElement* elem = root->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement()) {
        string elemName = elem->Value();
        const char* attr;
        attr = elem->Attribute("priority");
        if(strcmp(attr,"1")==0) {
            TiXmlElement* e1 = elem->FirstChildElement("bold");
            TiXmlNode* e2=e1->FirstChild();
            cout<<"priority=1\t"<<e2->ToText()->Value()<<endl;

        }
        else if(strcmp(attr,"2")==0) {
            TiXmlNode* e1 = elem->FirstChild();
            cout<<"priority=2\t"<<e1->ToText()->Value()<<endl;
        }
    }
    doc.Clear();
    return SUCCESS;
}

int main(int argc, char* argv[])
{
    if(loadXML() == FAILURE)
        return 1;
    return 0;
}