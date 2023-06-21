#include "columnDatabase.h"
#include "lightDatabase.h"
#include "securityDatabase.h"
#include "temperatureDatabase.h"
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

void handle_connection(tcp::socket& socket) {
    std::array<char, 256>     buffer;
    boost::system::error_code error;

    SecurityDatabase    s_db("security.db");
    TemperatureDatabase t_db("temperature.db");
    ColumnDatabase      c_db("column.db");
    LightDatabase       l_db("light.db");

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
            if (s_db.is_open()) {
                if (s_db.update_security_condition(!s_db.is_security_enabled())) {
                    if (s_db.is_security_enabled()) {
                        l_db.update_condition("Bathroom", 0);
                        l_db.update_condition("Kitchen", 0);
                        l_db.update_condition("Living room", 0);
                        t_db.update_security_condition(20);
                        c_db.update_column_condition(0);
                        response = "All light was turned off. \nThe temperature was set to 20. \nColumn was turned off. \nSecurity condition updated successfully: " +
                                   std::string(s_db.is_security_enabled() ? "on" : "off") + ".";
                    } else {
                        response = "Security condition updated successfully: " + std::string(s_db.is_security_enabled() ? "on" : "off") + ".";
                    }
                } else {
                    response = "Failed to update security condition\n";
                }
            } else {
                std::cout << "Failed to open database" << std::endl;
            }
        } else if (message.substr(0, 11) == "temperature") {
            if (t_db.is_open()) {
                t_db.update_security_condition(std::stoi(message.substr(11, 13)));
                response = std::string("Temperature was succesfully changed to ") + std::to_string(t_db.get_temperature_from_database()) + ".";
            }
        } else if (message == "living_room") {
            if (l_db.is_open()) {
                if (l_db.update_condition("Living room", !l_db.is_light_enabled("Living room"))) {
                    response = "Light condition (living room) updated successfully: " + std::string(l_db.is_light_enabled("Living room") ? "on" : "off") + ".";

                } else {
                    response = "Failed to update light condition\n";
                }
            } else {
                std::cout << "Failed to open database" << std::endl;
            }
        } else if (message == "kitchen") {
            if (l_db.is_open()) {
                if (l_db.update_condition("Kitchen", !l_db.is_light_enabled("Kitchen"))) {
                    response = "Light condition (kitchen) updated successfully: " + std::string(l_db.is_light_enabled("Kitchen") ? "on" : "off") + ".";

                } else {
                    response = "Failed to update light condition\n";
                }
            } else {
                std::cout << "Failed to open database" << std::endl;
            }
        } else if (message == "column") {
            if (c_db.is_open()) {
                if (c_db.update_column_condition(!c_db.is_column_enabled())) {
                    response = "Column condition updated successfully: " + std::string(c_db.is_column_enabled() ? "on" : "off") + ".";
                } else {
                    response = "Failed to update column condition\n";
                }
            }
        } else if (message == "bathroom") {
            if (l_db.is_open()) {
                if (l_db.update_condition("Bathroom", !l_db.is_light_enabled("Bathroom"))) {
                    response = "Light condition (bathroom) updated successfully: " + std::string(l_db.is_light_enabled("Bathroom") ? "on" : "off") + ".";

                } else {
                    response = "Failed to update light condition\n";
                }
            } else {
                std::cout << "Failed to open database" << std::endl;
            }
        } else if (message == "turnoff") {
            l_db.update_condition("Bathroom", 0);
            l_db.update_condition("Kitchen", 0);
            l_db.update_condition("Living room", 0);
            response = "All light was turned off.";
        } else if (message == "show") {
            response = "\nSecurity system: " + std::string(s_db.is_security_enabled() ? "on" : "off") + ".\n" +
                       "Temperature: " + std::to_string(t_db.get_temperature_from_database()) + ".\n" +
                       "Column: " + std::string(c_db.is_column_enabled() ? "on" : "off") + ".\n" +
                       "Living room: " + std::string(l_db.is_light_enabled("Living room") ? "on" : "off") + ".\n" +
                       "Kitchen: " + std::string(l_db.is_light_enabled("Kitchen") ? "on" : "off") + ".\n" +
                       "Bathroom: " + std::string(l_db.is_light_enabled("Bathroom") ? "on" : "off") + ".\n";
        } else {
            response = "Unknown command.";
        }

        boost::asio::write(socket, boost::asio::buffer(response));
    }
}

int main() {
    boost::asio::io_service io_service;
    tcp::acceptor           acceptor(io_service, tcp::endpoint(tcp::v4(), 1234));

    while (true) {
        tcp::socket socket(io_service);
        acceptor.accept(socket);

        handle_connection(socket);
    }

    return 0;
}