#include "ConnectedUser.h"

ConnectedUser::ConnectedUser(int socket) {
    this->socket = socket;
    contacts = new vector<string>();
    messages = new vector<string>();
}

ConnectedUser::~ConnectedUser() {
    delete contacts;
    delete messages;    
}

