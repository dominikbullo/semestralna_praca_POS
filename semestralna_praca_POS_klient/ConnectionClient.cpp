#include <algorithm>

#include "ConnectionClient.h"

using namespace std;

ConnectionClient::ConnectionClient() {
    struct sockaddr_in serv_addr;
    struct hostent* server;

    this->messages = new vector<vector<string>* >();
    this->requests = new vector<string>();

    server = gethostbyname("localhost");

    bzero((char*) &serv_addr, sizeof (serv_addr));

    serv_addr.sin_family = AF_INET;
    bcopy(
            (char*) server->h_addr,
            (char*) &serv_addr.sin_addr.s_addr,
            server->h_length
            );
    //ako druhý parameter zadávame číslo portu

    serv_addr.sin_port = htons(20049);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    //pripájame sa na sieťovú adresu

    if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof (serv_addr)) < 0) {
        perror("ERROR!!!");
    }

    thread t(&ConnectionClient::readResponse, this);
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

void ConnectionClient::readResponse() {
    char buffer[256];
    vector<string>* parsMsg = new vector<string>();
    int n;
    while (!this->end) {
        parsMsg->clear();
        n = read(this->sockfd, buffer, 255);
        cout << string (buffer) << endl;
        this->messReader->readMsg(parsMsg, string(buffer));
        cout << parsMsg->at(0) << endl;
        switch (stoi(parsMsg->at(0))) {
            case 0:
            {
                cout << "case 0" << endl;
                this->response = parsMsg->at(1);
                cout << this->response << endl;
                cv.notify_all();
                
                try {
                    if (this->response == "F") {
                        cout << parsMsg->at(2) << endl;
                    }
                } catch (const std::exception& e) {
                    cerr << e.what() << endl;
                }

                try {
                    if (this->response == "T") {
                        cout << parsMsg->at(2) << endl;
                    }
                } catch (const std::exception& e) {
                    cerr << e.what() << endl;
                }

                break;
            }
            case 1:
            {
                vector<string>* vec = new vector<string>(*parsMsg);
                this->messages->push_back(vec);
                break;
            }
            case 2:
            {
                cout << endl << "Contact list:" << endl;
                if (parsMsg->at(1) == "F") {
                    cout << parsMsg->at(2) << endl;
                } else {
                    for (int i = 1; i < parsMsg->size(); i++) {
                        if (parsMsg->at(i) != "") {
                            cout << i << ". " << parsMsg->at(i) << endl;
                        }
                    }
                }
                cv.notify_all();
                break;
            }
            case 3:
                this->requests->push_back(parsMsg->at(2));
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
        cout << "\t10. SIGN OUT" << endl << endl << endl;
        if (this->messages->size() > 0) {
            cout << "MESSAGES:" << endl;
        }

        try {
            for (int i = 0; i < this->messages->size(); i++) {
                cout << (*this->messages)[i]->at(2) << ": " << (*this->messages)[i]->at(3) << endl;
            }
        } catch (const std::exception& e) {
            cerr << e.what() << endl;
        }

        if (this->requests->size() > 0) {
            cout << "FRIEND REQUESTS:" << endl;
        }

        try {
            for (int i = 0; i < this->requests->size(); i++) {
                cout << this->requests->at(i) << endl;
            }
        } catch (const std::exception& e) {
            cerr << e.what() << endl;
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

void ConnectionClient::checkNews() {

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
                responseFromServer(msg);
                break;
            case 2:
                // show contact
                msg = isLogged + ";" + option + ";" + this->username;
                responseFromServer(msg);
                break;
            case 3:
                cout << "Enter username - add contact:" << endl;
                getline(cin, username);
                cout << username << endl;
                msg = isLogged + ";" + option + ";" + username;
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
                responseFromServer(msg);
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
                if (responseFromServer(msg)) {
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

    unique_lock<mutex> lk(this->mtx);

    this->cv.wait(lk);

    lk.unlock();
}

ConnectionClient::ConnectionClient(const ConnectionClient& orig) {
}

