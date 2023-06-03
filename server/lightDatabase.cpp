#include "lightDatabase.h"

LightDatabase::LightDatabase(const std::string& db_name) : m_name{db_name} {
    int rc = sqlite3_open(db_name.c_str(), &m_db);
    if (rc != SQLITE_OK) {
        std::cerr << "Error opening database: " << sqlite3_errmsg(m_db) << std::endl;
        return;
    }

    create();
}

LightDatabase::~LightDatabase() {
    if (m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}

bool LightDatabase::update_condition(const std::string& room, bool condition) {
    std::string condition_str = condition ? "1" : "0";
    std::string update_query  = "UPDATE light SET condition = " + condition_str + " WHERE room = '" + room + "'";
    int         rc            = sqlite3_exec(m_db, update_query.c_str(), nullptr, nullptr, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Error executing query: " << sqlite3_errmsg(m_db) << std::endl;
        return false;
    }
    return true;
}

bool LightDatabase::is_open() const {
    return m_db != nullptr;
}

bool LightDatabase::is_light_enabled(const std::string& str) {
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

void LightDatabase::create() {
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

void LightDatabase::insert_data(const std::string& room, bool condition) {
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