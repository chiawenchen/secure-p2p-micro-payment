#include "clientClass.h"

Client::Client(){
    this->IP = {};
    this->sockfd = 0;
}

Client::Client(char *IP, int sockfd){
    this->IP = IP;
    this->sockfd = sockfd;
 //   cout << "a client is created!\n";
}

string Client::getIP(){
    string _IP(this->IP);
    return _IP;
}

