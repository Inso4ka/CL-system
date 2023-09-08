#include "light.h"
#include "ui_light.h"

light::light(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::light)
{
    ui->setupUi(this);
}

light::~light()
{
    delete ui;
}
