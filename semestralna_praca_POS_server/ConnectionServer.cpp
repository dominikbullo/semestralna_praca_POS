#include "ConnectionServer.h"

#define REG 0
#define LOG 1
#define SND 2

ConnectionServer::ConnectionServer() {
    int sockfd, newsockfd, newsockf2;

    socklen_t cli_len;
    struct sockaddr_in serv_addr, cli_addr;

    //vyčistenie (vynulovanie) a inicializácia sieťovej adresy
    bzero((char*) &serv_addr, sizeof (serv_addr));

    //nastavenie pre adresovanie v doméne INET
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(20055);

    //tvorba samotného socketu "sockfd"
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    //priradenie adresy socketu (aj s overením)
    if (bind(sockfd, (struct sockaddr*) &serv_addr, sizeof (serv_addr)) < 0) {
        perror("Error! - cannot bind adresss");
    }

    //umožníme pripojenie klienta na socket "sockfd",
    //druhý parameter je dĺžka fronty na pripojenie k "sockfd"
    listen(sockfd, 10);
    cli_len = sizeof (cli_addr);

    // Creating new thread for every new user
    // Could youse thread pool...but..
    vector<thread> *threads = new vector<thread>();

    while (true) {
        cout << "Waiting for socket";
        newsockfd = accept(sockfd, (struct sockaddr*) &cli_addr, &cli_len);

        thread t(&ConnectionServer::controlUser, this, newsockfd);
        threads->push_back(move(t));
    }

    for (thread &thread : *threads) {
        thread.join();
    }
}

void ConnectionServer::controlUser(int socket) {
    ConnectedUser* user;

    char buffer[256];
    int response;

    vector<string>* parsedMsg = new vector<string>();

    while (true) {
        parsedMsg->clear();

        //čistí buffer
        bzero(buffer, 256);
        //načítava do buffera z newsockfd
        response = read(socket, buffer, 255);

        if (response == 0 || response == -1) {
            break;
        }

        std::cout << std::endl << response << std::endl;
        std::cout << std::endl << string(buffer);

        // parser spravy 
        messageReader->readMsg(parsedMsg, string(buffer)); //zavoal parsovac spravy

        std::cout << std::endl << parsedMsg->at(1) << parsedMsg->at(2);

        const char* msg;
        cout << "Message is" << msg;
        switch (stoi(parsedMsg->at(0))) {
            case 1:
                std::cout << "Received message type 1";
                break;
            default:
                std::cout << "Not implemented yet!";
        }
    }
    
    std::cout << "Closing socket";
    close(socket);
}

ConnectionServer::~ConnectionServer() {
}




