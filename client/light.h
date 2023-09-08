#ifndef LIGHT_H
#define LIGHT_H

#include <QDialog>

namespace Ui {
class light;
}

class light : public QDialog
{
    Q_OBJECT

public:
    explicit light(QWidget *parent = nullptr);
    ~light();

private:
    Ui::light *ui;
};

#endif // LIGHT_H
