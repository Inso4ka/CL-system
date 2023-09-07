#ifndef CONNECTION_H
#define CONNECTION_H

#include <QDialog>
#include <QMessageBox>
#include "client/client.h"

namespace Ui {
class Connection;
}

class Connection : public QDialog
{
    Q_OBJECT

public:
    explicit Connection(SmartHomeSystem &system, QWidget *parent = nullptr);
    ~Connection();

private slots:
    void on_m_cancel_btn_clicked();

    void on_m_connect_btn_clicked();

private:
    Ui::Connection *ui;
    SmartHomeSystem &m_system;
};

#endif // CONNECTION_H
