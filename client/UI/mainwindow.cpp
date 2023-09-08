#include "mainwindow.h"
#include <QtWidgets>
#include "ui_mainwindow.h"

MainWindow::MainWindow(boost::asio::io_service &io_service, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_system(io_service)
{
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint
                   | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_m_exit_btn_clicked()
{
    close();
}

void MainWindow::on_m_notification_btn_clicked()
{
    QMessageBox::information(
        this,
        "Notification",
        "Sorry, there is nothing new for your smart home application at the moment.");
}

void MainWindow::on_m_settings_btn_clicked()
{
    m_window = new Connection(m_system);
    m_window->show();
}

std::string MainWindow::getCommandString(const std::string &command)
{
    if (!m_system.getCondition()) {
        QMessageBox::critical(nullptr, "Error", "To begin, you must connect to the server.");
        return "";
    }
    QFuture<std::string> future = QtConcurrent::run([&]() { return m_system.start(command); });
    future.waitForFinished();
    return future.result();
}

void MainWindow::on_m_security_btn_clicked()
{
    const std::string &response = getCommandString("security");
    if (!response.empty()) {
        QMessageBox::information(this, "Notification", QString::fromStdString(response));
    }
}

void MainWindow::on_m_column_btn_clicked()
{
    const std::string &response = getCommandString("column");
    if (!response.empty()) {
        QMessageBox::information(this, "Notification", QString::fromStdString(response));
    }
}

void MainWindow::on_m_showall_btn_clicked()
{
    const std::string &response = getCommandString("show");
    if (!response.empty()) {
        QMessageBox::information(this, "Notification", QString::fromStdString(response));
    }
}

void MainWindow::on_m_temperature_btn_clicked()
{
    m_temp = new Temperature;
    connect(m_temp, &Temperature::valueChanged, this, &MainWindow::onTemperatureValueChanged);
    m_temp->show();
}

void MainWindow::onTemperatureValueChanged(int value)
{
    const std::string &response = getCommandString("temperature" + std::to_string(value));
    if (!response.empty()) {
        QMessageBox::information(this, "Notification", QString::fromStdString(response));
    }
}
