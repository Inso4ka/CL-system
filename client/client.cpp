#include "client.h"

// Конструктор
SmartHomeSystem::SmartHomeSystem(boost::asio::io_service& io_service) : m_socket(io_service), m_resolver(io_service), m_endpoints(m_resolver.resolve("localhost", "1234")) { }

// Реализация метода run()
void SmartHomeSystem::run() {
    boost::asio::connect(m_socket, m_endpoints);
    start();
}

// Реализация метода start()
void SmartHomeSystem::start() {
    // Выводим доступные опции пользователю
    std::cout << "Welcome to our smart home system. With our app, you can control the temperature, adjust the lighting, and keep your home safe and secure.\n\n"
              << "1. on/off light\n"
              << "2. on/off security\n"
              << "3. control temperature\n"
              << "4. on/off column\n"
              << "5. show conditions\n"
              << "6. exit\n\n";

    bool is_running = true;
    int  option;

    // Запускаем цикл выбора опций, пока пользователь не выберет выход
    while (is_running) {
        std::cout << "Choose an option: ";
        std::cin >> option;

        switch (option) {
            // Обрабатываем опции для управления освещением, системой безопасности, температурой и прочими функциями
            // Выходим из цикла при выборе опции выхода
            case 1:
                handle_light();
                break;
            case 2:
                handle_security();
                break;
            case 3:
                handle_temperature();
                break;
            case 4:
                handle_column();
                break;
            case 5:
                handle_show_conditions();
                break;
            case 6:
                is_running = false;
                std::cout << "Exiting the app.\n";
                break;
            default:
                std::cout << "Invalid option.\n";
                break;
        }
    }
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
void SmartHomeSystem::handle_command(const std::string& command) {
    // Отправляем команду на сервер
    boost::asio::write(m_socket, boost::asio::buffer(command));
    // Ждем ответа и выводим его в консоль
    std::string response = receive_response();
    std::cout << response << std::endl;
}