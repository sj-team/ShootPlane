//mysql_demo.cpp
#include <iostream>	// cin,coutµÈ
#include <iomanip>	// setwµÈ
#include "sql.h"
using namespace std;

int main(int argc, char* argv[])
{
    SERVER_MYSQL dbsql;
    vector<string> ve;
    while(1){
        char id[100],id2[100],msg[100];
        int num;
        cin>>id;

        cin>>num;
        dbsql.set_fontcolor(id,num);
        cout<<1<<"   "<<dbsql.get_fontcolor(id)<<endl;

        cin>>num;
        dbsql.set_color(id,num);
        cout<<2<<"   "<<dbsql.get_color(id)<<endl;

        cin>>num;
        dbsql.set_msgnum(id,num);
        cout<<3<<"   "<<dbsql.get_msgnum(id)<<endl;
    }

    return 0;
}
