#include "MessageHandler.h"

//#define DEBUG

#ifdef DEBUG
#define DEBUG_MSG(str) do { std::cout << str << std::endl; } while( false )
#else
#define DEBUG_MSG(str) do { } while ( false )
#endif

using namespace std;

const string delim = ";";

void MessageHandler::printMsg(vector<string>* parsedMsg) {
    int counter = 1;

    DEBUG_MSG("Message START");
    for (auto i : *parsedMsg) {
        cout << to_string(counter) << " -> " << i << endl;
        counter++;
    }
    DEBUG_MSG("Message END");
}

string MessageHandler::createMsg(vector<string>* responseMsg) {
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
            //            cout << "Not authentificated" << endl;
            return false;
        }
    } catch (const std::exception& e) {
        cout << "I cannot determine is message authenticated or not" << endl;
        cerr << e.what();
        return false;
    }
    return true;
}

void MessageHandler::sendMsg(int socket, string msg) {
    const char *msgToSocket = msg.c_str();
    this->sendMsg(socket, msgToSocket);
}

void MessageHandler::sendMsg(int socket, const char * msg) {
    DEBUG_MSG("Sending response with const char" << msg << " on socket " << socket);
    if (socket != -1) {
        int n = write(socket, msg, strlen(msg) + 1);
    }
}

void MessageHandler::sendTrue(int socket) {
    //    cout << "Sending TRUE" << endl;
    this->sendMsg(socket, this->MSG_TRUE);
}

void MessageHandler::sendTrue(int socket, string additionalInfo) {
    //    cout << "Sending TRUE with aditional info" << endl;
    string msg = this->MSG_TRUE + delim + additionalInfo.c_str();
    this->sendMsg(socket, msg);
}

void MessageHandler::sendFalse(int socket, string additionalInfo) {
    //    cout << "Sending FALSE because: " << additionalInfo << endl;
    string msg = this->MSG_FALSE + delim + additionalInfo.c_str();
    this->sendMsg(socket, msg);
}