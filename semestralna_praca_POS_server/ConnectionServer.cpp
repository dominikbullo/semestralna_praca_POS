#include "ConnectionServer.h"

#define REG 0
#define LOG 1
#define SND 2

string username;
string password;

ConnectionServer::ConnectionServer() {
    int sockfd, newsockfd, newsockf2;

    socklen_t cli_len;
    struct sockaddr_in serv_addr, cli_addr;

    int i = 0;
    char buffer[256];
    int n;

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
        perror("CHYBA! - nepodarilo sa priradiť adresu");
    }

    //umožníme pripojenie klienta na socket "sockfd",
    //druhý parameter je dĺžka fronty na pripojenie k "sockfd"

    listen(sockfd, 10);
    cli_len = sizeof (cli_addr);

    vector<thread> *threads = new vector<thread>();

    while (true) {
        cout << "Waiting for socket";
        newsockfd = accept(sockfd, (struct sockaddr*) &cli_addr, &cli_len);

        //        thread t(&ServerConnection::controlUser, this, newsockfd);
        //        threads->push_back(move(t));
    }

    for (thread &a : *threads) {
        a.join();
    }
}

ConnectionServer::~ConnectionServer() {
}




