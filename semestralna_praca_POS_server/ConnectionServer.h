#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <thread>
#include <mutex>

#include "ConnectedUser.h"
#include "MessageHandler.h"

#ifndef SERVERCONNECTION_H
#define SERVERCONNECTION_H

using namespace std;

class ConnectionServer {
public:
    ConnectionServer();
    void controlUser(int socket);
    bool registerUser(vector<string> parsedMsg);
    int loginUser(vector<string> parsedMsg, int socket);
    bool sendMsg(vector<string> parsedMsg, ConnectedUser* user);
    void addToContacts(vector<string> parsedMsg, ConnectedUser* user);
    void deleteFromContacts(vector<string> parsedMsg, ConnectedUser* user);

    inline virtual ~ConnectionServer() {
        delete[] onlineUsers;
        delete allUsers;
        delete messageReader;
    };

private:
    vector<ConnectedUser*> * allUsers;
    vector<ConnectedUser*> * onlineUsers;
    MessageHandler* messageReader;
    mutex mtx;
};

#endif /* SERVERCONNECTION_H */

