#pragma once

#include <string>
#include <vector>
#include <thread>

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>


#ifndef CONNECTEDUSER_H
#define CONNECTEDUSER_H

using namespace std;

class ConnectedUser {
public:
    ConnectedUser(int socket = -1);
    void threadReadFunction();
    virtual ~ConnectedUser();

    inline int getSocket() {
        return socket;
    };

    inline string getUsername() {
        return username;
    };

    inline string getPassword() {
        return password;
    };

    inline int setSocket(int socket) {
        this->socket = socket;
    }

    inline void setUsername(string username) {
        this->username = username;
    };

    inline void setPassword(string password) {
        this->password = password;
    };

    inline vector<string>* getContacts() {
        return contacts;
    };


private:
    int socket;
    string username;
    string password;
    vector<string>* contacts;

};

#endif /* CONNECTEDUSER_H */

