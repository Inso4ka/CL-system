#include "temperature.h"
#include "ui_temperature.h"

Temperature::Temperature(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Temperature)
{
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint
                   | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
    ui->setupUi(this);
}

Temperature::~Temperature()
{
    delete ui;
}

void Temperature::on_m_temperature_sl_valueChanged(int value)
{
    ui->m_temperature_lbl->setText(QString::number(value));
}

void Temperature::on_m_cancel_btn_clicked()
{
    close();
}

void Temperature::on_m_connect_btn_clicked()
{
    m_value = ui->m_temperature_sl->value();
    emit valueChanged(m_value);
}
