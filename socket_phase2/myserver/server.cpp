#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <arpa/inet.h>

#include "const.h"
#include "clientClass.h"
#include "user_userList_class.h"
#include "threadPoolClass.h"
#include <string>
using namespace std;

int main(){
    int serverPort;
    cout << "Please enter server's port: ";
    cin >> serverPort;
    int sockfd = 0;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    while(sockfd == -1){
        cout << "Fail to create a socket!";
        cout << "Please enter server's port: ";
        cin >> serverPort;
        int sockfd = 0;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
    }
   // cout << "sockfd: " << sockfd << endl;
    struct sockaddr_in clientInfo; 
    struct sockaddr_in serverInfo;
    int clientAddrlen = sizeof(clientInfo);
    bzero(&serverInfo, sizeof(serverInfo));

    serverInfo.sin_family = PF_INET;
    serverInfo.sin_addr.s_addr = INADDR_ANY;
    serverInfo.sin_port = htons(serverPort);

    int isBind = bind(sockfd, (struct sockaddr*) &serverInfo, sizeof(serverInfo));
    if (isBind < 0){
        cout << "Failed to bind!\n";
        return 1;
    }  

    listen(sockfd, LISTEN_BACKLOG);
    cout << "Waiting for connection......" << endl;

    ThreadPool threadPool;
    threadPool.createThreads();

    while(true){
        int forClientSockfd = accept(sockfd, (struct sockaddr*)&clientInfo, (socklen_t*)&clientAddrlen);//sizeof(clientAddrlen)
        if(forClientSockfd == -1){
            cout << "Fail to create client sockfd!";
        }
        else{
            char sendToClientMessage[] = {"Connection accepted!\n"};
           // cout << "Connection accepted!\n";
            send(forClientSockfd, sendToClientMessage, sizeof(sendToClientMessage), 0);
        }
        //extract client ip
        struct sockaddr_in* pV4Addr = (struct sockaddr_in *)&forClientSockfd;//need?
        struct in_addr ipAddr = pV4Addr -> sin_addr;
        //Getting IP address "string"
        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &ipAddr, clientIP,INET_ADDRSTRLEN);

        //creat a client
        Client client(clientIP, forClientSockfd);
        threadPool.connect(client);
    }
    return 0;
}