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


#ifndef SERVERCONNECTION_H
#define SERVERCONNECTION_H
using namespace std;

class ServerConnection {
public:
    ServerConnection();
    virtual ~ServerConnection();
    // ;
    //void showContacts();




};

#endif /* SERVERCONNECTION_H */

