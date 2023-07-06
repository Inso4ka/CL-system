#include "client.h"

//Main function of client
int main() {
    boost::asio::io_service io_service;
    SmartHomeSystem         system(io_service);
    system.run();

    return 0;
}