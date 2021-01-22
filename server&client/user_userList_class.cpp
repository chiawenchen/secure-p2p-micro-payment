#include "user_userList_class.h"
#include <bits/stdc++.h>

User::User(){
    this->money = 0;
    this->name = "";
    this->IP = "";
    this->port = "";
    this->isOnline = false;
}

User::User(string name, int money, string IP, string port, bool isOnline){
    this->money = money;
    this->name = name;
    this->IP = IP;
    this->port = port;
    this->isOnline = isOnline;  
}

string User::getName(){
    return this->name;
}

string User::getIP(){
    return this->IP;
}

string User::getPort(){
    return this->port;
}

int User::getMoney(){
    return this->money;
}

bool User::getIsOnline(){
    return this->isOnline;
}

void User::setName(const string name)
{
    this->name = name;
}

void User::setIP(const string IP){
    this->IP = IP;
}

void User::setPort(const string port)
{
    this->port = port;
}

void User::setMoney(const int money){
    this->money = money;
}

void User::setIsOnline(const bool online)
{
    this->isOnline = online;
}


UserList::UserList(){
    usersListPtr.clear();
}

User* UserList::findUser(string userName){
    int len = this->usersListPtr.size();
    for(int i = 0; i < len; i++){
        if(this->usersListPtr[i]->name == userName){
            return this->usersListPtr[i];
        }
    }
    return nullptr;
}

int UserList::getOnlineNum(){
    int cnt = 0;
    int len = this->usersListPtr.size();
    for(int i = 0; i < len ; i++){
        if(this->usersListPtr[i]->getIsOnline()){
            cnt++;
        }
    }
    return cnt;
}

bool UserList::regiserUpdate(string clientMessage){
    string mainMsg = clientMessage.substr(9);
    int balanceMsg = mainMsg.find("#") + 1;

    string name = mainMsg.substr(0, balanceMsg - 1);
    string balance = mainMsg.substr(balanceMsg);
    int balance_int = atoi(balance.c_str());

    if (this->findUser(name) == nullptr){
        User* newUser = new User(name, balance_int, "", "", false);
        this->usersListPtr.push_back(newUser);
        return true;
    }
    else{
        return false;
    }
}

User* UserList::loginUpdate(string clientMessage, Client aClient){
    int portMsg = clientMessage.find("#") + 1;
    string name = clientMessage.substr(0, portMsg - 1);
    string port = clientMessage.substr(portMsg);

    User* aUser = this->findUser(name);
    if (aUser != nullptr){
        if(aUser->isOnline == false){
            aUser->setIsOnline(true);
            aUser->setPort(port);
            aUser->setIP(aClient.getIP());
            return aUser;
        }
        else{
            cout << "This account has been logged in!";
            return nullptr;
        }    
    }
    else{
        cout << "The account haven't been registered!\n";
        return nullptr;
    }  
    return nullptr;
}

string UserList::list(User* aUser){
    int len = this->usersListPtr.size();

    string listMessage = "";
    listMessage += to_string(aUser->getMoney());
    listMessage += "\n";
    listMessage += to_string(this->getOnlineNum());
    listMessage += "\n";

    for(int i = 0; i < len; i++){
        if((usersListPtr[i]->isOnline) == true){

            listMessage += usersListPtr[i]->getName();
            listMessage += "#";
            listMessage += usersListPtr[i]->getIP();
            listMessage += "#";
            listMessage += usersListPtr[i]->getPort();
            listMessage += "\n";
        }
    }
    cout << "list Message Preview: " << listMessage << endl;
    return listMessage;
}

bool UserList::exit(User* aUser){
    if(aUser->getIsOnline() == true){
        aUser->setIsOnline(false);
        return true;
    }
    return false;
}

int UserList::transact(string clientMessage){
    
    //parse transact message
    int foundAt = clientMessage.find("#");
    string pay_peer = clientMessage.substr(0, foundAt);
    string temp = clientMessage.substr(foundAt + 1, clientMessage.length());
    foundAt = temp.find("#");
    string tranAmount_str = temp.substr(0, foundAt);
    temp = temp.substr(foundAt + 1, temp.length());
    string receive_peer = temp.substr(0, temp.find("&"));
    cout << endl;
    cout << "pay peer: " << pay_peer << endl;
    cout << "tranAmount str: " << tranAmount_str << endl;
    cout << "receive peer: " << receive_peer << endl;
    cout << endl;
    int tranAmount = stoi(tranAmount_str);
    
    //update transaction information in server 
    User* payUser = this->findUser(pay_peer);
    User* receiveUser = this->findUser(receive_peer);
    
    int scenario = 0; 
    if(payUser->getMoney() - tranAmount < 0){
        scenario = 1;
        cout << "CANNOT TRANSACT: pay_peer doesn't have enough money." << endl;
    }
    if(receiveUser->getMoney() > INT_MAX - tranAmount){
        scenario = 2;
        cout << "CANNOT TRANSACT: over deposit limit." << endl;
    }

    if(scenario == 0){
        payUser->setMoney(payUser->getMoney() - tranAmount);
        receiveUser->setMoney(receiveUser->getMoney() + tranAmount);
        cout << "========================================================" << endl;
        cout << "Congrats ~ ~ Transaction Succes ^____________^ !!" << endl;
        cout << "payUser's current money: " << payUser->getMoney() << endl;
        cout << "receiveUser's current money: " << receiveUser->getMoney() << endl;
        cout << "========================================================" << endl;
        return 0;
    }
    else if(scenario == 1){
        return 1;
    }
    else{
        return 2;
    }
}





