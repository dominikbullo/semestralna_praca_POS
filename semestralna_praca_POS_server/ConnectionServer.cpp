#include "ConnectionServer.h"

// For non-authentificated users
#define REG 1
#define LOG 2
#define EXIT 10


// For authentificated users
#define SND_MSSG 1
#define SHOW_CL 2
#define ADD_C 3
#define DELETE_C 4
#define OFFLINE 10

const char *MSSG_TRUE = "0;T";
const char *MSSG_FALSE = "0;F";

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
    serv_addr.sin_port = htons(20048);

    //tvorba samotného socketu "sockfd"
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // priradenie adresy socketu (aj s overením)
    // pokúsim sa 10 krát bindovať socker
    int binded;
    for (int a = 0; a < 10; a++) {
        binded = bind(sockfd, (struct sockaddr*) &serv_addr, sizeof (serv_addr));
        if (binded == 0) {
            cout << "SERVER binded socket SUCCESSFUL" << endl;
            break;
        } else {
            perror("Error! - cannot bind adresss");
            cout << binded << endl;
            cout << "Trying again" << endl;
            sleep(1);
        }
    }
    if (binded != 0) {
        exit(EXIT_FAILURE);
    }

    //umožníme pripojenie klienta na socket "sockfd",
    //druhý parameter je dĺžka fronty na pripojenie k "sockfd"
    listen(sockfd, 10);
    cli_len = sizeof (cli_addr);

    cout << "SERVER initialized SUCCESSFUL" << endl;

    // Creating new thread for every new user
    // Could ouse thread pool...but..
    vector<thread> *threads = new vector<thread>();

    while (true) {
        cout << "Waiting for socket..." << endl;
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
    cout << "Control user on socket " << socket << endl;

    ConnectedUser* user;

    char buffer[256];
    int response;
    const char* msg = MSSG_FALSE;
    int pozicia = -1;
    int n;

    vector<string>* parsedMsg = new vector<string>();

    while (true) {
        bool isMessageAuthentificated = false;
        int msgType = -1;
        string msgSender = "";
        string msgBody = "";

        msg = MSSG_FALSE;
        parsedMsg->clear();
        //čistí buffer
        bzero(buffer, 256);
        //načítava do buffera z newsockfd
        response = read(socket, buffer, 255);

        if (response == 0 || response == -1) {
            break;
        }

        // parser spravy 
        messageReader->readMsg(string(buffer), parsedMsg);
        isMessageAuthentificated = messageReader->isUserAuthentificated(parsedMsg);
        messageReader->printMsg(parsedMsg);
        
        
        // TODO make every message same lenght
        try {
            msgType = stoi(parsedMsg->at(1));
            msgSender = parsedMsg->at(2);
            msgBody = parsedMsg->at(3);
        } catch (const std::exception& e) {
            cerr << e.what();
        }

        // vypis spravu
        cout << "Received message type " << to_string(msgType) << endl;
        cout << endl << msgSender << " " << msgBody << endl << endl;

        if (!isMessageAuthentificated) {
            switch (msgType) {
                case REG:
                    // register user
                    cout << "Need to register user" << endl;
                    if (registerUser(*parsedMsg)) {
                        msg = MSSG_TRUE;
                    }
                    break;
                case LOG:
                    // login user
                    cout << "Need to login user" << endl;
                    int i;
                    cout << "logging user on socket " << socket << endl;
                    if ((i = loginUser(*parsedMsg, socket)) != -1) {
                        pozicia = i;
                        user = allUsers->at(i);
                        msg = MSSG_TRUE;
                    }
                    break;
                case EXIT:
                    // login user
                    cout << "Need to offline user" << endl;
                    cout << "Not implemented yet!" << endl;
                    break;
                default:
                    cout << "Not implemented yet!" << endl;
                    break;
            }
        } else {
            switch (msgType) {
                case SND_MSSG:
                    // received message
                    cout << "Received messsage to user" << endl;
                    if (sendMsg(*parsedMsg, user)) {
                        msg = MSSG_TRUE;
                    }
                    break;
                case SHOW_CL:
                    // show contact
                    cout << "Show conctacts" << endl;
                    cout << "Not implemented yet!" << endl;
                    break;
                case ADD_C:
                    // add contact
                    cout << "Add concact" << endl;
                    addToContacts(*parsedMsg, user);
                    break;
                case DELETE_C:
                    // delete contact
                    cout << "Delete contact" << endl;
                    cout << "Not implemented yet!" << endl;
                    break;
                case OFFLINE:
                    // make contact offline
                    cout << "Make contact offline" << endl;
                    
                    // TODO range check
                    for (int i = 0; i < onlineUsers->size(); i++) {
                        if ((*onlineUsers)[i]->getUsername() == user->getUsername()) {
                            onlineUsers->erase(onlineUsers->begin() + i);
                            break;
                        }

                    }
                    user->setSocket(-1);
                    user = nullptr;
                    msg = MSSG_TRUE;
                    break;
                default:
                    cout << "Not implemented yet!" << endl;
                    break;
            }
        }

        cout << "Sending response " << msg << endl;
        n = write(socket, msg, strlen(msg) + 1); //zasiela správu "msg" klientovi     
        //        lck.unlock();
    }

    delete parsedMsg;
    if (user != nullptr) {
        for (int i = 0; i < onlineUsers->size(); i++) {
            if ((*onlineUsers)[i]->getUsername() == user->getUsername()) {
                onlineUsers->erase(onlineUsers->begin() + i);
                break;
            }

        }
        user->setSocket(-1);
    }

    cout << "Closing socket number " << socket << endl;
    close(socket);
}

