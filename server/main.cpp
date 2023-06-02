#include <boost/asio.hpp>
#include <iostream>
#include <sqlite3.h>
#include <string>

using boost::asio::ip::tcp;

class SecurityDatabase
{
  public:
    SecurityDatabase(const std::string& db_name) : m_name(db_name) {
        int rc = sqlite3_open(db_name.c_str(), &m_db);
        if (rc != SQLITE_OK) {
            std::cerr << "Error opening database: " << sqlite3_errmsg(m_db) << std::endl;
            sqlite3_close(m_db);
            m_db = nullptr;
        } else {
            // создаем таблицу 'security', если она не существует
            std::string create_query("CREATE TABLE IF NOT EXISTS security (name TEXT, condition INTEGER);");
            rc = sqlite3_exec(m_db, create_query.c_str(), nullptr, nullptr, nullptr);
            if (rc != SQLITE_OK) {
                std::cerr << "Error creating table: " << sqlite3_errmsg(m_db) << std::endl;
                sqlite3_close(m_db);
                m_db = nullptr;
            }
        }
    }

    ~SecurityDatabase() {
        if (m_db) {
            sqlite3_close(m_db);
            m_db = nullptr;
        }
    }

    static int count;

    bool update_security_condition(bool value) {
        const char* condition    = value ? "1" : "0"; // заменили "true" и "false" на 1 и 0 соответственно
        char*       update_query = sqlite3_mprintf("UPDATE security SET condition = '%q' WHERE name = 'security'", condition);
        int         rc           = sqlite3_exec(m_db, update_query, nullptr, nullptr, nullptr);
        sqlite3_free(update_query);
        return rc == SQLITE_OK;
    }

    bool is_open() const {
        return m_db != nullptr;
    }

  private:
    std::string m_name;
    sqlite3*    m_db = nullptr;
};


void handle_connection(tcp::socket& socket) {
    std::array<char, 256>     buffer;
    boost::system::error_code error;
    bool condition = false;

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
            SecurityDatabase db("security.db");
            if (db.is_open()) {
                

                if (db.update_security_condition(condition)) {
                    if (condition)
                        response = "Security condition updated successfully: [TRUE]";
                    else
                        response = "Security condition updated successfully: [FALSE]";
                    condition = !condition;
                } else {
                    response = "Failed to update security condition\n";
                }
            } else {
                std::cout << "Failed to open database" << std::endl;
            }
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
    tcp::acceptor           acceptor(io_service, tcp::endpoint(tcp::v4(), 1234));

    while (true) {
        tcp::socket socket(io_service);
        acceptor.accept(socket);

        handle_connection(socket);
    }

    return 0;
}