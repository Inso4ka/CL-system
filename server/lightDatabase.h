#pragma once // Защита от повторного включения заголовочного файла

#include <iostream>
#include <sqlite3.h>
#include <string>

class LightDatabase
{
  public:
    LightDatabase(const std::string& db_name);                      // Конструктор класса, принимающий имя базы данных
    ~LightDatabase();                                               // Деструктор класса
    bool update_condition(const std::string& room, bool condition); // Обновление состояния света в заданной комнате
    bool is_open() const;                                           // Проверка, открыта ли база данных
    bool is_light_enabled(const std::string& str);                  // Проверка, включен ли свет в заданной комнате

  private:
    sqlite3*    m_db;                                                 // Указатель на объект базы данных
    std::string m_name;                                               // Имя базы данных
    void        create();                                             // Создание таблицы в базе данных
    void        insert_data(const std::string& room, bool condition); // Вставка данных в таблицу базы данных
};
