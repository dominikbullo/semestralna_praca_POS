#include "MessageHandler.h"

using namespace std;

void MessageHandler::printMsg(vector<string>* vecMsg) {
    int counter = 1;

    cout << "Message START" << endl << endl;
    for (auto i : *vecMsg) {
        cout << to_string(counter) << " -> " << i << endl;
        counter++;
    }
    cout << endl << "Message END" << endl;
}

void MessageHandler::readMsg(vector<string>* vecMsg, string msg) {
    string token = ";";

    while (msg.size()) {
        int index = msg.find(token);
        if (index != string::npos) {
            vecMsg->push_back(msg.substr(0, index));
            msg = msg.substr(index + token.size());
            if (msg.size() == 0)vecMsg->push_back(msg);
        } else {
            vecMsg->push_back(msg);
            msg = "";
        }
    }
}