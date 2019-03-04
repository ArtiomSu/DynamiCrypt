#include "ConnectionManager.h"

#include <cstring>
#include <iostream>
#include <string>

int main()
{
    // Start up the server
    ConnectionManager connectionManager(5000, 2);
    connectionManager.Start();

    // Pretend we are doing other things or just waiting for shutdown
    std::this_thread::sleep_for(std::chrono::minutes(5));

    // Stop the server
    connectionManager.Stop();

    return 0;
}