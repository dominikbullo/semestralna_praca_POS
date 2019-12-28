#include "ClientConnection.h"

using namespace std;

ClientConnection::ClientConnection() {

    struct sockaddr_in serv_addr;
    struct hostent* server;

    //printf("idem");
    end = false;
    messages = new vector<vector<string>* >();
    //funkcia knižnice sys/socket.h na získanie "hostname"
    //ako prvý parameter zadávame "localhost"

    server = gethostbyname("localhost");

    bzero((char*) &serv_addr, sizeof (serv_addr));

    serv_addr.sin_family = AF_INET;
    bcopy(
            (char*) server->h_addr,
            (char*) &serv_addr.sin_addr.s_addr,
            server->h_length
            );
    //ako druhý parameter zadávame číslo portu

    serv_addr.sin_port = htons(20055);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    //pripájame sa na sieťovú adresu

    if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof (serv_addr)) < 0) {
        perror("CHYBA!!!");
    }

    delete server;
}

ClientConnection::~ClientConnection() {
}

