#include "ConnectionServer.h"

#define PORT 20049 

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

// For authentificated users
#define INFO_RESPONSE 0
#define MSG_RESPONSE 1
#define CL_RESPONSE 2
#define FR_RESPONSE 3

using namespace std;

const string TRUE_RESPONSE = "T";
const string FALSE_RESPONSE = "F";
int newsockfd;

ConnectionServer::ConnectionServer() {
    // vytvorím si štruktúry
    onlineUsers = new vector<ConnectedUser*>();
    allUsers = new vector<ConnectedUser*>();

    int sockfd, newsockf2;

    socklen_t cli_len;
    struct sockaddr_in serv_addr, cli_addr;

    //vyčistenie (vynulovanie) a inicializácia sieťovej adresy
    bzero((char*) &serv_addr, sizeof (serv_addr));

    //nastavenie pre adresovanie v doméne INET
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

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

    // RES: https://stackoverflow.com/questions/10619952/how-to-completely-destroy-a-socket-connection-in-c
    close(newsockfd);
}

void ConnectionServer::controlUser(int socket) {
    cout << "Control user on socket " << socket << endl;

    ConnectedUser* user;

    char buffer[256];
    int response;
    int pozicia = -1;
    int n;

    vector<string>* parsedMsg = new vector<string>();
    vector<string>* responseMsg = new vector<string>();

    while (true) {
        responseMsg->clear();

        int msgType = -1;
        string msgSender = "";
        string msgBody = "";

        parsedMsg->clear();
        //čistí buffer
        bzero(buffer, 256);
        //načítava do buffera z newsockfd
        response = read(socket, buffer, 255);

        if (response == 0 || response == -1) {
            break;
        }

        // parser spravy 
        msgHandler->readMsg(string(buffer), parsedMsg);

        try {
            msgType = stoi(parsedMsg->at(1));
            msgSender = parsedMsg->at(2);
            cout << "Received message type " << to_string(msgType) << endl;
            cout << endl << msgSender << " " << msgBody << endl << endl;
        } catch (const std::exception& e) {
            cerr << e.what() << endl;
        }
        try {
            msgBody = parsedMsg->at(3);
        } catch (const std::exception& e) {
            cerr << e.what() << endl;
        }
        //        unique_lock<mutex> lck(mtx);
        if (!msgHandler->isUserAuthentificated(parsedMsg)) {
            switch (msgType) {
                case REG:
                    // register user
                    registerUser(*parsedMsg, socket);
                    break;
                case LOG:
                {
                    // login user
                    cout << "Need to login user" << endl;
                    int i;
                    cout << "Logging user on socket " << socket << endl;
                    if ((i = loginUser(*parsedMsg, socket)) != -1) {
                        pozicia = i;
                        user = allUsers->at(i);
                        //                        char* buffer;
                        //                        cout << "sending messages" << endl;
                        //                        for (int j = 0; j < user->getMessages()->size(); j++) {
                        //                            string saved = user->getMessages()->at(j);
                        //                            buffer = &saved[0u];
                        //                            cout << "buffer " << buffer << endl;
                        //                            n = write(socket, buffer, 255); //pošle serveru správu uloženú v bufferi
                        //                        }
                    }
                    break;
                }
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
                    sendMsg(*parsedMsg, user);
                    break;
                case SHOW_CL:
                    // show contact
                    cout << "Show conctacts" << endl;
                    responseMsg->push_back(to_string(CL_RESPONSE));

                    if (user->getContacts()->size() < 1) {
                        responseMsg->push_back(FALSE_RESPONSE);
                        responseMsg->push_back("The user has no contacts");
                    } else {
                        responseMsg->push_back(TRUE_RESPONSE);
                        for (int i = 0; i < user->getContacts()->size(); i++) {
                            cout << "User contacts " << user->getContacts()->at(i) << endl;
                            responseMsg->push_back(user->getContacts()->at(i));
                        }
                    }
                    msgHandler->sendMsg(socket, msgHandler->createMsg(responseMsg));
                    break;
                case ADD_C:
                    // add contact
                    cout << "Add concact" << endl;
                    addToContacts(*parsedMsg, user);
                    break;
                case DELETE_C:
                    // delete contact
                    cout << "Delete contact" << endl;
                    deleteFromContacts(*parsedMsg, user);
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
                    msgHandler->sendTrue(socket);
                    break;
                default:
                    cout << "Not implemented yet!" << endl;
                    break;
            }
        }
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

    msgHandler->sendTrue(socket);
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

    if (toUser == nullptr) {
        std::stringstream ss;
        ss << "User " << user->getUsername() << " not found in contact list";
        msgHandler->sendFalse(socket, ss.str());
    }

    // TODO via MSG_HANDLER
    socket = toUser->getSocket();

    vector<string>* responseMsg = new vector<string>();
    responseMsg->push_back(to_string(MSG_RESPONSE));
    responseMsg->push_back(TRUE_RESPONSE);
    // from 
    responseMsg->push_back(parsedMsg.at(2));
    // message
    responseMsg->push_back(parsedMsg.at(3));

    if (socket != -1) {
        // if is online -> comunikujem priamo na sockete 
        //        buffer = &msg[0u];
        //        //pošle serveru správu uloženú v bufferi 
        //        n = write(socket, buffer, 255);
        msgHandler->sendMsg(socket, msgHandler->createMsg(responseMsg));
    } else {
        // user is OFFLINE -> add into his vector of messages
        cout << "Push into toUser messages" << endl;
        // TODO via message handler
        msg = "1;T;" + parsedMsg.at(2) + ";" + parsedMsg.at(3);
        toUser->getMessages()->push_back(msg);
    }

    msgHandler->sendTrue(user->getSocket());
}

void ConnectionServer::addToContacts(vector<string> parsedMsg, ConnectedUser * user) {
    vector<string>* responseMsg = new vector<string>();
    vector<string>*contacts = user->getContacts();
    string usernameToAdd = parsedMsg.at(2);
    bool found = false;

    msgHandler->printMsg(&parsedMsg);

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
    cout << "Printint all users" << endl;
    for (int i = 0; i < allUsers->size(); i++) {
        cout << i + 1 << " " << allUsers->at(i)->getUsername() << endl;
    }

    for (int i = 0; i < allUsers->size(); i++) {
        cout << i + 1 << " " << allUsers->at(i)->getUsername() << endl;
        cout << "Evidované správy" << endl;
        for (int j = 0; j < allUsers->at(i)->getMessages()->size(); j++) {
            cout << allUsers->at(i)->getMessages()->at(j) << endl;
        }
    }

    for (int i = 0; i < allUsers->size(); i++) {
        cout << "Every user" << endl;
        cout << allUsers->at(i)->getUsername() << endl;
        cout << usernameToAdd << endl;
        cout << allUsers->at(i)->getUsername().compare(usernameToAdd) << endl;

        if (allUsers->at(i)->getUsername().compare(usernameToAdd) == 0) {
            found = true;
            // TODO user is OFFLINE
            if (allUsers->at(i)->getSocket() == -1) {
                string msg = "3;T;" + user->getUsername();
                allUsers->at(i)->getMessages()->push_back(msg);
                cout << "User is offline" << endl;
                break;
            }

            responseMsg->push_back(to_string(FR_RESPONSE));
            responseMsg->push_back(TRUE_RESPONSE);
            responseMsg->push_back(user->getUsername());

            string testMSg = msgHandler->createMsg(responseMsg);
            cout << "testssss " << testMSg << endl;
            msgHandler->sendMsg(allUsers->at(i)->getSocket(), testMSg);
        }
    }

    if (!found) {
        msgHandler->sendFalse(user->getSocket(), "Username does not exist");
    } else {
        msgHandler->sendTrue(user->getSocket(), "Friend request send!");
    }
}

void ConnectionServer::deleteFromContacts(vector<string> parsedMsg, ConnectedUser* user) {
    string send;
    int n;
    char* buff;
    string usernameToDelete = parsedMsg.at(1);

    for (int i = 0; i < user->getContacts()->size(); i++) {
        if (usernameToDelete != user->getUsername()) {
            user->getContacts()->erase(user->getContacts()->begin() + i);
            msgHandler->sendFalse(user->getSocket(), "Cannot delete self");
            return;
        }
    }
//    ConnectedUser* userD;
//    
//    for (auto a : *allUsers) {
//        if (a->getUsername().compare(parsedMsg.at(1)) == 0) {
//            userD = a;
//            for (int j = 0; j < a->getContacts()->size(); j++) {
//                if (a->getContacts()->at(j).compare(user->getUsername()) == 0) {
//                    a->getContacts()->erase(a->getContacts()->begin() + j);
//                }
//            }
//            break;
//        }
//    }
//    
//    int s = userD->getSocket();
//    send = "1;I have deleted you from my contacts;" + user->getUsername();
//    if (s != -1) {
//
//        buff = &send[0u];
//        n = write(s, buff, 255); //pošle serveru správu uloženú v bufferi 
//    } else {
//        userD->getMessages()->push_back(send);
//    }
}

ConnectionServer::~ConnectionServer() {
    delete[] onlineUsers;
    delete allUsers;
    delete msgHandler;
    close(newsockfd);
};
