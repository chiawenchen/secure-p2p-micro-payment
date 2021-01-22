#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <arpa/inet.h>
#include <string>
#include <fstream>

#include "const.h"
#include "clientClass.h"
#include "user_userList_class.h"
#include "threadPoolClass.h"
#include "SSL.h"

using namespace std;

int main(){
    
    char CertFile [] = "./Cert_s.crt";
    char KeyFile [] = "./Key_s.key";

    SSL_CTX* ctx = InitServerCTX();
    LoadCertificates(ctx);
    
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
        exit(1);
    }  

    listen(sockfd, LISTEN_BACKLOG);
    cout << "Waiting for connection......" << endl;

    ThreadPool threadPool;
    threadPool.createThreads();

    while(true){
        int forClientSockfd = accept(sockfd, (struct sockaddr*)&clientInfo, (socklen_t*)&clientAddrlen);//sizeof(clientAddrlen)
        if(forClientSockfd == -1){
            cout << "Fail to accept!";
            close(forClientSockfd);
            continue;
        }
        else{
            SSL* ssl = SSL_new(ctx);
            SSL_set_fd(ssl, forClientSockfd);
            if (SSL_accept(ssl) == -1){
                ERR_print_errors_fp(stderr);
                close(forClientSockfd);
                continue;
            }
            ShowCerts(ssl);
            // ssl substitutes socket to communicate with client

            char sendToClientMessage[] = {"Connecting...\n"};
            SSL_write(ssl, sendToClientMessage, sizeof(sendToClientMessage));
            
            //extract client ip
            struct sockaddr_in* pV4Addr = (struct sockaddr_in *)&clientInfo;
            struct in_addr ipAddr = pV4Addr -> sin_addr;
            
            //Getting IP address "string"
            char clientIP[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &ipAddr, clientIP,INET_ADDRSTRLEN);

            //creat a client
            string ip_addr(clientIP);
            Client client(ip_addr, forClientSockfd, ssl);
            threadPool.connect(client);//enter threadPoolClass
        }
    }

    return 0;
}