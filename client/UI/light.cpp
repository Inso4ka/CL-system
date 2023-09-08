#include "light.h"
#include "ui_light.h"

Light::Light(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Light)
{
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint
                   | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
    ui->setupUi(this);
}

Light::~Light()
{
    delete ui;
}

void Light::on_m_cancel_btn_clicked()
{
    close();
}

void Light::on_m_livingroom_btn_clicked()
{
    emit lightValue(1);
}

void Light::on_m_bathroom_btn_clicked()
{
    emit lightValue(2);
}

void Light::on_m_kitchen_btn_clicked()
{
    emit lightValue(3);
}

void Light::on_m_turnoff_btn_clicked()
{
    emit lightValue(4);
}
