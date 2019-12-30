#include "ConnectionServer.h"

#define REG 0
#define LOG 1
#define SND 2

using namespace std;

ConnectionServer::ConnectionServer() {
    // vytvorím si štruktúry
    onlineUsers = new vector<ConnectedUser*>();
    allUsers = new vector<ConnectedUser*>();

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
     cout << "control user on socket " << socket << endl;
    ConnectedUser* user;

    char buffer[256];
    int response;
    const char* msg;
    int pozicia = -1;
    int n;

    vector<string>* parsedMsg = new vector<string>();

    while (true) {
        parsedMsg->clear();
        //čistí buffer
        bzero(buffer, 256);
        //načítava do buffera z newsockfd
        response = read(socket, buffer, 255);

        cout << endl << response << endl;
        cout << endl << string(buffer) << endl;

        if (response == 0 || response == -1) {
            break;
        }

        cout << endl << response << endl;
        cout << endl << string(buffer) << endl;

        // parser spravy 
        messageReader->readMsg(parsedMsg, string(buffer)); //zavoal parsovac spravy

        cout << endl << parsedMsg->at(1) << " " << parsedMsg->at(2) << endl;

        switch (stoi(parsedMsg->at(0))) {
            case 1:
                // register user
                cout << "Received message type 1" << endl;
                cout << "Need to register user" << endl;
                if (registerUser(*parsedMsg)) {
                    msg = "0;T";

                } else {
                    msg = "0;F";
                }

                cout << "sending response " << msg << endl;
                n = write(socket, msg, strlen(msg) + 1); //zasiela správu "msg" klientovi                
                break;
            case 2:
                // login user
                cout << "Received message type 2" << endl;
                cout << "Need to login user" << endl;
                int i;
                cout << "logging user on socket " << socket << endl;
                if ((i = loginUser(*parsedMsg, socket)) != -1) {
                    pozicia = i;
                    user = allUsers->at(i);
                    msg = "0;T";
                } else {
                    msg = "0;F";
                }

                cout << "sending response " << msg << endl;
                n = write(socket, msg, strlen(msg) + 1); //zasiela správu "msg" klientovi
                break;
            case 3:
                // received message
                cout << "Received message type 3" << endl;
                cout << "Received messsage to user" << endl;
                break;
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

    cout << "Closing socket" << endl;
    close(socket);
}

bool ConnectionServer::registerUser(vector<string> parsedMsg) {
    string username = parsedMsg.at(1);
    string password = parsedMsg.at(2);
    ConnectedUser * user;
    bool flag;
    cout << "here i am" << endl;
    for (auto a : *allUsers) {
        cout << "for " << endl;
        if (a->getUsername().compare(username) == 0) {
            cout << a->getUsername();
            cout << "here i am and return false" << endl;
            return false;
        }
    }
    user = new ConnectedUser();
    user->setUsername(username);
    user->setPassword(password);
    unique_lock<mutex> lck(mtx);
    allUsers->push_back(user);
    lck.unlock();
    user = nullptr;
    return true;
}

int ConnectionServer::loginUser(vector<string> parsedMsg, int socket) {
    string username = parsedMsg.at(1);
    std::cout << username << endl;

    string password = parsedMsg.at(2);
    std::cout << password << endl;

    int i = -1;
    for (auto a : *allUsers) {
        i++;
        if (a->getUsername().compare(username) == 0 && a->getPassword().compare(password) == 0) {
            if (a->getSocket() == -1) {
                cout << "socket nastavujem na " << socket << endl;
                unique_lock<mutex> lck(mtx);
                a->setSocket(socket);
                onlineUsers->push_back(a);
                lck.unlock();
                cout << "socket nastavený na " << a->getSocket() << endl;
                cout << "push into online users" << endl;
                cout << "return (position?)" << i << endl;
                return i;
            } else {
                cout << "return bcs sck is not -1" << endl;
                return -1;
            }
        }
    }
    cout << "return end" << endl;
    return -1;
}

ConnectionServer::~ConnectionServer() {
}




