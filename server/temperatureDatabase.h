#pragma once // Защита от повторного включения заголовочного файла

#include <iostream>
#include <sqlite3.h>
#include <string>

class TemperatureDatabase
{
  public:
    TemperatureDatabase(const std::string& db_name); // Конструктор класса, принимающий имя базы данных.
    ~TemperatureDatabase();                          // Деструктор класса.
    bool update_security_condition(int value);       // Метод для обновления температурных данных.
    bool is_open() const;                            // Метод для проверки открыта ли база данных.
    int  get_temperature_from_database();            // Метод для получения последних температурных данных.

  private:
    std::string m_name;         // Имя базы данных.
    sqlite3*    m_db = nullptr; // Указатель на базу данных SQLite.
    static int  callback_count(void* data, int argc, char** argv, char** /*azColName*/);
    // Статический метод обратного вызова для получения результата запроса к базе данных.
    void create(); // Метод для создания таблицы в базе данных.
};