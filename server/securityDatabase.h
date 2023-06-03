#pragma once // Защита от повторного включения заголовочного файла

#include <iostream>
#include <sqlite3.h>
#include <string>

class SecurityDatabase
{
  public:
    SecurityDatabase(const std::string& db_name); // Конструктор класса, принимающий имя базы данных.
    ~SecurityDatabase();                          // Деструктор класса.
    bool update_security_condition(bool value);   // Метод для обновления состояния безопасности.
    bool is_open() const;                         // Метод для проверки открыта ли база данных.
    bool is_security_enabled();                   // Метод для проверки включен ли режим безопасности.

  private:
    std::string m_name;         // Имя базы данных.
    sqlite3*    m_db = nullptr; // Указатель на базу данных SQLite.
    static int  callback_count(void* data, int argc, char** argv, char** /*azColName*/);
    // Статический метод обратного вызова для получения результата запроса к базе данных.
    void create(); // Метод для создания таблицы в базе данных.
};