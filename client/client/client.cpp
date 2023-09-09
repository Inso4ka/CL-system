#include "client.h"

// Конструктор
SmartHomeSystem::SmartHomeSystem(boost::asio::io_service &io_service)
    : m_socket(io_service)
    , m_resolver(io_service)
{}

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
std::string SmartHomeSystem::start(const std::string &str)
{
    SHA256 sha;
    if (str.find("temperature") != std::string::npos) {
        std::string modifiedStr = str;
        modifiedStr.erase(0, modifiedStr.find("temperature") + strlen("temperature"));
        sha.update("temperature");
        uint8_t *digest = sha.digest();
        return handle_command(SHA256::toString(digest) + modifiedStr);
    }
    static const std::unordered_set<std::string> validStrings
        = {"security", "column", "show", "living_room", "bathroom", "kitchen", "turnoff"};

    sha.update(str);
    uint8_t *digest = sha.digest();

    if (validStrings.count(str) > 0) {
        return handle_command(SHA256::toString(digest));
    }

    return "";
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

// Реализация общего обработчика команд
std::string SmartHomeSystem::handle_command(const std::string &command)
{
    // Отправляем команду на сервер
    boost::asio::write(m_socket, boost::asio::buffer(command));
    // Ждем ответа и выводим его в консоль
    std::string response = receive_response();
    return response;
}
