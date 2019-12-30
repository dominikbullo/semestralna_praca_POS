#include "ConnectionServer.h"

#define REG 0
#define LOG 1
#define SND 2

using namespace std;

ConnectionServer::ConnectionServer() {
    int sockfd, newsockfd, newsockf2;

    socklen_t cli_len;
    struct sockaddr_in serv_addr, cli_addr;

    //vyčistenie (vynulovanie) a inicializácia sieťovej adresy
    bzero((char*) &serv_addr, sizeof (serv_addr));

    //nastavenie pre adresovanie v doméne INET
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(20045);

    //tvorba samotného socketu "sockfd"
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    //priradenie adresy socketu (aj s overením)
    if (bind(sockfd, (struct sockaddr*) &serv_addr, sizeof (serv_addr)) < 0) {
        perror("Error! - cannot bind adresss");
        perror("Error! - cannot bind adresss");
        perror("Error! - cannot bind adresss");
        perror("Error! - cannot bind adresss");
        perror("Error! - cannot bind adresss");
        perror("Error! - cannot bind adresss");
        perror("Error! - cannot bind adresss");
        perror("Error! - cannot bind adresss");
        perror("Error! - cannot bind adresss");
    }


    //umožníme pripojenie klienta na socket "sockfd",
    //druhý parameter je dĺžka fronty na pripojenie k "sockfd"
    listen(sockfd, 10);
    cli_len = sizeof (cli_addr);

    cout << "SERVER initialized SUCCESSFUL" << endl;

    // Creating new thread for every new user
    // Could youse thread pool...but..
    vector<thread> *threads = new vector<thread>();

    while (true) {
        cout << "Waiting for socket" << endl;
        newsockfd = accept(sockfd, (struct sockaddr*) &cli_addr, &cli_len);

        thread t(&ConnectionServer::controlUser, this, newsockfd);
        threads->push_back(move(t));
        cout << "Created thread for socket" << endl;
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

        cout << endl << response << endl;
        cout << endl << string(buffer) << endl;

        // parser spravy 
        messageReader->readMsg(parsedMsg, string(buffer)); //zavoal parsovac spravy

        cout << endl << parsedMsg->at(1) << " " << parsedMsg->at(2) << endl;

        const char* msg;
        //        cout << "Message is" << msg << endl;
        //        cout << "Message is lalalala" << parsedMsg << endl;
        cout << "parsed " << stoi(parsedMsg->at(0)) << endl;
        cout << "type parsed " << typeid (stoi(parsedMsg->at(0))).name() << endl;

        switch (stoi(parsedMsg->at(0))) {
            case 1:
                // register user
                cout << "Received message type 1" << endl;
                cout << "Need to register user" << endl;
                break;
            case 2:
                // login user
                cout << "Received message type 2" << endl;
                cout << "Need to login user" << endl;
            case 3:
                // received message
                cout << "Received message type 3" << endl;
                cout << "Received messsage to user" << endl;
            case 4:
                // show contact
                cout << "Received message type 4" << endl;
                break;
            case 5:
                // add contact
                cout << "Received message type 5" << endl;
                break;
            case 6:
                // delete contact
                cout << "Received message type 6" << endl;
                break;
            case 10:
                // make contact offline
                cout << "Received message type 10" << endl;
                break;
            default:
                cout << "Not implemented yet!" << endl;
                break;
        }
    }

    cout << "Closing socket";
    close(socket);
}

ConnectionServer::~ConnectionServer() {
}




