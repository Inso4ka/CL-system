#pragma once

#include <boost/asio.hpp>   // Подключение библиотеки Boost.Asio
#include <iostream>         // Подключение библиотеки для ввода/вывода

using boost::asio::ip::tcp; // Пространство имен для работы с протоколом TCP

// Определение класса SmartHomeSystem
class SmartHomeSystem
{
  public:
    // Конструктор класса с передачей объекта io_service по ссылке
    explicit SmartHomeSystem(boost::asio::io_service& io_service);
    // Метод для запуска приложения
    void run();

  private:
    // Объекты для работы с сокетами, резолвером и сетевыми точками
    tcp::socket                 m_socket;
    tcp::resolver               m_resolver;
    tcp::resolver::results_type m_endpoints;

    // Методы для обработки различных опций в приложении
    void        start();
    std::string receive_response();
    void        handle_light();
    void        handle_security();
    void        handle_temperature();
    void        handle_show_conditions();
    void        handle_column();
    void        handle_command(const std::string& command);
};
