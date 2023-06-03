#include "temperatureDatabase.h"

TemperatureDatabase::TemperatureDatabase(const std::string& db_name) : m_name(db_name) {
    int rc = sqlite3_open(db_name.c_str(), &m_db);
    if (rc != SQLITE_OK) {
        std::cerr << "Error opening database: " << sqlite3_errmsg(m_db) << std::endl;
        return;
    }
    create();
}
TemperatureDatabase::~TemperatureDatabase() {
    if (m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}

bool TemperatureDatabase::update_security_condition(int value) {
    char* update_query = sqlite3_mprintf("UPDATE temperature SET degrees = %d WHERE name = 'temperature'", value);
    int   rc           = sqlite3_exec(m_db, update_query, nullptr, nullptr, nullptr);
    sqlite3_free(update_query);
    return rc == SQLITE_OK;
}

bool TemperatureDatabase::is_open() const {
    return m_db != nullptr;
}

int TemperatureDatabase::get_temperature_from_database() {
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

int TemperatureDatabase::callback_count(void* data, int argc, char** argv, char** /*azColName*/) {
    int* count = static_cast<int*>(data);
    *count     = std::atoi(argv[0]);
    return 0;
}
void TemperatureDatabase::create() {
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