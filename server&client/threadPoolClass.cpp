#include "threadPoolClass.h"
#include <unistd.h>
#define THREAD_NUM 2

ThreadPool::ThreadPool() {
    this->threadNum = THREAD_NUM;
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
    pthread_mutex_unlock(&(this->mutex));
}

void ThreadPool::createThreads(){
    for(int i = 0; i < threadNum; i++){
        pthread_create(&(this->threads[i]), NULL, this->interface, this);
    }
}

void* ThreadPool::interface(void* thread){
    ThreadPool* _this = (ThreadPool*)thread;
    _this->handle(thread);
    return nullptr;
}

void ThreadPool::handle(void* thread){
    ThreadPool* _this = (ThreadPool*)thread;
    while(true){
        Client aClient;
        pthread_mutex_lock(&(this->mutex));
        if(!(this->waitingQueue.empty())){
            aClient = this->waitingQueue.front();
            this->waitingQueue.pop();
        }
        pthread_mutex_unlock(&(this->mutex));
        _this->function(aClient);

    }
}

void ThreadPool::function(Client aClient){
    if(aClient.getSSL() == nullptr){
         return;
    }
    cout << "Handling...." << endl;
    SSL* clientSSL = aClient.getSSL();
    char recvMessage[CSTRING_MAX_SIZE] = {};
    memset(recvMessage, '\0', sizeof(recvMessage)); 

    char serverMessage[CSTRING_MAX_SIZE*10] = {};
    memset(serverMessage, '\0', sizeof(serverMessage));
    string msg = "Connection Acceptted !";
    SSL_write(clientSSL, msg.c_str(), msg.length());

    User* aUser = nullptr;
    string message = "";
    while(true){
        SSL_read(clientSSL, recvMessage, sizeof(recvMessage));
        string clientMessage(recvMessage); 
        cout << "Client Message: " << clientMessage << endl;
        memset(recvMessage, '\0', sizeof(recvMessage));

        int function = 0;
        //function 1: register, 2: login, 3: query, 4:Exit, 5:microTransaction
        if(clientMessage.find("REGISTER") != string::npos){    
            function = 1;
        }

        else if(clientMessage.find("&") != string::npos){
            cout << "Server is updating transaction amount..." << endl;
            function = 5;
        }

        else if(clientMessage.find("#") != string::npos){
            function = 2;
        }

        if(clientMessage.find("Exit") != string::npos){
            function = 3;
        }

        if(clientMessage.find("List") != string::npos){
            function = 4;
        }
        
        //Register
        if(function == 1){
            bool isSuccess = userMap.regiserUpdate(clientMessage);
            //userName has been occupied
            if(!isSuccess){
                message = "210 FAIL\n";
                cout << "Sorry, this name has been registered. Please try another one.\n";
                SSL_write(clientSSL, message.c_str(), message.length());
            }
            else{
                message = "100 OK\n";
                cout << "Successfully register\n";
                SSL_write(clientSSL, message.c_str(), message.length());
            }
        }

        //Login
        else if (function == 2){
            aUser = userMap.loginUpdate(clientMessage, aClient);
            cout << "Updated..." << endl;

            //have logged in  or haven't registered
            if(aUser == nullptr){ 
                message = "220 AUTH_FAIL\n";
                SSL_write(clientSSL, message.c_str(), message.length());
            }
            else{
                message = userMap.list(aUser);
                SSL_write(clientSSL, message.c_str(), message.length());
                cout << "Successfully login!\n";
            }         
        } 

        //Exit
        else if (function == 3){
            bool isExit = userMap.exit(aUser);
            if(isExit){
                message = "Bye\n";
                cout << message;
                SSL_write(clientSSL, message.c_str(), message.length());
            }
            else{
                cout << "Fail to exit!" << endl;
            }
        }

        //Online List
        else if(function == 4){
            message = userMap.list(aUser);
            SSL_write(clientSSL, message.c_str(), message.length());
        }

        //Microtransaction
        else if(function == 5){
            int tranStatus = userMap.transact(clientMessage);
            if(tranStatus == 0){
                message = "Transaction Succeed.";
                SSL_write(clientSSL, message.c_str(), message.length());
            }
            else if(tranStatus == 1){
                message = "CANNOT TRANSACT: pay_peer doesn't have enough money. Try again!";
                SSL_write(clientSSL, message.c_str(), message.length());
            }
            else{
                message = "CANNOT TRANSACT: over deposit limit. Try again!";
                SSL_write(clientSSL, message.c_str(), message.length());
            }
        }

        else {
            if (aUser->getIsOnline())
            {
                message = "Server Message: Client disconnected\n";
                cout << message;
                message.clear();
                break;
            }
        }

        cout << "Server Message: " << message;
        message.clear();
    }

    SSL_shutdown(clientSSL);
    SSL_free(clientSSL);
    close(aClient.sockfd);
}


