#include "ConnectedUser.h"

ConnectedUser::ConnectedUser(int socket) {
    this->socket = socket;
    contacts = new vector<string>();
}
void ConnectedUser::threadReadFunction(){
    
}


ConnectedUser::~ConnectedUser() {
}

