#include "clientClass.h"
Client::Client(){
    this->IP = "";
    this->sockfd = 0;
    this->clientSSL = nullptr;
}

Client::Client(string IP, int sockfd, SSL* clientSSL){
    this->IP = IP;
    this->sockfd = sockfd;
    this->clientSSL = clientSSL;
}

string Client::getIP(){
    return this->IP;
}

SSL* Client::getSSL(){
    return this->clientSSL;
}

