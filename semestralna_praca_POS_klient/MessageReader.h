#ifndef MESSAGEREADER_H
#define MESSAGEREADER_H
#include <string>
#include <vector>
using namespace std;
class MessageReader {
public:
    MessageReader(){};
    inline void readMsg(vector<string>* vecMsg,string msg){
        string::size_type i = 0;
        string::size_type j = msg.find(';');
        while (j != string::npos) {
            vecMsg->push_back(msg.substr(i, j-i));
            i = ++j;
            j = msg.find(';', j);
            if (j == string::npos)
           vecMsg->push_back(msg.substr(i, msg.length()));
        }
    };

    virtual ~MessageReader(){};
private:
};

#endif /* MESSAGEREADER_H */

