#include <cstdlib>
#include <iostream>

#include "ConnectionServer.h"

using namespace std;

int main(int argc, char** argv) {
    cout << "Hello from SERVER" << endl;
    ConnectionServer* connection = new ConnectionServer();
    return EXIT_SUCCESS;
}

