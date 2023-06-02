#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp;

void handle_command(tcp::socket& socket, std::string command) {
    std::array<char, 256> buffer;
    boost::system::error_code error;

    boost::asio::write(socket, boost::asio::buffer(command));
    size_t bytes_transferred = socket.read_some(boost::asio::buffer(buffer), error);

    if (error) {
        std::cerr << "Error receiving response: " << error.message() << std::endl;
    } else {
        std::string response(buffer.data(), bytes_transferred);
        std::cout << "Received response: " << response << std::endl;
    }
}

int main() {
    boost::asio::io_service io_service;

    tcp::resolver               resolver(io_service);
    tcp::resolver::results_type endpoints = resolver.resolve("localhost", "1234");

    tcp::socket socket(io_service);
    boost::asio::connect(socket, endpoints);

    std::cout << "Welcome to our smart home system. With our app, you can control the temperature, adjust the lighting, and keep your home safe and secure.\n\n"
              << "1. on/off light\n"
              << "2. on/off security\n"
              << "3. control temperature\n"
              << "4. exit\n\n";

    bool isRunning = true;
    int  s{};

    std::array<char, 256>     buffer;
    boost::system::error_code error;

    size_t bytes_transferred = 0;
    std::string commands[] = {"light", "security"};
    while (isRunning) {
        std::cout << "Choose an option: ";
        std::cin >> s;

        switch (s) {
            case 1:
                std::cout << "1";
                break;
            case 2:
                handle_command(socket, commands[0]);
                break;
            default:
                std::cout << "Invalid option.\n";
                break;
        }
    }

    return 0;
}
