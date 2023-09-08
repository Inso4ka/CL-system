#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include <QDialog>

namespace Ui {
class Temperature;
}

class Temperature : public QDialog
{
    Q_OBJECT

public:
    explicit Temperature(QWidget *parent = nullptr);
    ~Temperature();

signals:
    void valueChanged(int value);

private slots:
    void on_m_temperature_sl_valueChanged(int value);

    void on_m_cancel_btn_clicked();

    void on_m_connect_btn_clicked();

private:
    Ui::Temperature *ui;
    int m_value;
};

#endif // TEMPERATURE_H
