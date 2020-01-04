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

    inline MessageHandler() {
        vector<string> msgTrue = {"0", "T"};
        this->MSG_TRUE = this->createMsg(&msgTrue);

        vector<string> msgFalse = {"0", "F"};
        this->MSG_FALSE = this->createMsg(&msgFalse);
    };

    virtual ~MessageHandler() {
    };

    string createMsg(vector<string>* responseMsg);
    void readMsg(string stringMsg, vector<string>* parsedMsg);
    void printMsg(vector<string>* vecMsg);

    void sendMsg(int socket, string msg);
    void sendMsg(int socket, const char * msg);

    void sendTrue(int socket);

    void sendTrue(int socket, string additionalInfo);
    void sendFalse(int socket, string additionalInfo);

    bool isUserAuthentificated(vector<string>* vecMsg);

private:
    string MSG_TRUE;
    string MSG_FALSE;
};

#endif /* MESSAGEREADER_H */

