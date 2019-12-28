#include "ConnectionServer.h"

#define REG 0
#define LOG 1
#define SND 2



ConnectionServer::ConnectionServer() {
    int sockfd, newsockfd, newsockf2;

    socklen_t cli_len;
    struct sockaddr_in serv_addr, cli_addr;

    int i = 0;
    char buffer[256];
    int n;

    //vyčistenie (vynulovanie) a inicializácia sieťovej adresy

    bzero((char*) &serv_addr, sizeof (serv_addr));

    //nastavenie pre adresovanie v doméne INET
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(20055);

    //tvorba samotného socketu "sockfd"
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    //priradenie adresy socketu (aj s overením)

    if (bind(sockfd, (struct sockaddr*) &serv_addr, sizeof (serv_addr)) < 0) {
        perror("CHYBA! - nepodarilo sa priradiť adresu");
    }

    //umožníme pripojenie klienta na socket "sockfd",
    //druhý parameter je dĺžka fronty na pripojenie k "sockfd"

    listen(sockfd, 10);
    cli_len = sizeof (cli_addr);

    vector<thread> *threads = new vector<thread>();

    while (true) {
        cout << "Waiting for socket";
        newsockfd = accept(sockfd, (struct sockaddr*) &cli_addr, &cli_len);

        thread t(&ConnectionServer::controlUser, this, newsockfd);
        threads->push_back(move(t));
    }

    for (thread &a : *threads) {
        a.join();
    }
}

void ConnectionServer::controlUser(int socket) {
    ConnectedUser* user;
    int pozicia = -1;
    char buffer[256];
    int n;
    vector<string>* parsedMsg = new vector<string>();
    while (true) {
        parsedMsg->clear();
        bzero(buffer, 256); //čistí buffer
        n = read(socket, buffer, 255); //načítava do buffera z newsockfd
        if (n == 0 || n == -1) {
            break;
        }
        std::cout << std::endl << n << std::endl;
        std::cout << std::endl << string(buffer);
        
        // parser spravy 
        messageReader->readMsg(parsedMsg, string(buffer)); //zavoal parsovac spravy
        
        std::cout << std::endl << parsedMsg->at(1) << parsedMsg->at(2);
        const char* msg;
        cout << "Message is" << msg;
        
        //        switch (stoi(parsedMsg->at(0))) {
        //            case 0: // sprava od klienta , registracia
        //                if (registerUser(*parsedMsg)) {
        //                    msg = "0;T";
        //
        //                } else {
        //                    msg = "0;F";
        //                }
        //                n = write(socket, msg, strlen(msg) + 1); //zasiela správu "msg" klientovi
        //                break;
        //            case 1: // sprava od klienta , prihlasenie
        //                int i;
        //                if ((i = loginUser(*parsedMsg, socket)) != -1) {
        //                    pozicia = i;
        //                    user = allUsers->at(i);
        //                    msg = "0;T";
        //                } else {
        //                    msg = "0;F";
        //                }
        //                n = write(socket, msg, strlen(msg) + 1); //zasiela správu "msg" klientovi
        //                break;
        //
        //            case 2: // sprava od klienta , odhlasenie
        //
        //                break;
        //            case 3: // sprava od klienta , poslanie spravy
        //                sendMsg(*parsedMsg);
        //                break;
        //            case 4:
        //            {
        //                //vyzor spravy 4;username
        //                vector<string>*contacts = user->getContacts();
        //                bool flag = false;
        //                //                for(int i = 0 ; i< contacts; i++){
        //                //                    if(contacts->at(i).compare(parsedMsg->at(1)) == 0){
        //                //                        flag = true;
        //                //                    }
        //                //                
        //                //                }
        //                //                if(!flag){
        //                //                    msg =  "0;T";
        //                //                    user->getContacts()->pop_back(parsedMsg->at(1));
        //                //                }else{
        //                //                    msg =  "0;F";
        //                //                }
        //                n = write(socket, msg, strlen(msg) + 1); //zasiela správu "msg" klientovi
        //                break;
        //            }
        //
        //
        //            case 5:
        //                //dorobit vypis kontaktov cey getContacts
        //
        //                break;
        //
        //            case 6:
        //
        //
        //                break;
        //        }

    }

    close(socket);
}

ConnectionServer::~ConnectionServer() {
}




