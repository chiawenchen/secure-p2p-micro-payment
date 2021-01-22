#include "user_userList_class.h"

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
 //   this->onlineNum = 0;
 //   cout << "Constructor onlineNum: " << this->onlineNum << endl; 
 //   cout << "User List Constructor initializing..." << endl;
}

User* UserList::findUser(string userName){
    int len = this->usersListPtr.size();
    for(int i = 0; i < len; i++){
      //  cout << this->usersListPtr[i]->name << endl;
        if(this->usersListPtr[i]->name == userName){
            //cout << "user exist! ^^\n\n";
            return this->usersListPtr[i];
        }
    }
    //cout << "user doesn't exist! QAQ\n\n";
    return nullptr;
}

int UserList::getOnlineNum(){
    //cout << "Current online number: " << this->onlineNum << endl;
    //return this->onlineNum;
    int cnt = 0;
    int len = this->usersListPtr.size();
    for(int i = 0; i < len ; i++){
        if(this->usersListPtr[i]->getIsOnline()){
            cnt++;
        }
    }
  //  cout << "cnt: " << cnt << endl;
    return cnt;
}

/*void UserList::setOnlineNum(int onlineNum){
    this->onlineNum = onlineNum;
}*/


bool UserList::regiserUpdate(string clientMessage){
    string mainMsg = clientMessage.substr(9);
    int balanceMsg = mainMsg.find("#") + 1;

    string name = mainMsg.substr(0, balanceMsg - 1);
    string balance = mainMsg.substr(balanceMsg);
    int balance_int = atoi(balance.c_str());

    // cout << name << endl;
    // cout << balance << endl;

    if (this->findUser(name) == nullptr){
        User* newUser = new User(name, balance_int, "", "", false);
    //    cout << "newUser: " << newUser->getName() << endl;
    //    cout << "newUser: " << newUser->getMoney() << endl;
        this->usersListPtr.push_back(newUser);
     //   cout << "online number: " << this->onlineNum << endl;
    //    cout << "usersListPtr size: " << this->usersListPtr.size() << endl;
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
         //   cout << "getOnlineNum: " << this->getOnlineNum() << endl;
         //   this->setOnlineNum(this->getOnlineNum() + 1);
         //   cout << "online number: " << this->onlineNum << endl;
         //   cout << "usersListPtr size: " << this->usersListPtr.size() << endl;
            return aUser;
        }
        else{
            cout << "This account has been logged in!";
            return nullptr;
        }    
    }
    else{
        cout << "the account haven't been registered!\n";
        return nullptr;
    }  
    return nullptr;
}

string UserList::list(User* aUser){
    int len = this->usersListPtr.size();

 //   cout << "usersListPtr Size: " << len << endl;
 //   cout << "Preparing list..." << endl;
    string listMessage = "";
    listMessage += to_string(aUser->getMoney());
    listMessage += "\n";
 //   cout << "listMessage 1 : " << listMessage;
    listMessage += to_string(this->getOnlineNum());
    listMessage += "\n";
  //  cout << "listMessage 2 : " << listMessage;    
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
        //  this->setOnlineNum(this->getOnlineNum() - 1);
        return true;
    }
    return false;
}





