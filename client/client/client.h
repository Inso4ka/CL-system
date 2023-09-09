#pragma once

#include "hashfunc.h"
#include <boost/asio.hpp> // Подключение библиотеки Boost.Asio
#include <iostream> // Подключение библиотеки для ввода/вывода
#include <unordered_set>

using boost::asio::ip::tcp; // Пространство имен для работы с протоколом TCP

// Определение класса SmartHomeSystem
class SmartHomeSystem
{
public:
    // Конструктор класса с передачей объекта io_service по ссылке
    explicit SmartHomeSystem(boost::asio::io_service &io_service);
    bool checkConnection(const std::string &ip, const std::string &port);
    std::string start(const std::string &str);
    bool getCondition();

private:
    // Объекты для работы с сокетами, резолвером и сетевыми точками
    tcp::socket m_socket;
    tcp::resolver               m_resolver;
    tcp::resolver::results_type m_endpoints;
    bool m_condition = false;

    // Методы для обработки различных опций в приложении

    std::string receive_response();
    std::string handle_command(const std::string &command);
};
