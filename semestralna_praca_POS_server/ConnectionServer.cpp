#include "ConnectionServer.h"

#define PORT 20045

// For non-authentificated users
#define REG 1
#define LOG 2
#define EXIT 10

// For authentificated users
#define SND_MSG 1
#define SHOW_CL 2
#define ADD_C 3
#define DELETE_C 4
#define CHECK_FR 5
#define DELETE_ACC 6
#define OFFLINE 10

// For authentificated users
#define INFO_RESPONSE 0
#define MSG_RESPONSE 1
#define CL_RESPONSE 2
#define FR_RESPONSE 3

#define DEBUG

#ifdef DEBUG
#define DEBUG_MSG(str) do { std::cout << str << std::endl; } while( false )
#else
#define DEBUG_MSG(str) do { } while ( false )
#endif

using namespace std;

const string TRUE_RESPONSE = "T";
const string FALSE_RESPONSE = "F";

ConnectionServer::ConnectionServer() {
    // vytvorím si štruktúry
    onlineUsers = new vector<ConnectedUser*>();
    allUsers = new vector<ConnectedUser*>();

    // RES: https://www.bogotobogo.com/Qt/Qt5_QTcpServer_Multithreaded_Client_Server.php
    int sockfd, newsockfd;

    socklen_t cli_len;
    struct sockaddr_in serv_addr, cli_addr;

    //vyčistenie (vynulovanie) a inicializácia sieťovej adresy
    bzero((char*) &serv_addr, sizeof (serv_addr));

    //nastavenie pre adresovanie v doméne INET
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    //tvorba samotného socketu "sockfd"
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    int enable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof (int)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");

    // priradenie adresy socketu (aj s overením)
    // pokúsim sa 10 krát bindovať socker
    int binded;
    for (int a = 0; a < 10; a++) {
        binded = bind(sockfd, (struct sockaddr*) &serv_addr, sizeof (serv_addr));
        if (binded == 0) {
            DEBUG_MSG("SERVER binded socket SUCCESSFUL");
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
    vector<thread> *threads = new vector<thread>();
    while (1) {
        /* accept incoming connections */
        DEBUG_MSG("Waiting for socket...");
        newsockfd = accept(sockfd, (struct sockaddr*) &cli_addr, &cli_len);
        /* start a new thread but do not wait for it */
        DEBUG_MSG("Created thread for socket");
        thread t(&ConnectionServer::controlUser, this, newsockfd);
        t.detach();
        threads->push_back(move(t));
    }

    for (thread &thread : *threads) {
        thread.join();
    }

    // RES: https://stackoverflow.com/questions/10619952/how-to-completely-destroy-a-socket-connection-in-c
    // TODO proper closing
    close(newsockfd);
    close(sockfd);
    delete threads;
}

void ConnectionServer::controlUser(int socket) {
    DEBUG_MSG("Control user on socket " << socket);

    ConnectedUser* user;

    char buffer[256];
    int response;
    int pozicia = -1;
    int n;

    vector<string>* parsedMsg = new vector<string>();
    vector<string>* responseMsg = new vector<string>();

    while (true) {
        int msgType = -1;
        string msgSender = "";
        string msgBody = "";

        responseMsg->clear();
        parsedMsg->clear();
        //čistí buffer
        bzero(buffer, 256);
        //načítava do buffera z newsockfd
        response = read(socket, buffer, 255);

        DEBUG_MSG("Response is " << response);
        if (response == 0 || response == -1) {
            break;
        }

        // parser spravy 
        msgHandler->readMsg(string(buffer), parsedMsg);

        try {
            msgType = stoi(parsedMsg->at(1));
            msgSender = parsedMsg->at(2);
            DEBUG_MSG("Received message type " << to_string(msgType));
        } catch (const std::exception& e) {
            cerr << e.what() << endl;
        }

        if (parsedMsg->size() >= 3) {
            try {
                msgBody = parsedMsg->at(3);
                DEBUG_MSG(endl << msgSender << " " << msgBody << endl);
            } catch (const std::exception& e) {
                cerr << e.what() << endl;
            }
        }

        // TODO
        // unique_lock<mutex> lck(mtx);        
        if (!msgHandler->isUserAuthentificated(parsedMsg)) {
            switch (msgType) {
                case REG:
                    // register user
                    registerUser(*parsedMsg, socket);
                    break;
                case LOG:
                {
                    // login user
                    DEBUG_MSG("Need to login user");
                    int i;
                    DEBUG_MSG("Logging user on socket " << socket);
                    if ((i = loginUser(*parsedMsg, socket)) != -1) {
                        pozicia = i;
                        user = allUsers->at(i);

                        //                        user->getMessages()->push_back("1;T;From;Message;dominik");
                        //                        user->getMessages()->push_back("1;T;From;Message;dominik");
                        //                        user->getMessages()->push_back("3;T;asdasd_dominik");
                        //                        user->getMessages()->push_back("3;T;domin asdas ik");
                        for (int j = 0; j < user->getMessages()->size(); j++) {
                            usleep(100);
                            string saved = user->getMessages()->at(j);
                            msgHandler->sendMsg(socket, saved);
                        }
                    }
                    break;
                }
                case EXIT:
                    // login user
                    DEBUG_MSG("Need to offline user");
                    msgHandler->sendTrue(socket);
                    break;
                default:
                    cout << "Message type " << msgType << "not implemented yet!" << endl;
                    break;
            }
        } else {
            switch (msgType) {
                case SND_MSG:
                    // received message
                    DEBUG_MSG("Received messsage to user");
                    sendMsg(*parsedMsg, user);
                    break;
                case SHOW_CL:
                    // show contact
                    DEBUG_MSG("Show conctacts");
                    responseMsg->push_back(to_string(CL_RESPONSE));

                    if (user->getContacts()->size() < 1) {
                        responseMsg->push_back(FALSE_RESPONSE);
                        responseMsg->push_back("The user has no contacts");
                    } else {
                        responseMsg->push_back(TRUE_RESPONSE);
                        for (int i = 0; i < user->getContacts()->size(); i++) {
                            DEBUG_MSG("User contacts " << user->getContacts()->at(i));
                            responseMsg->push_back(user->getContacts()->at(i));
                        }
                    }
                    msgHandler->sendMsg(socket, msgHandler->createMsg(responseMsg));
                    break;
                case ADD_C:
                    // add contact
                    DEBUG_MSG("Add concact");
                    addToContacts(*parsedMsg, user);
                    break;
                case CHECK_FR:
                    // odpoved na request o pridani do priatelov
                    user->getContacts()->push_back(parsedMsg->at(2));
                    // TODO nie hocikoho
                    for (auto a : *allUsers) {
                        if (a->getUsername().compare(parsedMsg->at(2)) == 0) {
                            unique_lock<mutex> lck(mtx);
                            a->getContacts()->push_back(user->getUsername());
                            lck.unlock();
                            msgHandler->sendTrue(socket, "User SUCCESSFUL added to contact list");
                            break;
                        }
                    }
                    break;
                case DELETE_C:
                    // delete contact
                    DEBUG_MSG("Delete contact");
                    deleteFromContacts(*parsedMsg, user);
                    break;
                case OFFLINE:
                    // make contact offline
                    DEBUG_MSG("Make contact offline");
                    for (int i = 0; i < onlineUsers->size(); i++) {
                        if ((*onlineUsers)[i]->getUsername() == user->getUsername()) {
                            unique_lock<mutex> lck(mtx);
                            onlineUsers->erase(onlineUsers->begin() + i);
                            lck.unlock();
                            break;
                        }
                    }

                    user->setSocket(-1);
                    user = nullptr;
                    msgHandler->sendTrue(socket, "User SUCCESSFUL deleted");
                    break;
                case DELETE_ACC:
                    // make contact offline
                    DEBUG_MSG("Delete user account");
                    for (int i = 0; i < allUsers->size(); i++) {
                        if ((*allUsers)[i]->getUsername() == user->getUsername()) {
                            unique_lock<mutex> lck(mtx);
                            allUsers->erase(allUsers->begin() + i);
                            lck.unlock();
                            break;
                        }

                    }
                    user->setSocket(-1);
                    user = nullptr;
                    msgHandler->sendTrue(socket, "User deleted");
                    break;
                default:
                    cout << "Message type " << msgType << "not implemented yet!" << endl;
                    break;
            }
        }
        //                lck.unlock();
    }

    delete responseMsg;
    delete parsedMsg;
    if (user != nullptr) {
        for (int i = 0; i < onlineUsers->size(); i++) {
            if ((*onlineUsers)[i]->getUsername() == user->getUsername()) {
                unique_lock<mutex> lck(mtx);
                onlineUsers->erase(onlineUsers->begin() + i);
                lck.unlock();
                break;
            }
        }
        user->setSocket(-1);
    }

    cout << "Closing socket number " << socket << endl;
    close(socket);
}

void ConnectionServer::registerUser(vector<string> parsedMsg, int socket) {
    cout << "REGISTERING user on socket " << socket << endl;

    string username = parsedMsg.at(2);
    string password = parsedMsg.at(3);

    ConnectedUser * user;
    bool flag;
    for (ConnectedUser* user : *allUsers) {
        if (user->getUsername().compare(username) == 0) {
            cout << user->getUsername();
            msgHandler->sendFalse(socket, "Username already exist");
            return;
        }
    }
    user = new ConnectedUser();
    user->setUsername(username);
    user->setPassword(password);
    unique_lock<mutex> lck(mtx);
    allUsers->push_back(user);
    lck.unlock();
    user = nullptr;

    msgHandler->sendTrue(socket, "User created");
}

int ConnectionServer::loginUser(vector<string> parsedMsg, int socket) {
    string username = parsedMsg.at(2);
    string password = parsedMsg.at(3);

    int i = -1;
    for (ConnectedUser* user : *allUsers) {
        i++;
        if (user->getUsername().compare(username) == 0 && user->getPassword().compare(password) == 0) {
            if (user->getSocket() == -1) {
                user->setSocket(socket);
                unique_lock<mutex> lck(mtx);
                onlineUsers->push_back(user);
                lck.unlock();
                msgHandler->sendTrue(socket);
                return i;
            } else {
                msgHandler->sendFalse(socket, "Username already connected");
                return -1;
            }
        }
    }
    msgHandler->sendFalse(socket, "Username and password do not match");
    return -1;
}

void ConnectionServer::sendMsg(vector<string> parsedMsg, ConnectedUser * user) {
    ConnectedUser* toUser = nullptr;
    bool flag = false;
    string toUserUsername = parsedMsg.at(3);

    for (ConnectedUser* user : *allUsers) {
        // needs to be online
        if (user->getUsername().compare(toUserUsername) == 0) {
            cout << "Found online user with username " <<
                    user->getUsername() << endl;

            // Needs to be in my contacts
            for (int i = 0; i < user->getContacts()->size(); i++) {
                toUser = user;
                flag = true;
                cout << "Found user in your contact list" << endl;
                break;
            }
        }

        if (flag) {
            break;
        }
    }

    if (toUser == nullptr || !flag) {
        std::stringstream ss;
        ss << "User " << toUserUsername << " not found in contact list";
        msgHandler->sendFalse(user->getSocket(), ss.str());
        return;
    }

    // TODO via MSG_HANDLER
    int socket = toUser->getSocket();
    vector<string>* responseMsg = new vector<string>();
    
    responseMsg->push_back(to_string(MSG_RESPONSE));
    responseMsg->push_back(TRUE_RESPONSE);
    // from 
    responseMsg->push_back(parsedMsg.at(2));
    // message
    responseMsg->push_back(parsedMsg.at(4));

    if (socket != -1) {
        DEBUG_MSG("test3");
        msgHandler->sendMsg(socket, msgHandler->createMsg(responseMsg));
    } else {
        // user is OFFLINE -> add into his vector of messages
        DEBUG_MSG("Push into toUser messages");
        toUser->getMessages()->push_back(msgHandler->createMsg(responseMsg));
    }
}

void ConnectionServer::addToContacts(vector<string> parsedMsg, ConnectedUser * user) {
    vector<string>* responseMsg = new vector<string>();
    vector<string>* contacts = user->getContacts();
    string usernameToAdd = parsedMsg.at(2);
    bool found = false;

    // sam seba    
    if (usernameToAdd == user->getUsername()) {
        msgHandler->sendFalse(user->getSocket(), "Cannot add yourself to contact list");
        return;
    }

    // already in CL
    for (int i = 0; i < contacts->size(); i++) {
        if (contacts->at(i).compare(usernameToAdd) == 0) {
            msgHandler->sendFalse(user->getSocket(), "User is already a friend");
            return;
        }
    }

    for (int i = 0; i < allUsers->size(); i++) {
        if (allUsers->at(i)->getUsername().compare(usernameToAdd) == 0) {
            found = true;

            responseMsg->push_back(to_string(FR_RESPONSE));
            responseMsg->push_back(TRUE_RESPONSE);
            responseMsg->push_back(user->getUsername());
            if (allUsers->at(i)->getSocket() != -1) {
                msgHandler->sendMsg(allUsers->at(i)->getSocket(), msgHandler->createMsg(responseMsg));
            } else {
                allUsers->at(i)->getMessages()->push_back(msgHandler->createMsg(responseMsg));
                DEBUG_MSG("User is offline");
            }
        }
    }

    if (!found) {
        msgHandler->sendFalse(user->getSocket(), "Username does not exist");
    } else {
        msgHandler->sendTrue(user->getSocket(), "Friend request send!");
    }

    delete responseMsg;
}

void ConnectionServer::deleteFromContacts(vector<string> parsedMsg, ConnectedUser* user) {
    string usernameToDelete = parsedMsg.at(2);
    DEBUG_MSG("Tryi'n delete " << usernameToDelete);

    for (int i = 0; i < user->getContacts()->size(); i++) {
        if (usernameToDelete != user->getUsername()) {
            user->getContacts()->erase(user->getContacts()->begin() + i);
        }
    }

    ConnectedUser* userD = nullptr;

    for (auto a : *allUsers) {
        if (a->getUsername().compare(usernameToDelete) == 0) {
            userD = a;
            for (int j = 0; j < a->getContacts()->size(); j++) {
                if (a->getContacts()->at(j).compare(user->getUsername()) == 0) {
                    DEBUG_MSG("Deleting contact " << user->getUsername());
                    a->getContacts()->erase(a->getContacts()->begin() + j);
                }
            }
            break;
        }
    }

    if (userD == nullptr) {
        DEBUG_MSG("Username do not exist");
        msgHandler->sendFalse(user->getSocket(), "Cannot delete user. Username do not exist");
        return;
    } else {
        msgHandler->sendTrue(user->getSocket(), "User has been deleted");
    }

    vector<string>* responseMsg = new vector<string>();
    responseMsg->push_back(to_string(SND_MSG));
    responseMsg->push_back(TRUE_RESPONSE);
    responseMsg->push_back(user->getUsername());
    responseMsg->push_back("I have deleted you from my contacts");
    responseMsg->push_back(user->getUsername());

    int s = userD->getSocket();
    if (s != -1) {
        msgHandler->sendMsg(s, msgHandler->createMsg(responseMsg));
    } else {
        userD->getMessages()->push_back(msgHandler->createMsg(responseMsg));
    }
}

ConnectionServer::~ConnectionServer() {
    DEBUG_MSG("Destructor");
    delete[] onlineUsers;
    delete allUsers;
    delete msgHandler;
};
