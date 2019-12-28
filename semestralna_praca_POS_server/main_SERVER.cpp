#include <cstdlib>
#include <iostream>
#include "ServerConnection.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    cout << "Hello from SERVER";
    ServerConnection* connection = new ServerConnection();
    return EXIT_SUCCESS;
}