bool ConnectionServer::registerUser(vector<string> parsedMsg) {
    string username = parsedMsg.at(2);
    string password = parsedMsg.at(3);
    ConnectedUser * user;
    bool flag;
    for (ConnectedUser* user : *allUsers) {
        if (user->getUsername().compare(username) == 0) {
            cout << user->getUsername();
            cout << "Username already exist" << endl;
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
    string username = parsedMsg.at(2);
    std::cout << username << endl;

    string password = parsedMsg.at(3);
    std::cout << password << endl;

    int i = -1;
    for (ConnectedUser* user : *allUsers) {
        i++;
        if (user->getUsername().compare(username) == 0 && user->getPassword().compare(password) == 0) {
            if (user->getSocket() == -1) {
                unique_lock<mutex> lck(mtx);
                user->setSocket(socket);
                onlineUsers->push_back(user);
                lck.unlock();
                //                cout << "socket nastavený na " << a->getSocket() << endl;
                //                cout << "push into online users" << endl;
                //                cout << "return (position?)" << i << endl;
                return i;
            } else {
                //                cout << "return bcs sck is not -1" << endl;
                return -1;
            }
        }
    }
    cout << "return end" << endl;
    return -1;
}

bool ConnectionServer::sendMsg(vector<string> parsedMsg, ConnectedUser * user) {
    ConnectedUser* toUser = nullptr;

    int socket;
    char* buffer;
    string msg;
    int n;
    bool flag = false;
    string toUserUsername = parsedMsg.at(4);

    for (ConnectedUser* user : *allUsers) {
        // needs to be online
        if (user->getUsername().compare(toUserUsername) == 0) {
            cout << "Found online user with username " <<
                    user->getUsername() << endl;

            // TODO DELETE this on production
            toUser = user;
            flag = true;
            // needs to be in my contacts
            //            for (int i = 0; i < user->getContacts()->size(); i++) {
            //                toUser = user;
            //                flag = true;
            //                cout << "Found user in your contact list" << endl;
            //                break;
            //            }
        }

        if (flag) {
            break;
        }
    }

    if (toUser == nullptr) {
        cout << "User " << user->getUsername() << " not found in contact list" << endl;
        return false;
    }

    socket = toUser->getSocket();
    msg = "1;" + parsedMsg.at(3) + ";" + parsedMsg.at(4);

    //    messageReader->printMsg(parsedMsg);

    if (socket != -1) {
        // if is online -> comunikujem priamo na sockete 
        buffer = &msg[0u];
        //pošle serveru správu uloženú v bufferi 
        n = write(socket, buffer, 255);
    } else {
        // user is OFFLINE -> add into his vector of messages
        cout << "Push into toUser messages" << endl;
        toUser->getMessages()->push_back(msg);
    }

    return true;
}

void ConnectionServer::addToContacts(vector<string> parsedMsg, ConnectedUser* user) {
    int n;
    vector<string>*contacts = user->getContacts();
    bool flag = false;

    if (parsedMsg.at(1) != user->getUsername()) {
        for (int i = 0; i < contacts->size(); i++) {
            if (contacts->at(i).compare(parsedMsg.at(1)) == 0) {
                flag = true;
            }
        }
    } else {
        flag = true;

    }
    if (!flag) {
        string msg;
        char* buff;
        for (int i = 0; i < allUsers->size(); i++) {
            if (allUsers->at(i)->getUsername().compare(parsedMsg.at(1)) == 0) {
                msg = "3;" + user->getUsername();
                if (allUsers->at(i)->getSocket() == -1) {
                    allUsers->at(i)->getMessages()->push_back(msg);

                    break;
                }
                msg = "3;" + user->getUsername();
                buff = &msg[0u];
                n = write(allUsers->at(i)->getSocket(), buff, 255);

                break;
            }

        }
    }

}
