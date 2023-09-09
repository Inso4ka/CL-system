#include "../shared/hashfunc.h"
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

        std::vector<std::string> words = {"security", "column", "show", "living_room", "bathroom", "kitchen", "turnoff", "temperature"};
        std::vector<std::string> expected_hashes;

        for (const std::string& word : words) {
            SHA256 sha;
            sha.update(word);
            uint8_t* digest = sha.digest();
            expected_hashes.push_back(SHA256::toString(digest));
        }

        const size_t hash_size = expected_hashes[0].size();
        if (message.size() != hash_size) {
            if (expected_hashes[7].find(message)) {
                message.erase(0, message.find(expected_hashes[7]) + strlen(expected_hashes[7].c_str()));
                if (t_db.is_open()) {
                    t_db.update_security_condition(std::stoi(message));
                    response = std::string("Temperature was succesfully changed to ") + std::to_string(t_db.get_temperature_from_database()) + ".";
                }
            }
        } else if (memcmp(message.c_str(), expected_hashes[0].c_str(), hash_size) == 0) {
            if (s_db.is_open()) {
                if (s_db.update_security_condition(!s_db.is_security_enabled())) {
                    if (s_db.is_security_enabled()) {
                        l_db.update_condition("Bathroom", 0);
                        l_db.update_condition("Kitchen", 0);
                        l_db.update_condition("Living room", 0);
                        t_db.update_security_condition(20);
                        c_db.update_column_condition(0);
                        response = "Security condition updated successfully: " + std::string(s_db.is_security_enabled() ? "on" : "off") + ".\n\n" +
                                   "All light was turned off. \nThe temperature was set to 20. \nColumn was turned off.";
                    } else {
                        response = "Security condition updated successfully: " + std::string(s_db.is_security_enabled() ? "on" : "off") + ".";
                    }
                } else {
                    response = "Failed to update security condition\n";
                }
            } else {
                std::cout << "Failed to open database" << std::endl;
            }
        } else if (memcmp(message.c_str(), expected_hashes[3].c_str(), hash_size) == 0) {
            if (l_db.is_open()) {
                if (l_db.update_condition("Living room", !l_db.is_light_enabled("Living room"))) {
                    response = "Light condition (living room) updated successfully: " + std::string(l_db.is_light_enabled("Living room") ? "on" : "off") + ".";

                } else {
                    response = "Failed to update light condition\n";
                }
            } else {
                std::cout << "Failed to open database" << std::endl;
            }
        } else if (memcmp(message.c_str(), expected_hashes[5].c_str(), hash_size) == 0) {
            if (l_db.is_open()) {
                if (l_db.update_condition("Kitchen", !l_db.is_light_enabled("Kitchen"))) {
                    response = "Light condition (kitchen) updated successfully: " + std::string(l_db.is_light_enabled("Kitchen") ? "on" : "off") + ".";

                } else {
                    response = "Failed to update light condition\n";
                }
            } else {
                std::cout << "Failed to open database" << std::endl;
            }
        } else if (memcmp(message.c_str(), expected_hashes[1].c_str(), hash_size) == 0) {
            if (c_db.is_open()) {
                if (c_db.update_column_condition(!c_db.is_column_enabled())) {
                    response = "Column condition updated successfully: " + std::string(c_db.is_column_enabled() ? "on" : "off") + ".";
                } else {
                    response = "Failed to update column condition\n";
                }
            }
        } else if (memcmp(message.c_str(), expected_hashes[4].c_str(), hash_size) == 0) {
            if (l_db.is_open()) {
                if (l_db.update_condition("Bathroom", !l_db.is_light_enabled("Bathroom"))) {
                    response = "Light condition (bathroom) updated successfully: " + std::string(l_db.is_light_enabled("Bathroom") ? "on" : "off") + ".";

                } else {
                    response = "Failed to update light condition\n";
                }
            } else {
                std::cout << "Failed to open database" << std::endl;
            }
        } else if (memcmp(message.c_str(), expected_hashes[6].c_str(), hash_size) == 0) {
            l_db.update_condition("Bathroom", 0);
            l_db.update_condition("Kitchen", 0);
            l_db.update_condition("Living room", 0);
            response = "All light was turned off.";
        } else if (memcmp(message.c_str(), expected_hashes[2].c_str(), hash_size) == 0) {
            response = "\nSecurity system: " + std::string(s_db.is_security_enabled() ? "on" : "off") + ".\n" +
                       "Temperature: " + std::to_string(t_db.get_temperature_from_database()) + ".\n" + "Column: " + std::string(c_db.is_column_enabled() ? "on" : "off") + ".\n" +
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