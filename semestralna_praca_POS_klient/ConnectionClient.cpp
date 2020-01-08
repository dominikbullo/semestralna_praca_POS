#include "ConnectionClient.h"

#define PORT 20045

// For non-authentificated users
#define REG 1
#define LOG 2
#define EXIT 10

// For authentificated users
#define SND_MSSG 1
#define SHOW_C 2
#define ADD_C 3
#define DELETE_C 4
#define CHECK_FR_REQ 5
#define DELETE_ACC 6
#define OFFLINE 10

// For authentificated users
#define INFO_RESPONSE 0
#define MSG_RESPONSE 1
#define C_RESPONSE 2

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

    serv_addr.sin_port = htons(PORT);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof (serv_addr)) < 0) {
        perror("500 ");
    }

    thread t(&ConnectionClient::readResponse, this);

    isLoggedPerson = false;
    while (true) {
        if (this->stop) {
            break;
        }
        menu();
    }
    t.join();
}

void ConnectionClient::readResponse() {
    char buffer[256];
    vector<string>* parsMsg = new vector<string>();
    int n;
    while (!this->stop) {
        parsMsg->clear();
        n = read(this->sockfd, buffer, 255);
        conditionVariable.notify_all();
        this->messageHandler->readMsg(parsMsg, string(buffer));
        switch (stoi(parsMsg->at(0))) {
            case INFO_RESPONSE:
            {
                this->response = parsMsg->at(1);
                if (parsMsg->size() > 2) {
                    cout << parsMsg->at(2) << endl;
                }
                break;
            }
            case MSG_RESPONSE:
            {
                vector<string>* vec = new vector<string>(*parsMsg);
                this->messages->push_back(vec);
                break;
            }
            case C_RESPONSE:
            {
                if (parsMsg->at(1) == "F") {
                } else {
                    cout << endl << "Contact list:" << endl;
                    for (int i = 2; i < parsMsg->size(); i++) {
                        if (parsMsg->at(i) != "") {
                            cout << i - 1 << ". " << parsMsg->at(i) << endl;
                        }
                    }
                }
                break;
            }
            case 3:
                this->requests->push_back(parsMsg->at(2));
                break;
            case 10:
                this->stop = true;
                break;
        }
    }
    delete parsMsg;
}

int ConnectionClient::menu() {
    int option;
    if (isLoggedPerson) {
        cout << "Chat room" << endl;
        cout << "\t1. Send message." << endl;
        cout << "\t2. Show contacts." << endl;
        cout << "\t3. Add user to contacts." << endl;
        cout << "\t4. Delete user from contacts." << endl;
        cout << "\t5. Accept - friend requests." << endl;
        cout << "\t6. Delete account." << endl;
        cout << "\t10. SIGN OUT" << endl << endl << endl;
        if (this->messages->size() > 0) {
            cout << "MESSAGES:" << endl;
        }

        try {
            for (int i = 0; i < this->messages->size(); i++) {
                cout << (*this->messages)[i]->at(2) << ": " << (*this->messages)[i]->at(3) << endl;
            }
            cout << endl;
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
        cout << "Chat room" << endl;
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
    string message = "";
    string toUser = "errorName";
    string password = "errorName";
    string option = to_string(option_switch);
    int confirmDeleteAccount = 0;

    string isLogged = to_string(this->isLoggedPerson);

    if (this->isLoggedPerson) {
        switch (option_switch) {
            case SND_MSSG:
                toUser = userInput("Please enter a username:");
                if (toUser != this->username) {
                    message = isLogged + ";" + option + ";" + this->username + ";" + toUser + ";" + userInput("Please enter a your message:");
                } else {
                    cout << "You can`t send message to yourself" << endl;
                }
                responseFromServer(message);
                break;
            case SHOW_C:
                message = isLogged + ";" + option + ";" + this->username;
                responseFromServer(message);
                break;
            case ADD_C:
                message = isLogged + ";" + option + ";" + userInput("Enter username - add contact:");
                responseFromServer(message);
                break;
            case DELETE_C:
                message = isLogged + ";" + option + ";" + userInput("Enter username:");
                responseFromServer(message);
                break;
            case CHECK_FR_REQ:
                message = isLogged + ";" + option + ";" + userInput("Enter username - add to friend");
                if (responseFromServer(message)) {
                    for (int i = 0; i < this->requests->size(); i++) {
                        if (this->requests->at(i).compare(toUser) == 0) {
                            this->requests->erase(this->requests->begin() + i);
                        }
                    }
                }
                break;
            case DELETE_ACC:
                cout << "Are you sure? '1' - yes, '2' - no" << endl;
                cin >> confirmDeleteAccount;
                if (confirmDeleteAccount == 1) {
                    message = isLogged + ";" + option + ";" + this->username;
                    if (responseFromServer(message)) {
                        this->isLoggedPerson = false;
                    }
                }
                break;
            case OFFLINE:
                message = isLogged + ";" + option + ";" + this->username;
                this->isLoggedPerson = false;
                responseFromServer(message);
                break;
            default:
                cout << "404" << endl;
                break;
        }
    } else {
        switch (option_switch) {
            case REG:
                toUser = userInput("Please, enter a new username:");
                password = userInput("Please, enter a new password:");
                message = isLogged + ";" + option + ";" + toUser + ";" + password;
                cout << message << endl;
                responseFromServer(message);
                break;
            case LOG:
                toUser = userInput("Please, enter a username:");
                password = userInput("Please, enter a password:");
                message = isLogged + ";" + option + ";" + toUser + ";" + password;
                if (responseFromServer(message)) {
                    this->username = toUser;
                    this->isLoggedPerson = true;
                }
                break;
            case EXIT:
                message = isLogged + ";" + option + ";" + this->username;
                this->stop = true;
                responseFromServer(message);
                break;
            default:
                cout << "404" << endl;
                break;
        }
    }
}

string ConnectionClient::userInput(string description) {
    cout << description << endl;
    string value;
    cin.clear();
    getline(cin, value);
    return value;
}

bool ConnectionClient::responseFromServer(string message) {
    sendToServer(message);
    cout << this->response << endl;
    if (this->response == "T") {
        return true;
    }
    return false;
}

void ConnectionClient::sendToServer(string message) {
    // cout << message << endl;
    const char* buffer = &message[0u];
    int any = write(sockfd, buffer, 255);

    unique_lock<mutex> lk(this->mutexBase);

    this->conditionVariable.wait(lk);

    lk.unlock();
}

ConnectionClient::~ConnectionClient() {
    delete this->requests;
    for (auto a : * this->messages) {
        delete a;
    }
    delete this->messages;
}

