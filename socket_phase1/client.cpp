#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>
#include <cstring>
#include <exception>
using namespace std;

#define MAX_STRING_LENGTH 200
#define ListenerBacklog 10

int main(){
    //create socket
    int socket_d = 0;
    socket_d = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_d == -1){
        cout << "Fail to create a socket" << endl;
    }

    int hostPort = 0;
    string hostIP;
    cout << "Please enter server's port number: ";
    cin >> hostPort;
    cin.ignore(MAX_STRING_LENGTH,'\n');
    cout << "Please enter the IP address(or enter 'localhost' instead): ";
    cin >> hostIP;
    cin.ignore(MAX_STRING_LENGTH,'\n');

    if(hostIP.compare("localhost")==0){
        hostIP = "127.0.0.1";
    }

    //Setting socket information
    struct sockaddr_in sockInfo;
   // bzero(&info,sizeof(sockInfo));//Initialization
    sockInfo.sin_family = AF_INET;//IPv4
    sockInfo.sin_addr.s_addr = inet_addr(hostIP.c_str());
    sockInfo.sin_port = htons(hostPort);

    // establish a connection
    int isConnect = connect(socket_d, (struct sockaddr*)&sockInfo, sizeof(sockInfo));
    if(isConnect == -1){
        cout << "Fail to connect." << endl;
    }

    char recvMessage[MAX_STRING_LENGTH];
    recv(socket_d, recvMessage, sizeof(recvMessage),0);
    cout << recvMessage << endl;

    string sendToServerMessage;
    //main function
    while(true){
        //function list
        int num1;
        cout << "\n======================\n\nChoose a function:" << endl;
        cout << "1.Register\n2.Login\nYour choice: ";
        cin >> num1;//write exception
        cin.ignore(MAX_STRING_LENGTH,'\n');
        memset(recvMessage, 0, sizeof(recvMessage));
        sendToServerMessage.clear();
        switch(num1){
            //Register
            case 1:
            {
                string userName;
                string deposit;

                cout << "Please enter user's account name: ";
                cin >> userName;
                cin.ignore(MAX_STRING_LENGTH,'\n');

                cout << "Please enter deposit amount: ";
                cin >> deposit;
                cin.ignore(MAX_STRING_LENGTH,'\n');

                sendToServerMessage = "REGISTER#" + userName + "#"+ deposit;
                send(socket_d, sendToServerMessage.c_str(), sizeof(sendToServerMessage), 0);
                recv(socket_d, recvMessage, sizeof(recvMessage), 0);
                cout << recvMessage;
                break;
            }
            //Login
            case 2:
            {
                string userName;
                string portNum;

                cout << "Please enter user's account name: ";
                cin >> userName;
                cin.ignore(MAX_STRING_LENGTH,'\n');

                cout << "Please enter the client's port number: ";
                cin >> portNum;
                cin.ignore(MAX_STRING_LENGTH,'\n');

                sendToServerMessage = userName + "#" + portNum;
                send(socket_d, sendToServerMessage.c_str(), sizeof(sendToServerMessage), 0);
                recv(socket_d, recvMessage, sizeof(recvMessage), 0);
                cout << recvMessage;

                if(strcmp(recvMessage,"220 AUTH_FAIL\n") == 0){
                    cout << "user doesn't exist!!"<< endl;
                }
                else{
                    pid_t pid;
                    pid = fork();
                    if(pid < 0){
                        cout << "fork doesn't exist!!";
                    }
                    else if (pid == 0){//child fork
                    ///////////////////////////////////////////////////////////////I'm child fork, I'm listening
                        int sock_toListen = 0;
                        sock_toListen = socket(AF_INET, SOCK_STREAM, 0);

                        if (sock_toListen == -1){
                            cout << "Fail to create a socket!!" << endl;
                        }
                        //Setting socket information
                        struct sockaddr_in sock_toListen_info;
                        // bzero(&info,sizeof(sockInfo));//Initialization
                        sock_toListen_info.sin_family = AF_INET;//IPv4
                        sock_toListen_info.sin_addr.s_addr = inet_addr("127.0.0.1");
                        sock_toListen_info.sin_port = htons(atoi(portNum.c_str()));

                        int isbind = bind(sock_toListen, (struct sockaddr*) &sock_toListen_info, sizeof(sock_toListen_info));
                        if (isbind < 0){
                            cout << "Failed to bind!!\n";
                        }  
                        listen(sock_toListen, ListenerBacklog);
                        while(true){
                            int sock_forClient;
                            struct sockaddr_in sock_forClient_info;
                            int addrlen = sizeof(sock_forClient_info);
                            sock_forClient = accept(sock_toListen, NULL, NULL);
                            memset(recvMessage, 0, sizeof(recvMessage));
                            string sendToPeer = "Connected!!!\n";
                            send(sock_forClient, sendToPeer.c_str(), sizeof(sendToPeer), 0);
                            recv(sock_forClient, recvMessage, sizeof(recvMessage), 0);
                            cout << "\n===========Transaction Record===========\n\n" << recvMessage << "\n========================================\n\n";
                            close(sock_forClient);
                            break;
                        } 
                        //cout << "child say goodbye~" << endl;
                    }
                    else{//parent fork
                        while(true){
                            int num2;
                            memset(recvMessage, 0, sizeof(recvMessage));
                            sendToServerMessage.clear();
                            cout << "\n======================\n\nChoose a function:\n1.Query Accounts information\n2.Exit\n3.Micropayment transaction\nYour choice: ";
                            cin >> num2;//write exception
                            cin.ignore(MAX_STRING_LENGTH,'\n');
                            switch (num2){
                                //Query Accounts information
                                case 1:
                                {
                                    sendToServerMessage = "List";
                                    send(socket_d, sendToServerMessage.c_str(), sizeof(sendToServerMessage), 0);
                                    recv(socket_d, recvMessage, sizeof(recvMessage), 0);
                                    cout << recvMessage;
                                    break;                
                                }
                                //Exit
                                case 2:
                                {
                                    sendToServerMessage = "Exit";
                                    send(socket_d, sendToServerMessage.c_str(), sizeof(sendToServerMessage), 0);
                                    recv(socket_d, recvMessage, sizeof(recvMessage), 0);
                                    cout << recvMessage;               
                                    break;
                                }
                                //Micropayment transaction
                                case 3:
                                {  
                                    ////////////////////////create socket to connect with other's child
                                    //create socket
                                    int socket_toPeer = 0;
                                    socket_toPeer = socket(AF_INET, SOCK_STREAM, 0);

                                    if (socket_toPeer == -1){
                                        cout << "Fail to create a socket to peer!!" << endl;
                                    }

                                    string peerName;
                                    cout << "who do you want to transact with? ";
                                    cin >> peerName;
                                    cin.ignore(MAX_STRING_LENGTH,'\n');

                                    ///////////take list to parse
                                    send(socket_d, "List", sizeof("List"), 0);
                                    recv(socket_d, recvMessage, sizeof(recvMessage), 0);

                                    // parse server's list
                                    string to_parse(recvMessage);
                                    int account_list_begin = 0;
                                    while (to_parse[account_list_begin++] != '\n') {
                                        continue;
                                    }
                                   // cout << "ONE" << endl;
                                    while (to_parse[account_list_begin++] != '\n') {
                                        continue;
                                    }
                                  //  cout << "TWO" << endl;
                                    string peerIP = "";
                                    string peerPort = "";
                                    string temp = "";
                                    while (peerIP == "" && peerPort == "")
                                    {
                                      //  cout << "THREE" << endl;
                                        while (to_parse[account_list_begin] != '#') { // parse name
                                            temp += to_parse[account_list_begin];
                                            account_list_begin++;
                                        }
                                      //  cout << "NAME: " << temp << endl;
                                        account_list_begin++;
                                        if (temp == peerName) {
                                            while (to_parse[account_list_begin] != '#') { // parse ip
                                                peerIP += to_parse[account_list_begin];
                                                account_list_begin++;
                                            }
                                            account_list_begin++;
                                         //   cout << "IP: " << peerIP << std::endl;
                                            while (to_parse[account_list_begin] != '\n') { // parse port
                                                peerPort += to_parse[account_list_begin];
                                                account_list_begin++;
                                            }
                                            account_list_begin++;
                                          //  cout << "PORT: " << peerPort << std::endl;
                                            break;
                                        }
                                        else { // pass this line
                                            temp = "";
                                            while (to_parse[account_list_begin++] != '\n') { 
                                                continue;
                                            }
                                        }
                                    }


                                    // string peerPort;
                                    // cout << "Please enter peer port: ";
                                    // cin >> peerPort;
                                    // cin.ignore(MAX_STRING_LENGTH,'\n');

                                    // string peerIP;
                                    // cout << "Please enter the IP address(or enter 'localhost' instead): ";
                                    // cin >> peerIP;
                                    // cin.ignore(MAX_STRING_LENGTH,'\n');

                                    // if(peerIP.compare("localhost")==0){
                                    //     peerIP = "127.0.0.1";
                                    // }

                                    //Setting socket information
                                    struct sockaddr_in sock_toPeer_info;
                                    // bzero(&info,sizeof(sockInfo));//Initialization
                                    sock_toPeer_info.sin_family = AF_INET;//IPv4
                                    sock_toPeer_info.sin_addr.s_addr = inet_addr(peerIP.c_str());
                                    sock_toPeer_info.sin_port = htons(atoi(peerPort.c_str()));

                                    // establish a connection
                                    isConnect = connect(socket_toPeer, (struct sockaddr*)&sock_toPeer_info, sizeof(sock_toPeer_info));
                                    if(isConnect == -1){
                                        cout << "Fail to connect with peer!!" << endl;
                                    }

                                    memset(recvMessage, 0, sizeof(recvMessage));
                                    recv(socket_toPeer, recvMessage, sizeof(recvMessage),0);
                                    cout << recvMessage << endl;
                                    
                                    string transFee;
                                    cout << "How much do you want to transact? ";
                                    cin >> transFee;

                                    sendToServerMessage.clear();
                                    sendToServerMessage = userName + "#" + transFee + "#" + peerName;
                                    send(socket_toPeer, sendToServerMessage.c_str(), sizeof(sendToServerMessage), 0);
                                    break;
                                }
                            }
                            if(num2 == 2){
                                break;
                            }
                        }
                        
                    }
                    //cout << "user exist" << endl;

                }
            }    
            break;
        }
    }
    close(socket_d);
    cout << "close Socket\n";
    return 0;
}
