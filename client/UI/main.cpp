#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    boost::asio::io_service io_service;
    MainWindow window(io_service);
    window.show();

    return a.exec();
}
