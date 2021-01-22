#ifndef _CLIENTCLASS_H_
#define _CLIENTCLASS_H_

#include <iostream>
#include "SSL.h"
using namespace std;


class Client{
private:
    string IP;
    SSL* clientSSL;
public:
    Client();
    Client(string, int, SSL*);
    int sockfd;
    string getIP();
    SSL* getSSL();
};

#endif

