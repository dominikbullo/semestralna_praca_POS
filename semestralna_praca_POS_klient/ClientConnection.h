#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include "MessageReader.h"
#include <thread>
#include <mutex>
#include <condition_variable>


#ifndef CLIENTCONNECTION_H
#define CLIENTCONNECTION_H
using namespace std;

/*
struct Data{
    bool response = true;;
}data;
 */
class ClientConnection {
public:
    ClientConnection();
    bool sendMessage(string username);
    bool userRegister();
    void menuLogged();
    bool userLogin();
    vector<string> showContacts();
    void addContact();
    void deleteContact();
    int menu();
    virtual ~ClientConnection();
    bool addContact(string pusername);
    void reader();
private:
    bool logged;
    int sockfd;
    string username;
    MessageReader* messReader;
    bool end;
    vector<vector<string>* > * messages;
    string response;
    condition_variable cv;
    mutex mtx;
};

#endif /* CLIENTCONNECTION_H */

