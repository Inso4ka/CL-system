#include <boost/asio.hpp>
#include <iostream>
#include <sqlite3.h>
#include <string>

using boost::asio::ip::tcp;

class LightDatabase
{
  public:
    LightDatabase(const std::string& db_name) : m_name{db_name} {
        int rc = sqlite3_open(db_name.c_str(), &m_db);
        if (rc != SQLITE_OK) {
            std::cerr << "Error opening database: " << sqlite3_errmsg(m_db) << std::endl;
            return;
        }

        create();
    }

    ~LightDatabase() {
        if (m_db) {
            sqlite3_close(m_db);
            m_db = nullptr;
        }
    }

    bool update_condition(const std::string& room, bool condition) {
        std::string condition_str = condition ? "1" : "0";
        std::string update_query  = "UPDATE light SET condition = " + condition_str + " WHERE room = '" + room + "'";
        int         rc            = sqlite3_exec(m_db, update_query.c_str(), nullptr, nullptr, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "Error executing query: " << sqlite3_errmsg(m_db) << std::endl;
            return false;
        }
        return true;
    }

    bool is_open() const {
        return m_db != nullptr;
    }

    bool is_light_enabled(const std::string& str) {
        std::string query("SELECT condition FROM light WHERE room='" + str + "';");
        int         cond = 0;
        int         rc   = sqlite3_exec(
            m_db, query.c_str(),
            [](void* data, int argc, char** argv, char** az_col_name) -> int {
                int* cond = static_cast<int*>(data);
                *cond     = std::atoi(argv[0]);
                return 0;
            },
            &cond, nullptr);
        return cond != 0;
    }

  private:
    sqlite3*    m_db;
    std::string m_name;
    void        create() {
        std::string create_query = "CREATE TABLE IF NOT EXISTS light ("
                                          "room TEXT PRIMARY KEY NOT NULL, "
                                          "condition BOOLEAN NOT NULL)";
        int         rc           = sqlite3_exec(m_db, create_query.c_str(), nullptr, nullptr, nullptr);
        if (rc != SQLITE_OK) {
            throw std::runtime_error("Error creating table: " + std::string(sqlite3_errmsg(m_db)));
        }

        insert_data("Living room", false);
        insert_data("Kitchen", false);
        insert_data("Bathroom", false);
    }

    void insert_data(const std::string& room, bool condition) {
        std::string   select_query = "SELECT room FROM light WHERE room = '" + room + "'";
        sqlite3_stmt* stmt;

        int rc = sqlite3_prepare_v2(m_db, select_query.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "Error preparing statement: " << sqlite3_errmsg(m_db) << std::endl;
            return;
        }

        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {
            sqlite3_finalize(stmt);
        } else {
            std::string condition_str = condition ? "1" : "0";
            std::string insert_query  = "INSERT INTO light (room, condition)"
                                        "VALUES ('" +
                                       room + "', " + condition_str + ")";
            rc = sqlite3_exec(m_db, insert_query.c_str(), nullptr, nullptr, nullptr);
            if (rc != SQLITE_OK) {
                std::cerr << "Error executing query: " << sqlite3_errmsg(m_db) << std::endl;
                return;
            }
        }
    }
};

class SecurityDatabase
{
  public:
    SecurityDatabase(const std::string& db_name) : m_name(db_name) {
        int rc = sqlite3_open(db_name.c_str(), &m_db);
        if (rc != SQLITE_OK) {
            std::cerr << "Error opening database: " << sqlite3_errmsg(m_db) << std::endl;
            return;
        }
        create();
    }
    ~SecurityDatabase() {
        if (m_db) {
            sqlite3_close(m_db);
            m_db = nullptr;
        }
    }

    bool update_security_condition(bool value) {
        const char* condition    = value ? "1" : "0";
        char*       update_query = sqlite3_mprintf("UPDATE security SET condition = '%q' WHERE name = 'security'", condition);
        int         rc           = sqlite3_exec(m_db, update_query, nullptr, nullptr, nullptr);
        sqlite3_free(update_query);
        return rc == SQLITE_OK;
    }

    bool is_open() const {
        return m_db != nullptr;
    }

    bool is_security_enabled() {
        std::string query("SELECT condition FROM security WHERE name='security';");
        int         cond = 0;
        int         rc   = sqlite3_exec(
            m_db, query.c_str(),
            [](void* data, int argc, char** argv, char** az_col_name) -> int {
                int* cond = static_cast<int*>(data);
                *cond     = std::atoi(argv[0]);
                return 0;
            },
            &cond, nullptr);
        return cond != 0;
    }

