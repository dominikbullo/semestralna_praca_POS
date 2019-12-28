#include <cstdlib>
#include <iostream>

#include "ConnectionClient.h"

using namespace std;


int main(int argc, char** argv) {
    cout << "Hello from CLIENT";
    ConnectionClient* connectionClient = new ConnectionClient();
    delete connectionClient;
    return EXIT_SUCCESS;
}

