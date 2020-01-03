#include <string>
#include <vector>
#include <iostream>

#ifndef MESSAGEREADER_H
#define MESSAGEREADER_H

using namespace std;

class MessageHandler {
public:

    MessageHandler() {
    };

    virtual ~MessageHandler() {
    };

    void readMsg(vector<string>* vecMsg, string msg);
    void printMsg(vector<string>* vecMsg);
    bool isUserAuthentificated(vector<string>* vecMsg);


private:
};

#endif /* MESSAGEREADER_H */

