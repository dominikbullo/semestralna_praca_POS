#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <sstream>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "MessageHandler.h"

#ifndef CONNECTIONCLIENT_H
#define CONNECTIONCLIENT_H

using namespace std;

class ConnectionClient {
public:
    ConnectionClient();
    virtual ~ConnectionClient();
    bool sendRequest(int option);
    bool responseFromServer(string message);
    int menu();
    void sendToServer(string message);
    void readResponse();
    string userInput(string description);
private:
    string username;
    string response;
    int sockfd;
    bool isLoggedPerson;
    bool stop;
    vector<vector<string>* > * messages;
    vector<string> * requests;
    mutex mutexBase;
    condition_variable conditionVariable;
    MessageHandler* messageHandler;
};

#endif /* CONNECTIONCLIENT_H */

