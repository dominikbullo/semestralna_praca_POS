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
#include "MessageReader.h"

#ifndef CONNECTIONCLIENT_H
#define CONNECTIONCLIENT_H

using namespace std;

class ConnectionClient {
public:
    ConnectionClient();
    bool sendRequest(int option);
    bool responseFromServer(string msg);
    ConnectionClient(const ConnectionClient& orig);
    int menu();
    void sendToServer(string message);
    void readResponse();
    virtual ~ConnectionClient();
private:
    int sockfd;
    bool logged;
    bool end;
    vector<vector<string>* > * messages;
    string username;
    vector<string> * requests;
    mutex mtx;
    condition_variable conditionVariable;
    string response;
    //MessageReader* messReader;
};

#endif /* CONNECTIONCLIENT_H */

