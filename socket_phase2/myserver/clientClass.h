#ifndef _CLIENTCLASS_H_
#define _CLIENTCLASS_H_

#include <iostream>
using namespace std;


class Client{
private:
    char* IP;
public:
    Client();
    Client(char*, int);
    int sockfd;
    string getIP();
};

#endif

