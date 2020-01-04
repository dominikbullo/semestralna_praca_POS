#include "MessageHandler.h"

using namespace std;

const string delim = ";";

void MessageHandler::printMsg(vector<string>* parsedMsg) {
    int counter = 1;

    cout << "Message START" << endl << endl;
    for (auto i : *parsedMsg) {
        cout << to_string(counter) << " -> " << i << endl;
        counter++;
    }
    cout << endl << "Message END" << endl;
}

string MessageHandler::createMsg(vector<string>* responseMsg) {
    cout << "Creating msg" << endl;
    string msg;

    for (vector<string>::const_iterator p = responseMsg->begin();
            p != responseMsg->end(); ++p) {
        msg += *p;
        if (p != responseMsg->end() - 1)
            msg += delim;
    }
    return msg;
}

void MessageHandler::readMsg(string stringMsg, vector<string>* parsedMsg) {

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
}

bool MessageHandler::isUserAuthentificated(vector<string>* parsedMsg) {
    int userIsAuthentificated = -1;
    try {
        userIsAuthentificated = stoi(parsedMsg->at(0));
        if (userIsAuthentificated != 1) {
            cout << "Not authentificated" << endl;
            return false;
        }
    } catch (const std::exception& e) {
        cout << "I cannot determine whether a user is authenticated or not" << endl;
        cerr << e.what();
        return false;
    }
    return true;
}