  private:
    std::string m_name;
    sqlite3*    m_db = nullptr;
    static int  callback_count(void* data, int argc, char** argv, char** /*azColName*/) {
        int* count = static_cast<int*>(data);
        *count     = std::atoi(argv[0]);
        return 0;
    }
    void create() {
        std::string create_query("CREATE TABLE IF NOT EXISTS security (name TEXT, condition BOOLEAN);");
        int         rc = sqlite3_exec(m_db, create_query.c_str(), nullptr, nullptr, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "Error creating table: " << sqlite3_errmsg(m_db) << std::endl;
            return;
        } else {
            // Проверяем не существует ли запись с именем "security" в таблице "security"
            std::string select_query("SELECT COUNT(*) FROM security WHERE name='security';");
            int         count = 0;
            rc                = sqlite3_exec(m_db, select_query.c_str(), callback_count, &count, nullptr);
            if (rc != SQLITE_OK) {
                std::cerr << "Error selecting record count: " << sqlite3_errmsg(m_db) << std::endl;
                return;
            } else {
                if (count == 0) {
                    // Если записи с именем "security" нет, то добавляем ее в таблицу
                    std::string insert_query("INSERT INTO security (name, condition) VALUES ('security', false);");
                    rc = sqlite3_exec(m_db, insert_query.c_str(), nullptr, nullptr, nullptr);
                    if (rc != SQLITE_OK) {
                        std::cerr << "Error inserting record: " << sqlite3_errmsg(m_db) << std::endl;
                        return;
                    }
                }
            }
        }
    }
};

class TemperatureDatabase
{
  public:
    TemperatureDatabase(const std::string& db_name) : m_name(db_name) {
        int rc = sqlite3_open(db_name.c_str(), &m_db);
        if (rc != SQLITE_OK) {
            std::cerr << "Error opening database: " << sqlite3_errmsg(m_db) << std::endl;
            return;
        }
        create();
    }
    ~TemperatureDatabase() {
        if (m_db) {
            sqlite3_close(m_db);
            m_db = nullptr;
        }
    }

    bool update_security_condition(int value) {
        char* update_query = sqlite3_mprintf("UPDATE temperature SET degrees = %d WHERE name = 'temperature'", value);
        int   rc           = sqlite3_exec(m_db, update_query, nullptr, nullptr, nullptr);
        sqlite3_free(update_query);
        return rc == SQLITE_OK;
    }

    bool is_open() const {
        return m_db != nullptr;
    }

    int get_temperature_from_database() {
        std::string query("SELECT degrees FROM temperature WHERE name='temperature';");
        int         temp = -1;

        int rc = sqlite3_exec(
            m_db, query.c_str(),
            [](void* data, int argc, char** argv, char** az_col_name) -> int {
                int* temp = static_cast<int*>(data);
                *temp     = std::stoi(argv[0]);
                return 0;
            },
            &temp, nullptr);

        if (rc != SQLITE_OK) {
            std::cerr << "Error executing query: " << sqlite3_errmsg(m_db) << std::endl;
        }

        return temp;
    }

  private:
    std::string m_name;
    sqlite3*    m_db = nullptr;
    static int  callback_count(void* data, int argc, char** argv, char** /*azColName*/) {
        int* count = static_cast<int*>(data);
        *count     = std::atoi(argv[0]);
        return 0;
    }
    void create() {
        std::string create_query("CREATE TABLE IF NOT EXISTS temperature (name TEXT, degrees INTEGER);");
        int         rc = sqlite3_exec(m_db, create_query.c_str(), nullptr, nullptr, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "Error creating table: " << sqlite3_errmsg(m_db) << std::endl;
            return;
        } else {
            std::string select_query("SELECT COUNT(*) FROM temperature WHERE name='temperature';");
            int         count = 0;
            rc                = sqlite3_exec(m_db, select_query.c_str(), callback_count, &count, nullptr);
            if (rc != SQLITE_OK) {
                std::cerr << "Error selecting record count: " << sqlite3_errmsg(m_db) << std::endl;
                return;
            } else {
                if (count == 0) {
                    // Если записи с именем "temperature" нет, то добавляем ее в таблицу
                    std::string insert_query("INSERT INTO temperature (degrees, name) VALUES (25, 'temperature');");
                    rc = sqlite3_exec(m_db, insert_query.c_str(), nullptr, nullptr, nullptr);
                    if (rc != SQLITE_OK) {
                        std::cerr << "Error inserting record: " << sqlite3_errmsg(m_db) << std::endl;
                        return;
                    }
                }
            }
        }
    }
};

void handle_connection(tcp::socket& socket) {
    std::array<char, 256>     buffer;
    boost::system::error_code error;

    SecurityDatabase    s_db("security.db");
    TemperatureDatabase t_db("temperature.db");
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
                    response = "Security condition updated successfully: " + std::string(s_db.is_security_enabled() ? "on" : "off") + ".";

                } else {
                    response = "Failed to update security condition\n";
                }
            } else {
                std::cout << "Failed to open database" << std::endl;
            }
        } else if (message.substr(0, 11) == "temperature") {
            if (t_db.is_open()) {
                std::cout << message.substr(11, 13);
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