#ifndef LIGHT_H
#define LIGHT_H

#include <QDialog>

namespace Ui {
class Light;
}

class Light : public QDialog
{
    Q_OBJECT

public:
    explicit Light(QWidget *parent = nullptr);
    ~Light();
signals:
    void lightValue(int value);
private slots:
    void on_m_cancel_btn_clicked();

    void on_m_livingroom_btn_clicked();

    void on_m_bathroom_btn_clicked();

    void on_m_kitchen_btn_clicked();

    void on_m_turnoff_btn_clicked();

private:
    Ui::Light *ui;
};

#endif // LIGHT_H
