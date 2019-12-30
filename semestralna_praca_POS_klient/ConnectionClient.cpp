#include <algorithm>

#include "ConnectionClient.h"

using namespace std;

ConnectionClient::ConnectionClient() {
    struct sockaddr_in serv_addr;
    struct hostent* server;

    server = gethostbyname("localhost");

    bzero((char*) &serv_addr, sizeof (serv_addr));

    serv_addr.sin_family = AF_INET;
    bcopy(
            (char*) server->h_addr,
            (char*) &serv_addr.sin_addr.s_addr,
            server->h_length
            );
    //ako druhý parameter zadávame číslo portu

    serv_addr.sin_port = htons(20045);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    //pripájame sa na sieťovú adresu

    if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof (serv_addr)) < 0) {
        perror("ERROR!!!");
    }

    while (true) {
        //sendMessage();
        if (end) {
            break;
        }
        menu();
    }
}

int ConnectionClient::menu() {
    int option;
    if (logged) {
        int n;

        const char* buffer;
        cout << "Welcome to Chat room" << endl;
        cout << "\t1. Send message." << endl;
        cout << "\t2. Show contacts." << endl;
        cout << "\t3. Add user to contacts." << endl;
        cout << "\t4. Delete user from contacts." << endl;
        cout << "\t5. Check friend requests." << endl;
        cout << "\t6. Delete account." << endl;
        cout << "\t10. CLOSE" << endl << endl << endl;
        cout << "SPRAVY:" << endl;
        for (int i = 0; i < messages->size(); i++) {
            cout << (*messages)[i]->at(2) << ": " << (*messages)[i]->at(1) << endl;
        }
        cout << endl << endl;

        do {
            cin >> option;
            cin.clear();
            cin.ignore(1000, '\n');
            if (option != 0) {
                break;
            }
        } while (true);
        char buffer2[256];
        switch (option) {
            case 1:
            {
                //if (toUser != this->username) {
                if (sendRequest(3)) {

                } else {
                    cout << "You can only chat with your contacts" << endl;
                }

                //}
                break;
            }
            case 2:
            {
                sendRequest(4);
                break;
            }
            case 3:
            {
                string msg;
                string username;
                cout << "Enter username: " << endl;
                cin >> username;
                msg = "4;" + username;
                buffer = &msg[0u];
                n = write(sockfd, buffer, 255);



                break;
            }
            case 4:
            {
                string msg;
                string username;
                getContacts();
                cout << "Enter username to delete user from contacts:" << endl;
                cin >> username;
                msg = "2;" + username;
                buffer = &msg[0u];
                n = write(sockfd, buffer, 255);
                cin.clear();
                cin.ignore(1000, '\n');
                break;

            }
            case 5:
            {
                int answer;
                string msg;
                cout << "0 or other number - decline " << endl << "1 - accept" << endl;
                int i;
                for (i = 0; i < requests->size(); i++) {
                    cout << requests->at(i) << endl;
                    cin >> answer;
                    if (answer == 1) {
                        msg = "6;" + requests->at(i);
                        buffer = &msg[0u];
                        n = write(sockfd, buffer, 255); //pošle serveru správu uloženú v bufferi
                    }
                }
                if (i == 0)
                    cout << "no requests" << endl;
                requests->clear();
                break;
            }
            case 6:
            {
                string msg;
                msg = "8;";
                buffer = &msg[0u];
                n = write(sockfd, buffer, 255);
                logged = false;
                break;
            }
            case 10:
            {
                string msg;
                msg = "7;";
                buffer = &msg[0u];
                n = write(sockfd, buffer, 255);
                logged = false;
                break;
            }
        }
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
        switch (option) {
            case 1:
                if (sendRequest(option)) {
                    cout << "\nRegistration successful";
                    printf("\033[H\033[J");
                } else {
                    cout << "Username already exists";
                }
                break;
            case 2:
                if (sendRequest(option)) {
                    logged = true;
                    cout << "Log-in successful";
                    printf("\033[H\033[J");
                } else {
                    cout << "Wrong details";
                }
                break;
            case 10:
            {
                int n;
                const char* buffer;
                string msg;
                msg = "10;";
                buffer = &msg[0u];
                n = write(sockfd, buffer, 255);
                end = true;
                break;
            }
        }
    }

    return 0;
}

void ConnectionClient::getContacts() {
    const char* buffer;
    int n;
    buffer = "5;";
    n = write(sockfd, buffer, 255);
    unique_lock<mutex> lk(mtx);

    cv.wait(lk);

    lk.unlock();

}

bool ConnectionClient::sendRequest(int option) {
    string toUser;
    string msg = "";
    string username = "errorName";
    string password = "errorPasswd";
    string message;
    switch (option) {
        case 1:
            cout << "\nEnter a new username:\n";

            cin >> username;
            cin.clear();
            cin.ignore(10000, '\n');

            cout << "\nPlease enter a new password:\n";

            cin >> password;
            cin.clear();
            cin.ignore(10000, '\n');
            msg = to_string(option) + ";" + username + ";" + password;
            cout << option << endl;
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
            break;
        case 3:
            cout << "Enter username of message reciever: " << endl;
            cin >> toUser;
            if (toUser != this->username) {
                cout << "\nPlease enter a your message:\n";
                cin >> message;
                msg = option + ";" + username + ";" + message + ";" + this->username;
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
            msg = option;
            break;
        default:
            cout << "\n404\n";
            break;
    }
    sendToServer(msg);

    unique_lock<mutex> lk(mtx);

    cv.wait(lk);

    lk.unlock();
    if (string(response).compare("T") == 0) {
        return true;
    }
    return false;

}

void ConnectionClient::sendToServer(string message) {
    cout << message << endl;
    char* buffer = &message[0u];
    int any = write(sockfd, buffer, 255);
}

ConnectionClient::ConnectionClient(const ConnectionClient& orig) {
}

