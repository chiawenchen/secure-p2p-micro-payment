#ifndef _USER_USERLIST_CLASS_H_
#define _USER_USERLIST_CLASS_H_

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include "const.h"
#include "clientClass.h"
using namespace std;

class User{
public:
    User();
    User(string name, int money, string IP, string port, bool isOnline);
    string getName();
    string getIP();
    string getPort();
    int getMoney();
    bool getIsOnline();

    void setName(const string name);
    void setIP(const string IP);
    void setPort(const string port);
    void setMoney(const int money);
    void setIsOnline(const bool online);
    friend class UserList;
private:
    string port;
    string IP;
    string name;
    int money;
    bool isOnline;
};

class UserList{
private:
    vector<User*> usersListPtr;
public:
    int getOnlineNum();
    UserList();
    User* findUser(string name);
    bool regiserUpdate(string clientMessage);
    User* loginUpdate(string clientMessage, Client aClient);
    string list(User* aUser);
    bool exit(User* aUser);
    int transact(string clientMessage);
};

#endif