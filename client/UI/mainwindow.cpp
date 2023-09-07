#include "mainwindow.h"
#include <QtWidgets>
#include "ui_mainwindow.h"

MainWindow::MainWindow(boost::asio::io_service &io_service, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , system(io_service)
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
    window = new Connection(system);
    window->show();
}

void MainWindow::on_m_security_btn_clicked()
{
    system.start();
}
