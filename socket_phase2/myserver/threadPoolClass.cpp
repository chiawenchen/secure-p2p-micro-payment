#include "threadPoolClass.h"
#include <unistd.h>
ThreadPool::ThreadPool() {
    this->threadNum = 2;
 //   this->freeThreads = this->threadNum; //**
    this->threads = new pthread_t[this->threadNum];
    int isMutex = pthread_mutex_init(&(this->mutex), NULL);
    if(isMutex != 0){
        cout << "Error on locking!\n";
    }
    this->condition_var = PTHREAD_COND_INITIALIZER;
}

void ThreadPool::connect(Client &c){
    pthread_mutex_lock(&(this->mutex));
    this->waitingQueue.push(c);
   // pthread_cond_signal(&(this->condition_var));
    pthread_mutex_unlock(&(this->mutex));
}

void ThreadPool::createThreads(){
    for(int i = 0; i < threadNum; i++){
        pthread_create(&(this->threads[i]), NULL, this->work_static, this);
    }
}

void* ThreadPool::work_static(void* thread){
    ThreadPool* _this = (ThreadPool*)thread;
    _this->work(thread);
    return nullptr;
}

void ThreadPool::work(void* thread){
    ThreadPool* _this = (ThreadPool*)thread;
    while(true){
        Client aClient;
        pthread_mutex_lock(&(this->mutex));
      //  pthread_cond_wait(&(this->condition_var), &(this->mutex));
        if(!(this->waitingQueue.empty())){
            aClient = this->waitingQueue.front();
            this->waitingQueue.pop();
        }
        pthread_mutex_unlock(&(this->mutex));
        //freeThreads--;//**
        //cout << "Current online number: " << this->userMap.getOnlineNum() << endl;
        _this->workFunctions(aClient);

    }
}

void ThreadPool::workFunctions(Client aClient){
    if(aClient.sockfd == 0){
         return;
    }
    cout << "Handling...." << endl;
    //cout << "Current online number: " << this->userMap.getOnlineNum() << endl;
    int client_socket = aClient.sockfd;
    char recvMessage[CSTRING_MAX_SIZE] = {};
    memset(recvMessage, '\0', sizeof(recvMessage)); 

    char serverMessage[CSTRING_MAX_SIZE*10] = {}; //*10
    memset(serverMessage, '\0', sizeof(serverMessage));
    string msg = "Connection Acceptted !!";
    send(client_socket, msg.c_str(), msg.length(), 0);

    User* aUser = nullptr;
    string message = "";
    while(true){
        recv(client_socket, recvMessage, sizeof(recvMessage), 0);
        string clientMessage(recvMessage); 
        cout << "Client Message: " << recvMessage << endl;
        memset(recvMessage, '\0', sizeof(recvMessage));

        int function = 0;
        //function 1: register, 2: login, 3: query, 4:logout
        if(clientMessage.find("REGISTER") != string::npos){    
            function = 1;
        }
        else if(clientMessage.find("#") != string::npos){
            function = 2;
        }
        else if(clientMessage.find("Exit") != string::npos){
            function = 3;
        }
        else if(clientMessage.find("List") != string::npos){
            function = 4;
        }

        if(function == 1){//client wants to register
            //message.clear();
            bool isSuccess = userMap.regiserUpdate(clientMessage);
            if(!isSuccess){//userName has been occupied
                message = "210 FAIL\n";
                strcpy(serverMessage, message.c_str());
                cout << "Sorry, this name has been registered. Please try another one.\n";
                //send(client_socket, serverMessage, sizeof(serverMessage), 0);
                send(client_socket, message.c_str(), message.length(), 0);
                memset(serverMessage, '\0', sizeof(serverMessage));
            }
            else{
                //update user map
                message = "100 OK\n";
                strcpy(serverMessage, message.c_str());   
                cout << "Successfully register\n";
                //send(client_socket, serverMessage, sizeof(serverMessage), 0);
                send(client_socket, message.c_str(), message.length(), 0);
                memset(serverMessage, '\0', sizeof(serverMessage));
            }
        }
        else if (function == 2){//client wants to login
            //message.clear();
            aUser = userMap.loginUpdate(clientMessage, aClient);
            if(aUser == nullptr){ //have logged in  or haven't registered
                message = "220 AUTH_FAIL\n";
                strcpy(serverMessage, message.c_str());
                //send(client_socket, serverMessage, sizeof(serverMessage), 0);
                send(client_socket, message.c_str(), message.length(), 0);
                memset(serverMessage, '\0', sizeof(serverMessage));
            }
            else{
                message = userMap.list(aUser);
                strcpy(serverMessage, message.c_str());
                //send(client_socket, serverMessage, sizeof(serverMessage), 0);
                send(client_socket, message.c_str(), message.length(), 0);
                memset(serverMessage, '\0', sizeof(serverMessage));
                cout << "Successfully login!\n";
            }         
        } 

        else if (function == 3){//client wants to logout
            //message.clear();
            bool isExit = userMap.exit(aUser);
            if(isExit){
                message = "Bye\n";
                strcpy(serverMessage, message.c_str());
                send(client_socket, message.c_str(), message.length(), 0);
                memset(serverMessage, '\0', sizeof(serverMessage));
            }
            else{
                cout << "Fail to exit!" << endl;
            }
        }

        else if(function == 4){//client wants to request online list
          //  cout << "in List function\n";
            //message.clear();
            message = userMap.list(aUser);
            strcpy(serverMessage, message.c_str());
            //send(client_socket, serverMessage, sizeof(serverMessage), 0);
            send(client_socket, message.c_str(), message.length(), 0);
            memset(serverMessage, '\0', sizeof(serverMessage));
        }

        else {
            // if (aUser->getIsOnline())
            // {
            //     //bool isExit = userMap.exit(aUser);
            //     message = "Server Message: Client disconnected\n";
            // }
            break;
        }

        cout << "Server Message: " << message;
        message.clear();
    }

    close(client_socket);
}


