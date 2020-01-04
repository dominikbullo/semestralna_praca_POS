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
    void registerUser(vector<string> parsedMsg, int socket);
    int loginUser(vector<string> parsedMsg, int socket);
    void sendMsg(vector<string> parsedMsg, ConnectedUser* user);
    void addToContacts(vector<string> parsedMsg, ConnectedUser* user);
    void deleteFromContacts(vector<string> parsedMsg, ConnectedUser* user);

    inline virtual ~ConnectionServer() {
        delete[] onlineUsers;
        delete allUsers;
        delete msgHandler;
    };

private:
    vector<ConnectedUser*> * allUsers;
    vector<ConnectedUser*> * onlineUsers;
    MessageHandler* msgHandler;
    mutex mtx;
};

#endif /* SERVERCONNECTION_H */

