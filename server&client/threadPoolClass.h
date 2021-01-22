#ifndef _THREADPOOLCLASS_H_
#define _THREADPOOLCLASS_H_
#include <pthread.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <string>
#include "const.h"
#include "user_userList_class.h"
#include "clientClass.h"
#include <queue>
#include "SSL.h"
using namespace std;

class ThreadPool{
private:
    int threadNum;
    pthread_mutex_t mutex; 
    pthread_cond_t condition_var;
    pthread_t* threads;
    queue<Client> waitingQueue;
    static void* interface(void* thread);
    void handle(void* thread);
    void function(Client aClient);
    UserList userMap;
public:
    ThreadPool();
    void connect(Client &c);
    void createThreads();
};

#endif
