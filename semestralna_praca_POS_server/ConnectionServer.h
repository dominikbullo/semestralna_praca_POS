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
#include "MessageReader.h"

#ifndef SERVERCONNECTION_H
#define SERVERCONNECTION_H

using namespace std;

class ConnectionServer {
public:
    ConnectionServer();
    virtual ~ConnectionServer();
    void controlUser(int socket);
    bool registerUser(vector<string> parsedMsg);
    int loginUser(vector<string> parsedMsg, int socket);
    bool sendMsg(vector<string> parsedMsg, ConnectedUser* user);

private:
    vector<ConnectedUser*> * allUsers;
    vector<ConnectedUser*> * onlineUsers;
    MessageReader* messageReader;
    mutex mtx;
};

#endif /* SERVERCONNECTION_H */

