#include "connection.h"
#include "ui_connection.h"

Connection::Connection(SmartHomeSystem &system, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Connection)
    , m_system(system)
{
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint
                   | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
    ui->setupUi(this);
}

Connection::~Connection()
{
    delete ui;
}

void Connection::on_m_cancel_btn_clicked()
{
    close();
}

void Connection::on_m_connect_btn_clicked()
{
    QString ip, port;
    ip = ui->m_ip_le->text();
    port = ui->m_port_le->text();
    if (m_system.checkConnection(ip.toStdString(), port.toStdString())) {
        QMessageBox::information(nullptr, "Success", "Connection established successfully.");
        close();
    } else {
        QMessageBox::critical(nullptr, "Error", "Failed to establish connection.");
    }
}
