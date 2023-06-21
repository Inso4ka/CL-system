#include "columnDatabase.h"

ColumnDatabase::ColumnDatabase(const std::string& db_name) : m_name(db_name) {
    int rc = sqlite3_open(db_name.c_str(), &m_db);
    if (rc != SQLITE_OK) {
        std::cerr << "Error opening database: " << sqlite3_errmsg(m_db) << std::endl;
        return;
    }
    create();
}
ColumnDatabase::~ColumnDatabase() {
    if (m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}

bool ColumnDatabase::update_column_condition(bool value) {
    const char* condition    = value ? "1" : "0";
    char*       update_query = sqlite3_mprintf("UPDATE column SET condition = '%q' WHERE name = 'column'", condition);
    int         rc           = sqlite3_exec(m_db, update_query, nullptr, nullptr, nullptr);
    sqlite3_free(update_query);
    return rc == SQLITE_OK;
}

bool ColumnDatabase::is_open() const {
    return m_db != nullptr;
}

bool ColumnDatabase::is_column_enabled() {
    std::string query("SELECT condition FROM column WHERE name='column';");
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

int ColumnDatabase::callback_count(void* data, int argc, char** argv, char** /*azColName*/) {
    int* count = static_cast<int*>(data);
    *count     = std::atoi(argv[0]);
    return 0;
}
void ColumnDatabase::create() {
    std::string create_query("CREATE TABLE IF NOT EXISTS column (name TEXT, condition BOOLEAN);");
    int         rc = sqlite3_exec(m_db, create_query.c_str(), nullptr, nullptr, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Error creating table: " << sqlite3_errmsg(m_db) << std::endl;
        return;
    } else {
        // Проверяем не существует ли запись с именем "security" в таблице "security"
        std::string select_query("SELECT COUNT(*) FROM column WHERE name='column';");
        int         count = 0;
        rc                = sqlite3_exec(m_db, select_query.c_str(), callback_count, &count, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "Error selecting record count: " << sqlite3_errmsg(m_db) << std::endl;
            return;
        } else {
            if (count == 0) {
                // Если записи с именем "security" нет, то добавляем ее в таблицу
                std::string insert_query("INSERT INTO column (name, condition) VALUES ('column', false);");
                rc = sqlite3_exec(m_db, insert_query.c_str(), nullptr, nullptr, nullptr);
                if (rc != SQLITE_OK) {
                    std::cerr << "Error inserting record: " << sqlite3_errmsg(m_db) << std::endl;
                    return;
                }
            }
        }
    }
}