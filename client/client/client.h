#pragma once

#include <boost/asio.hpp>   // Подключение библиотеки Boost.Asio
#include <iostream>         // Подключение библиотеки для ввода/вывода

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
    void        handle_light();
    void        handle_security();
    void        handle_temperature();
    void        handle_show_conditions();
    void        handle_column();
    std::string handle_command(const std::string &command);
};
