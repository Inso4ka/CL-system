#include "client.h"

// Конструктор
SmartHomeSystem::SmartHomeSystem(boost::asio::io_service &io_service)
    : m_socket(io_service)
    , m_resolver(io_service)
{}

// Реализация метода run()
void SmartHomeSystem::run()
{
    try {
        boost::asio::connect(m_socket, m_endpoints);
        start();
    } catch (const boost::system::system_error &e) {
        std::cerr << "Error connecting to the server: " << e.what() << std::endl;
        // Дополнительная обработка ошибки, включая повторную попытку подключения или вывод сообщения пользователю
    }
}

bool SmartHomeSystem::checkConnection(const std::string &ip, const std::string &port)
{
    m_condition = false;
    // Проверка корректности IP-адреса
    boost::system::error_code ipAddressError;
    boost::asio::ip::address_v4::from_string(ip, ipAddressError);
    if (ipAddressError) {
        std::cerr << "Invalid IP address: " << ip << std::endl;
        return false;
    }

    // Проверка корректности порта
    unsigned short portNumber = 0;
    try {
        portNumber = std::stoul(port);
    } catch (const std::exception &e) {
        std::cerr << "Invalid port number: " << port << std::endl;
        return false;
    }

    try {
        m_endpoints = m_resolver.resolve(boost::asio::ip::tcp::v4(), ip, std::to_string(portNumber));
        boost::asio::connect(m_socket, m_endpoints);
        m_condition = true;
        return true;
    } catch (const boost::system::system_error &e) {
        std::cerr << "Cannot establish connection: " << e.what() << std::endl;
        return false;
    }
}

bool SmartHomeSystem::getCondition()
{
    return m_condition;
}

// Реализация метода start()
std::string SmartHomeSystem::start()
{
    return handle_command("security");
}

// Функция чтения ответа от сервера
std::string SmartHomeSystem::receive_response() {
    std::array<char, 256>     buffer;
    boost::system::error_code error;

    // Читаем ответ от сервера
    size_t bytes_transferred = m_socket.read_some(boost::asio::buffer(buffer), error);

    if (error) {
        std::cerr << "Error receiving response: " << error.message() << std::endl;
        return std::string();
    } else {
        return std::string(buffer.data(), bytes_transferred);
    }
}

// Реализация обработчика для управления освещением
void SmartHomeSystem::handle_light() {
    bool is_running = true;
    int  option;

    while (is_running) {
        // Выводим доступные опции для управления освещением
        std::cout << "Turn on/off light\n"
                  << "1. living room\n"
                  << "2. kitchen\n"
                  << "3. bathroom\n"
                  << "4. turn off light\n"
                  << "5. cancel\n\n";
        std::cout << "Choose a place to turn on/off the light: ";
        std::cin >> option;

        switch (option) {
            // Отправляем команду на сервер в зависимости от выбранной опции
            case 1:
                handle_command("living_room");
                break;
            case 2:
                handle_command("kitchen");
                break;
            case 3:
                handle_command("bathroom");
                break;
            case 4:
                handle_command("turnoff");
                break;
            // Выходим из цикла при выборе опции отмены
            case 5:
                is_running = false;
                std::cout << "Canceling the operation.\n\n";
                break;
            default:
                std::cout << "Invalid option.\n";
                break;
        }
    }
}

// Реализация обработчика для управления колонкой
void SmartHomeSystem::handle_column() {
    handle_command("column");
}

// Реализация обработчика для управления системой безопасности
void SmartHomeSystem::handle_security() {
    handle_command("security");
}

// Реализация обработчика для управления температурой
void SmartHomeSystem::handle_temperature() {
    int num;
    do {
        std::cout << "Enter the desired temperature (in degrees Celsius [-10 : 40]): ";
        std::cin >> num;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    } while (num < -10 || num > 40);
    handle_command("temperature" + std::to_string(num));
}

// Реализация обработчика для вывода информации о состоянии системы
void SmartHomeSystem::handle_show_conditions() {
    handle_command("show");
}

// Реализация общего обработчика команд
std::string SmartHomeSystem::handle_command(const std::string &command)
{
    // Отправляем команду на сервер
    boost::asio::write(m_socket, boost::asio::buffer(command));
    // Ждем ответа и выводим его в консоль
    std::string response = receive_response();
    return response;
}
