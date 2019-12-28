
#include "ClientConnection.h"
#include "MessageReader.h"
using namespace std;

ClientConnection::ClientConnection() {

    struct sockaddr_in serv_addr;
    struct hostent* server;

    //printf("idem");
    end = false;
    messReader = new MessageReader();
    messages = new vector<vector<string>* >();
    //funkcia knižnice sys/socket.h na získanie "hostname"
    //ako prvý parameter zadávame "localhost"

    server = gethostbyname("localhost");

    bzero((char*) &serv_addr, sizeof (serv_addr));

    serv_addr.sin_family = AF_INET;
    bcopy(
            (char*) server->h_addr,
            (char*) &serv_addr.sin_addr.s_addr,
            server->h_length
            );
    //ako druhý parameter zadávame číslo portu

    serv_addr.sin_port = htons(20055);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    //pripájame sa na sieťovú adresu

    if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof (serv_addr)) < 0) {
        perror("CHYBA!!!");
    }

    //v nekonečnom cykle zasiela správy serveru

    //        thread t(&ServerConnection::controlUser,this,newsockfd);
    //      threads->push_back(move(t));
    thread t(&ClientConnection::reader, this);

    logged = false;
    while (true) {
        //sendMessage();
        menu();
    }
    t.join();
    //sendMessage();
    delete server;





}

void ClientConnection::reader() {
    char buffer2[256];
    vector<string>* parsMsg;
    int n;
    while (!end) {
        parsMsg = new vector<string>();
        n = read(sockfd, buffer2, 255);
        cout << string(buffer2);
        messReader->readMsg(parsMsg, string(buffer2));
        cout << parsMsg->at(0) << parsMsg->at(1);
        switch (stoi(parsMsg->at(0))) {
            case 0:
            {
                response = parsMsg->at(1);
                cv.notify_all();
                break;
            }
            case 1:
                messages->push_back(parsMsg);
                //printf("%s : %s\n",parsMsg->at(2),parsMsg->at(1));
                break;


        }

    }

}

int ClientConnection::menu() {
    int option;
    string toUser;
    if (logged) {

        cout << "Welcome" << endl;
        cout << "\tPress '1' to send message." << endl;
        cout << "\tPress '2' to show contacts." << endl;
        cout << "\tPress '3' to add user to contacts." << endl;
        cout << "\tPress '4' to delete user from contacts." << endl;
        cout << "\tPress '0' for exit" << endl << endl << endl;
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
                cout << "Enter userename of message reciever";
                cin >> toUser;
                sendMessage(toUser);
                // cout << "Registration successful"; 
                // printf("\033[H\033[J");
                break;
            }
            case 2:

                break;
            case 3:
            {



                break;
            }
            case 4:
                read(sockfd, buffer2, 255);
                printf("%s\n", buffer2);

                break;
            case 0:
                exit(0);
                break;
        }
    } else {
        cout << "Welcome" << endl;
        cout << "\tPress '1' to Register." << endl;
        cout << "\tPress '2' to Login." << endl;
        cout << "\tPress '0' for exit" << endl;
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
                if (userRegister()) {
                    cout << "Registration successful";
                    printf("\033[H\033[J");
                } else {
                    cout << "Username aleardy exists";
                }
                break;
            case 2:
                if (userLogin()) {
                    logged = true;
                    cout << "Log-in successful";
                    printf("\033[H\033[J");
                } else {
                    cout << "Wrong details";
                }
                break;
            case 10:
                exit(0);
                break;
        }


    }

    return 0;
}

bool ClientConnection::userLogin() {
    int n;
    string username;
    char* buffer;
    char buffer2[256];
    string password;

    cout << "Welcome!\n-------------------------\n\nPlease login.\nEnter a new username:\n";

    cin >> username;
    cin.clear();
    cin.ignore(10000, '\n');


    /*if(!messReader->isSpace(username)){
         break;
     }*/

    //ToDO regular expression
    cout << "\nPlease enter a new password:\n";


    cin >> password;
    cin.clear();
    cin.ignore(10000, '\n');

    /*
    if(!messReader->isSpace(password)){
            break;
        }
     */

    string msg = "1;" + username + ";" + password;
    buffer = &msg[0u];
    n = write(sockfd, buffer, 255); //pošle serveru správu uloženú v bufferi  

    unique_lock<mutex> lk(mtx);
    cv.wait(lk);
    lk.unlock();
    if (string(response).compare("T") == 0) {
        this->username = username;
        return true;
    }
    return false;

}
//ToDo thread pre read a thread pre uzivatela interface a tak

bool ClientConnection::userRegister() {
    const char* buffer;
    char buffer2[256];
    // char* bufferOut
    int n;
    string username;
    string password;
    cout << "Welcome!\n-------------------------\n\nPlease register.\nEnter a new username:\n";


    cin >> username;
    cin.clear();
    cin.ignore(10000, '\n');

    /*if(!messReader->isSpace(username)){
        break;
    }*/


    cout << "\nPlease enter a new password:\n";


    cin >> password;
    cin.clear();
    cin.ignore(10000, '\n');

    /*if(!(messReader->isSpace(password))){
            break;
        }*/

    string msg = "0;" + username + ";" + password;
    buffer = msg.c_str();
    n = write(sockfd, buffer, 255); //pošle serveru správu uloženú v bufferi

    unique_lock<mutex> lk(mtx);

    cv.wait(lk);

    lk.unlock();
    if (string(response).compare("T") == 0) {
        return true;
    }
    return false;
    /*
    buffer = &msg[0u];
     printf("%s",buffer);
    n = write(sockfd, buffer, 255); //pošle serveru správu uloženú v bufferi  
    bzero(buffer,256);
    n = read(sockfd, buffer, 255); //číta správu od servera
    printf("%d",n);
    printf("Prijatá správa: %s\n", buffer);
     */


}

bool ClientConnection::sendMessage(string username) {
    int n;
    string message;
    char* buffer;
    //ToDO regular expression
    cout << "\nPlease enter a your message:\n";
    cin >> message;
    string msg = "3;" + username + ";" + message + ";" + this->username;
    buffer = &msg[0u];
    n = write(sockfd, buffer, 255); //pošle serveru správu uloženú v bufferi  

    return true;
    /*
   char buffer[256];
   int n;
   int x=0;
 
       while(x == 0){
               printf("Zadaj správu: ");
               bzero(buffer,256);
               fgets(buffer, 255, stdin);

               n = write(sockfd, buffer, strlen(buffer)); //pošle serveru správu uloženú v bufferi

               bzero(buffer,256); //čistí buffer
               n = read(sockfd, buffer, 255); //číta správu od servera

               printf("%s\n",buffer);
       }

       close(sockfd);
       return true;
     */
}

bool ClientConnection::addContact(string pusername) {
    char* buffer;
    int n;
    string msg = "4;" + username + ";" + pusername;
    buffer = &msg[0u];
    n = write(sockfd, buffer, 255); //pošle serveru správu uloženú v bufferi

}

ClientConnection::~ClientConnection() {
}

