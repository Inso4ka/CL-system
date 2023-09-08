#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QtConcurrent/QtConcurrent>
#include "client/client.h"
#include "connection.h"
#include "temperature.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(boost::asio::io_service &io_service, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_m_exit_btn_clicked();

    void on_m_notification_btn_clicked();

    void on_m_settings_btn_clicked();

    void on_m_security_btn_clicked();

    void on_m_column_btn_clicked();

    void on_m_showall_btn_clicked();

    void on_m_temperature_btn_clicked();

    void onTemperatureValueChanged(int value);

private:
    Ui::MainWindow *ui;
    SmartHomeSystem m_system;
    Connection *m_window;
    Temperature *m_temp;
    std::string getCommandString(const std::string &command);
};

#endif // MAINWINDOW_H
