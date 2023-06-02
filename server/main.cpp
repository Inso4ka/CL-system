#include <boost/asio.hpp>
#include <iostream>
#include <sqlite3.h>

using boost::asio::ip::tcp;

void handle_connection(tcp::socket& socket) {
    std::array<char, 256>     buffer;
    boost::system::error_code error;

    // Keep reading from the socket until the client closes the connection
    while (true) {
        size_t bytes_transferred = socket.read_some(boost::asio::buffer(buffer), error);

        if (error) {
            if (error == boost::asio::error::eof) {
                break;
            } else {
                std::cerr << "Error receiving request: " << error.message() << std::endl;
                return;
            }
        }

        std::string message(buffer.data(), bytes_transferred);
        std::cout << "Received message: " << message << std::endl;

        std::string response;
        if (message == "security") {
            response = "Hi there!";
        } else if (message == "What is your name?") {
            response = "My name is Server.";
        } else if (message == "What time is it?") {
            time_t now = std::time(nullptr);
            response   = std::ctime(&now);
        } else {
            response = "Unknown command.";
        }

        boost::asio::write(socket, boost::asio::buffer(response));
    }
}

int main() {
    boost::asio::io_service io_service;

    tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 1234));

    while (true) {
        tcp::socket socket(io_service);
        acceptor.accept(socket);

        handle_connection(socket);
    }

    return 0;
}
