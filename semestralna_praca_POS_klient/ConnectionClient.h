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

#ifndef CONNECTIONCLIENT_H
#define CONNECTIONCLIENT_H

using namespace std;

class ConnectionClient {
public:
    ConnectionClient();
    bool sendRequest(int option);
    bool userRegister();
    bool userLogin();
    ConnectionClient(const ConnectionClient& orig);
    int menu();
    void getContacts();
    void sendToServer(string message);
    virtual ~ConnectionClient() {
        delete requests;
        for(auto a : *messages){
            delete a;
        }
        delete messages;
    };
private:
    int sockfd;
    bool logged;
    bool end;
    vector<vector<string>* > * messages;
    string username;
    vector<string> * requests;
    mutex mtx;
    condition_variable cv;
    string response;
};

#endif /* CONNECTIONCLIENT_H */

