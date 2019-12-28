#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

#ifndef CONNECTIONCLIENT_H
#define CONNECTIONCLIENT_H

class ConnectionClient {
public:
    ConnectionClient();
    ConnectionClient(const ConnectionClient& orig);
    virtual ~ConnectionClient();
private:
    int sockfd;
};

#endif /* CONNECTIONCLIENT_H */

