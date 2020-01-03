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
        if (this->end) {
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
    while (!this->end) {
        parsMsg->clear();
        n = read(sockfd, buffer, 255);
        messReader->readMsg(parsMsg, string(buffer));
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
                this->end = true;
                cv.notify_all();
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
        if (this->messages->size() > 0) {
            cout << "SPRAVY:" << endl;
        }
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
    int confirmDeleteAccount = 0;

    string isLogged = to_string(this->logged);

    if (this->logged) {
        switch (option_switch) {
            case 1:
                cout << "Enter username of message receiver: " << endl;
                cin >> toUser;
                if (toUser != this->username) {
                    cout << endl << "Please enter a your message:" << endl;
                    cin.ignore(10000, '\n');
                    cin.clear();
                    getline(cin, sendMessage);
                    msg = isLogged + ";" + option + ";" + this->username + ";" + sendMessage + ";" + toUser;
                } else {
                    cout << "You can`t send message to yourself" << endl;
                    // TODO remove
                    cout << "Please enter a your message:" << endl; //
                    cin.ignore(10000, '\n');
                    cin.clear();
                    getline(cin, sendMessage);
                    msg = isLogged + ";" + option + ";" + this->username + ";" + sendMessage + ";" + toUser; //
                    cout << msg << endl;
                    //break;
                }
                if (!responseFromServer(msg)) {
                    cout << endl << "The user is not in contact" << endl;
                }
                break;
            case 2:
                msg = isLogged + ";" + option + ";" + this->username;
                responseFromServer(msg);
                // show contact
                break;
            case 3:
                cout << "Enter username - add contact:" << endl;
                cin.ignore();
                getline(cin, username);
                cout << username << endl;
                msg = isLogged + ";" + option + ";" + this->username + ";" + username;
                cout << msg << endl;
                responseFromServer(msg);
                break;
            case 4:
                cout << "Enter username:" << endl;
                cin.ignore(10000, '\n');
                cin.clear();
                getline(cin, username);
                msg = isLogged + ";" + option + ";" + this->username + ";" + username;
                responseFromServer(msg);
                break;
            case 5:
                // confirm new contact
                break;
            case 6:
                cout << "Are you sure?" << endl;
                cin.ignore(10000, '\n');
                cin.clear();
                cin >> confirmDeleteAccount;
                if (confirmDeleteAccount == 1) {
                    msg = isLogged + ";" + option + ";" + this->username;
                    responseFromServer(msg);
                }
                break;
            case 10:
                msg = isLogged + ";" + option + ";" + this->username;
                this->logged = false;
                responseFromServer(msg);
                break;
            default:
                cout << "404" << endl;
                break;
        }
    } else {
        switch (option_switch) {
            case 1:
                cout << endl << "Enter a new username:" << endl;

                cin >> username;
                cin.clear();
                cin.ignore(10000, '\n');

                cout << endl << "Please enter a new password:" << endl;

                cin >> password;
                cin.clear();
                cin.ignore(10000, '\n');
                msg = isLogged + ";" + option + ";" + username + ";" + password;
                if (!responseFromServer(msg)) {
                    cout << endl << "Wrong login name or password" << endl;
                }
                break;
            case 2:
                cout << endl << "Enter a username:" << endl;
                cin >> username;
                cin.clear();
                cin.ignore(10000, '\n');

                cout << endl << "Please enter a password:" << endl;

                cin >> password;
                cin.clear();
                cin.ignore(10000, '\n');
                msg = isLogged + ";" + option + ";" + username + ";" + password;
                if (!responseFromServer(msg)) {
                    cout << endl << "Wrong login name or password" << endl;
                } else {
                    this->username = username;
                    cout << this->username << endl;
                    this->logged = true;
                }
                break;
            case 10:
                msg = isLogged + ";" + option + ";" + this->username;
                this->end = true;
                responseFromServer(msg);
                break;
            default:
                cout << "404" << endl;
                break;
        }
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

