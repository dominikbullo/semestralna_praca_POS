#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H
#include <string>
#include <vector>

using namespace std;

class MessageHandler {
public:

    MessageHandler() {
    };

    inline void readMsg(vector<string>* parsedMsg, string stringMsg) {
        string delim = ";";
        while (stringMsg.size()) {
            int index = stringMsg.find(delim);
            if (index != string::npos) {
                parsedMsg->push_back(stringMsg.substr(0, index));
                stringMsg = stringMsg.substr(index + delim.size());
                if (stringMsg.size() == 0)parsedMsg->push_back(stringMsg);
            } else {
                parsedMsg->push_back(stringMsg);
                stringMsg = "";
            }
        }
    };

    virtual ~MessageHandler() {
    };
private:
};

#endif /* MESSAGEHANDLER_H */

