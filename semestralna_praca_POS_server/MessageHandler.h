#include <string>       // std::string
#include <vector>       // std::vector
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream, std::stringbuf

#ifndef MESSAGEREADER_H
#define MESSAGEREADER_H

using namespace std;

class MessageHandler {
public:

    MessageHandler() {
    };

    virtual ~MessageHandler() {
    };

    void readMsg(string stringMsg, vector<string>* parsedMsg);
    void printMsg(vector<string>* vecMsg);
    bool isUserAuthentificated(vector<string>* vecMsg);

    string createMsg(vector<string>* responseMsg);

private:
};

#endif /* MESSAGEREADER_H */

