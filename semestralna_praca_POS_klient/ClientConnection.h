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
    virtual ~ClientConnection();

private:
    bool logged;
    int sockfd;
    string username;
    bool end;
    vector<vector<string>* > * messages;
    string response;
    condition_variable cv;
    mutex mtx;
};

#endif /* CLIENTCONNECTION_H */

