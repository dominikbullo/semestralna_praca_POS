#include <algorithm>

#include "ConnectionClient.h"

using namespace std;

ConnectionClient::ConnectionClient() {
    struct sockaddr_in serv_addr;
    struct hostent* server;

    messages = new vector<vector<string>* >();
    requests = new vector<string>();
    
    server = gethostbyname("localhost");

    bzero((char*) &serv_addr, sizeof (serv_addr));

    serv_addr.sin_family = AF_INET;
    bcopy(
            (char*) server->h_addr,
            (char*) &serv_addr.sin_addr.s_addr,
            server->h_length
            );
    //ako druhý parameter zadávame číslo portu

    serv_addr.sin_port = htons(20048);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    //pripájame sa na sieťovú adresu

    if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof (serv_addr)) < 0) {
        perror("ERROR!!!");
    }

    thread t(&ConnectionClient::reader, this);
    logged = false;
    while (true) {
        //sendMessage();
        if (end) {
            break;
        }
        menu();
    }
    t.join();
    delete server;
}

void ConnectionClient::reader() {
    char buffer[256];
    vector<string>* parsMsg = new vector<string>();
    int n;
    while (!end) {
        parsMsg->clear();
        n = read(sockfd, buffer, 255);
        messReader->readMsg(parsMsg, string(buffer));
        // cout << "reader switch " << parsMsg->at(0) << " " << parsMsg->at(1) << endl;
        switch (stoi(parsMsg->at(0))) {
            case 0:
            {
                response = parsMsg->at(1);
                cv.notify_all();
                break;
            }
            case 1:
            {
                vector<string>* vec = new vector<string>(*parsMsg);
                messages->push_back(vec);
                break;
            }
            case 2:
            {
                cout << endl << "CONTACTS:" << endl;
                for (int i = 1; i < parsMsg->size(); i++) {
                    if (parsMsg->at(i) != "") {
                        cout << i << "." << parsMsg->at(i) << endl;
                    }

                }
                cv.notify_all();
                break;
            }
            case 3:
                requests->push_back(parsMsg->at(1));
                break;

            case 10:
                end = true;
                // cv.notify_all();
                break;
        }
    }
    delete parsMsg;
}

int ConnectionClient::menu() {
    int option;
    if (logged) {
        cout << "Welcome to Chat room" << endl;
        cout << "\t1. Send message." << endl;
        cout << "\t2. Show contacts." << endl;
        cout << "\t3. Add user to contacts." << endl;
        cout << "\t4. Delete user from contacts." << endl;
        cout << "\t5. Check friend requests." << endl;
        cout << "\t6. Delete account." << endl;
        cout << "\t10. CLOSE" << endl << endl << endl;
        cout << "SPRAVY:" << endl;
        cout << endl << endl;

        for (int i = 0; i < messages->size(); i++) {
            cout << (*messages)[i]->at(2) << ": " << (*messages)[i]->at(1) << endl;
        }
        do {
            cin >> option;
            cin.clear();
            cin.ignore(1000, '\n');
            if (option != 0) {
                break;
            }
        } while (true);
        option += 2;
    } else {
        cout << "Welcome to Chat room" << endl;
        cout << "\t1. to Sign Up." << endl;
        cout << "\t2. to Sign In." << endl;
        cout << "\t10. CLOSE" << endl;
        do {
            cin >> option;
            cin.clear();
            cin.ignore(1000, '\n');
            if (option != 0) {
                break;
            }
        } while (true);
    }
    sendRequest(option);
    return 0;
}

bool ConnectionClient::sendRequest(int option_switch) {
    string toUser;
    string msg = "";
    string username = "errorName";
    string password = "errorPasswd";
    string sendMessage;
    string option = to_string(option_switch);
    bool response;
    switch (option_switch) {
        case 1:
            cout << "\nEnter a new username:\n";

            cin >> username;
            cin.clear();
            cin.ignore(10000, '\n');

            cout << "\nPlease enter a new password:\n";

            cin >> password;
            cin.clear();
            cin.ignore(10000, '\n');
            msg = option + ";" + username + ";" + password;
            if (!responseFromServer(msg)) {
                cout << "\nWrong login name or password\n" << endl;
            }
            break;
        case 2:
            cout << "\nEnter a username:\n";

            cin >> username;
            cin.clear();
            cin.ignore(10000, '\n');

            cout << "\nPlease enter a password:\n";

            cin >> password;
            cin.clear();
            cin.ignore(10000, '\n');
            msg = option + ";" + username + ";" + password;
            if (!responseFromServer(msg)) {
                cout << "\nWrong login name or password\n" << endl;
            } else {
                this->username = username;
                cout << this->username << endl;
                this->logged = true;
            }
            break;
        case 3:
            cout << "Enter username of message receiver: " << endl;
            cin >> toUser;
            if (toUser != this->username) {
                cout << "\nPlease enter a your message:\n";
                cin >> sendMessage;
                // TODO ignore string after whitespace
                msg = option + ";" + toUser + ";" + sendMessage + ";" + this->username;
            } else {
                cout << "\nYou can`t send message to yourself\n" << endl;
                // TODO remove
                cout << "\nPlease enter a your message:\n"; //
                cin >> sendMessage; //
                msg = option + ";" + toUser + ";" + sendMessage + ";" + this->username; //
                //break;
            }
            if (!responseFromServer(msg)) {
                cout << "\nThe user is not in contact\n" << endl;
            }
            break;
        case 4:
            cout << "Enter username: " << endl;
            cin >> username;
            msg = option;
            // show contact
            break;
        case 5:
            // add contact
            break;
        case 6:
            // delete contact
            break;
        case 10:
            end = true;
            break;
        default:
            cout << "\n404\n";
            break;
    }
}

bool ConnectionClient::responseFromServer(string msg) {
    sendToServer(msg);

    unique_lock<mutex> lk(mtx);

    cv.wait(lk);

    lk.unlock();
    // TODO remove
    cout << string(response) << endl;
    if (string(response).compare("T") == 0) {
        return true;
    }
    return false;
}

void ConnectionClient::sendToServer(string message) {
    // TODO remove
    cout << message << endl;
    char* buffer = &message[0u];
    int any = write(sockfd, buffer, 255);
}

ConnectionClient::ConnectionClient(const ConnectionClient& orig) {
}

