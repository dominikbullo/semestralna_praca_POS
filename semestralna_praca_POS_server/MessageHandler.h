#include <string>       // std::string
#include <vector>       // std::vector
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream, std::stringbuf

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#ifndef MESSAGEREADER_H
#define MESSAGEREADER_H

using namespace std;

class MessageHandler {
public:

    MessageHandler() {
    };

    virtual ~MessageHandler() {
    };

    string createMsg(vector<string>* responseMsg);
    void readMsg(string stringMsg, vector<string>* parsedMsg);
    void printMsg(vector<string>* vecMsg);
    void sendMsg(int socket, string msg);
    void sendMsg(int socket, const char * msg);
    void sendTrue(int socket);

    bool isUserAuthentificated(vector<string>* vecMsg);


private:
};

#endif /* MESSAGEREADER_H */

