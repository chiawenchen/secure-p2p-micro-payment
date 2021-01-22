#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> //gain access to the definition of _LP64 and _ILP32
#include <sys/socket.h> //makes available a type, socklen_t
#include <netinet/in.h> //defines the IN6ADDR_LOOPBACK_INIT macro
#include <arpa/inet.h> //inet_addr()
#include <string>
#include <unistd.h>
#include <cstring>
#include <exception>
#include "SSL.h" //Secure Sockets Layer
using namespace std;

#define MAX_STRING_LENGTH 500
#define LISTEN_BACKLOG 10

int main(){
    
    SSL_CTX* ctx = InitClientCTX();
    LoadCertificates(ctx);
    
    //create socket
    int socket_d = 0;
    socket_d = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_d == -1){
        cout << "Fail to create a socket" << endl;
    }

    int hostPort = 0;
    string hostIP;
    cout << "Wellcome! Please enter server's port number: ";
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
    sockInfo.sin_family = AF_INET;//IPv4
    sockInfo.sin_addr.s_addr = inet_addr(hostIP.c_str());
    sockInfo.sin_port = htons(hostPort);

    //establish a connection
    int isConnect = connect(socket_d, (struct sockaddr*)&sockInfo, sizeof(sockInfo));
    SSL* ssl;
    if(isConnect == -1){
        cout << "Fail to connect." << endl;
        exit(EXIT_FAILURE);
        close(socket_d);
    }
    else{
        // generate a new SSL based on ctx
        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, socket_d);

        //establish SSL connection
        if (SSL_connect(ssl) == -1){
            ERR_print_errors_fp(stderr);
        }
        else{
            printf("Connected with %s encryption\n", SSL_get_cipher(ssl));
            ShowCerts(ssl);
        }  
    }

    char recvMessage[MAX_STRING_LENGTH];
    SSL_read(ssl, &recvMessage, sizeof(recvMessage));
    cout << recvMessage << endl;

    memset(recvMessage, '\0', sizeof(recvMessage));
    SSL_read(ssl, &recvMessage, sizeof(recvMessage));
    cout << recvMessage << endl;

    string sendMessage;
    //main function
    while(true){
        //function list
        int num1;
        cout << "\n======================\n\nChoose a function:" << endl;
        cout << "1.Register\n2.Login\nYour choice: ";
        cin >> num1;
        while((num1 != 1) && (num1 != 2)){
            cout << endl;
            cout << "Attention! Your answer should be 1 or 2." << endl;
            cout << "Please try again: ";
            cin >> num1;
        }
        cin.ignore(MAX_STRING_LENGTH,'\n');
        memset(recvMessage, '\0', sizeof(recvMessage));
        sendMessage.clear();
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

                sendMessage = "REGISTER#" + userName + "#"+ deposit;
                SSL_write(ssl, sendMessage.c_str(), sendMessage.length());
                SSL_read(ssl, &recvMessage, sizeof(recvMessage));
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
                while (stoi(portNum) < 1025 || stoi(portNum) > 65535)
                {
                    cout << "Port Number should be in range 1025 ~ 65535.\n";
                    cout << "Please enter the client's port number: ";
                    cin >> portNum;
                    cin.ignore(MAX_STRING_LENGTH,'\n');
                }

                sendMessage = userName + "#" + portNum;
                SSL_write(ssl, sendMessage.c_str(), sizeof(sendMessage));
                SSL_read(ssl, &recvMessage, sizeof(recvMessage));
                cout << recvMessage;

                if(strcmp(recvMessage,"220 AUTH_FAIL\n") == 0){
                    cout << endl;
                    cout << "This account has been logged in!" << endl; 
                    cout << "OR The account haven't been registered!"<< endl;
                    cout << "Please ask the server for more information." << endl;
                    exit(EXIT_FAILURE);
                }
                else{
                    pid_t pid;
                    pid = fork();
                    if(pid < 0){
                        cout << "fork doesn't exist!!";
                    }
                    else if (pid == 0){//I'm child fork, I'm listening
                    
                        SSL_CTX* ctx_B2A = InitServerCTX();
                        LoadCertificates(ctx_B2A);

                        int sock_toListen = 0;
                        sock_toListen = socket(AF_INET, SOCK_STREAM, 0);

                        if (sock_toListen == -1){
                            cout << "Fail to create a socket!!" << endl;
                        }

                        //Setting socket information
                        struct sockaddr_in sock_toListen_info;
                        sock_toListen_info.sin_family = AF_INET;//IPv4
                        sock_toListen_info.sin_addr.s_addr = inet_addr("127.0.0.1");
                        sock_toListen_info.sin_port = htons(atoi(portNum.c_str()));

                        int isbind = bind(sock_toListen, (struct sockaddr*) &sock_toListen_info, sizeof(sock_toListen_info));
                        if (isbind < 0){
                            cout << "Failed to bind!!\n";
                            exit(EXIT_FAILURE);
                        }  
                        listen(sock_toListen, LISTEN_BACKLOG);
                        while(true){
                            int sock_forClient;                            
                            sock_forClient = accept(sock_toListen, NULL, NULL);
                            if(sock_forClient == -1){
                                cout << "Fail to accept!";
                                close(sock_forClient);
                                continue;
                            }
                            else{
                                SSL* ssl_B2A = SSL_new(ctx_B2A);
                                SSL_set_fd(ssl_B2A, sock_forClient);
                                if (SSL_accept(ssl_B2A) == -1){
                                    ERR_print_errors_fp(stderr);
                                    close(sock_forClient);
                                    continue;
                                }

                                ShowCerts(ssl_B2A);
                                memset(recvMessage, '\0', sizeof(recvMessage));
                                string sendToPeer = "Connected to peer!!!\n";
                                
                                SSL_write(ssl_B2A, sendToPeer.c_str(), sizeof(sendToPeer));
                                SSL_read(ssl_B2A, &recvMessage, sizeof(recvMessage));
                                
                                cout << "\n===========Transaction Request===========\n\n" << recvMessage << "\n========================================\n\n";
                                memset(recvMessage, '\0', sizeof(recvMessage));

                                SSL_shutdown(ssl_B2A);
                                SSL_free(ssl_B2A);
                                close(sock_forClient);
                            }
                        } 
                        cout << "child say goodbye ~ " << endl; 
                    }
                    else{//I'm parent fork
                        while(true){
                            int num2;
                            memset(recvMessage, '\0', sizeof(recvMessage));
                            sendMessage.clear();
                            cout << "\n======================\n\nChoose a function:\n1.Query Accounts information\n2.Exit\n3.Micropayment transaction\nYour choice: ";
                            cin >> num2;
                            while((num2 != 1) && (num2 != 2) && (num2 != 3)){
                                cout << endl;
                                cout << "Attention! Your answer should be 1, 2, or 3." << endl;
                                cout << "Please try again: ";
                                cin >> num2;
                            }
                            cin.ignore(MAX_STRING_LENGTH,'\n');


                            switch (num2){
                                //Query Accounts information
                                case 1:
                                {
                                    sendMessage = "List";
                                    SSL_write(ssl, sendMessage.c_str(), sizeof(sendMessage));
                                    SSL_read(ssl, &recvMessage, sizeof(recvMessage));
                                    cout << recvMessage;
                                    break;                
                                }

                                //Exit
                                case 2:
                                {
                                    sendMessage = "Exit";
                                    SSL_write(ssl, sendMessage.c_str(), sizeof(sendMessage));
                                    SSL_read(ssl, &recvMessage, sizeof(recvMessage));
                                    cout << recvMessage;               
                                    break;
                                }

                                //Micropayment transaction
                                case 3:
                                {  
                                    //create socket to connect with other's child
                                    SSL_CTX* ctx_toPeer = InitClientCTX();
                                    LoadCertificates(ctx_toPeer);

                                    int socket_toPeer = 0;
                                    socket_toPeer = socket(AF_INET, SOCK_STREAM, 0);

                                    if (socket_toPeer == -1){
                                        cout << "Fail to create a socket to peer!!" << endl;
                                    }

                                    string peerName;
                                    cout << "Who do you want to transact with? ";
                                    cin >> peerName;
                                    cin.ignore(MAX_STRING_LENGTH,'\n');

                                    //take list to parse
                                    SSL_write(ssl, "List", sizeof("List"));
                                    SSL_read(ssl, &recvMessage, sizeof(recvMessage));

                                    // parse server's list
                                    string to_parse(recvMessage);
                                    int account_list_begin = 0;
                                    while (to_parse[account_list_begin++] != '\n') {
                                        continue;
                                    }

                                    while (to_parse[account_list_begin++] != '\n') {
                                        continue;
                                    }

                                    string peerIP = "";
                                    string peerPort = "";
                                    string temp = "";
                                    while ((peerIP == "" && peerPort == "") && account_list_begin < to_parse.length())
                                    {
                                        // parse name
                                        while (to_parse[account_list_begin] != '#') { 
                                            temp += to_parse[account_list_begin];
                                            account_list_begin++;
                                        }
                                        account_list_begin++;
                                        if (temp == peerName) {
                                            // parse ip
                                            while (to_parse[account_list_begin] != '#') { 
                                                peerIP += to_parse[account_list_begin];
                                                account_list_begin++;
                                            }
                                            account_list_begin++;
                                            // parse port
                                            while (to_parse[account_list_begin] != '\n') { 
                                                peerPort += to_parse[account_list_begin];
                                                account_list_begin++;
                                            }
                                            account_list_begin++;
                                            break;
                                        }
                                        else { 
                                            // pass this line
                                            temp = "";
                                            while (to_parse[account_list_begin++] != '\n') { 
                                                continue;
                                            }
                                        }
                                    }
                                    if(temp != peerName){
                                        cout << "client hasn't logged in!" << endl;
                                        continue;
                                    }
                                    //end parsing



                                    //Setting socket information
                                    struct sockaddr_in sock_toPeer_info;
                                    sock_toPeer_info.sin_family = AF_INET;//IPv4
                                    sock_toPeer_info.sin_addr.s_addr = inet_addr(peerIP.c_str()); //"127.0.0.1" 
                                    sock_toPeer_info.sin_port = htons(atoi(peerPort.c_str()));
                                    cout << endl;
                                    cout << "List Received:" << endl;
                                    cout << "peer ip: " << peerIP << endl;;
                                    cout << "peer port: " << peerPort << endl;
                                    cout << endl;
                                    
                                    // establish a connection
                                    isConnect = connect(socket_toPeer, (struct sockaddr*)&sock_toPeer_info, sizeof(sock_toPeer_info));
                                    SSL* ssl_toPeer;
                                    if(isConnect == -1){
                                        cout << "Fail to connect with peer!!" << endl;
                                        exit(EXIT_FAILURE);
                                        close(socket_toPeer);
                                    }
                                     else{
                                        // generate a new SSL based on ctx
                                        ssl_toPeer = SSL_new(ctx_toPeer);
                                        SSL_set_fd(ssl_toPeer, socket_toPeer);
                                        //establish SSL connection
                                        if (SSL_connect(ssl_toPeer) == -1){
                                            ERR_print_errors_fp(stderr);
                                        }
                                        else{
                                            printf("Connected with %s encryption\n", SSL_get_cipher(ssl_toPeer));
                                            ShowCerts(ssl_toPeer);
                                        }  
                                    }

                                    memset(recvMessage, '\0', sizeof(recvMessage));
                                    SSL_read(ssl_toPeer, &recvMessage, sizeof(recvMessage));
                                    cout << recvMessage << endl;
                                    
                                    string transFee;
                                    cout << "How much do you want to transact? ";
                                    try{
                                        cin >> transFee;
                                    }
                                    catch(const out_of_range& orr){
                                        cout << "Out of Range error: " << orr.what() << endl;
                                        cout << "OVER LIMIT: single transaction limit is 1 ~ 100000. Try again: ";
                                        cin >> transFee;
                                    }
                                    try{
                                        while(stoi(transFee) > 100000 || stoi(transFee) <= 0){
                                            cout << "OVER LIMIT: single transaction limit is 1 ~ 100000." << endl;
                                            cout << "Try again: " << endl;
                                            cin >> transFee;
                                        }
                                    }
                                    catch(const out_of_range& orr){
                                        cout << "Out of Range error: " << orr.what() << endl;
                                        cout << "OVER LIMIT: single transaction limit is 1 ~ 100000. Try again: ";
                                        cin >> transFee;
                                    }

                                    sendMessage.clear();
                                    sendMessage = userName + "#" + transFee + "#" + peerName + "&";
                                    
                                    //send message to peer
                                    SSL_write(ssl_toPeer, sendMessage.c_str(), sizeof(sendMessage));
                                    close(socket_toPeer);
                                    
                                    //send message to server
                                    SSL_write(ssl, sendMessage.c_str(), sizeof(sendMessage));

                                    //read server's message
                                    memset(recvMessage, '\0', sizeof(recvMessage));
                                    SSL_read(ssl, &recvMessage, sizeof(recvMessage));
                                    cout << recvMessage << endl;

                                    SSL_shutdown(ssl_toPeer);
                                    SSL_free(ssl_toPeer);
                                    SSL_CTX_free(ctx_toPeer);
                                    break;
                                }
                            }
                            //Exit
                            if(num2 == 2){
                                if(strstr(recvMessage, "Bye\n") != NULL){
                                    break;
                                }
                            }
                        }
                        
                    }
                }
                break;
            }    
        }
    }
    close(socket_d);
    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    return 0;
}