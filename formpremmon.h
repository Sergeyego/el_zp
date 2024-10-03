#ifndef FORMPREMMON_H
#define FORMPREMMON_H

#include <QWidget>

namespace Ui {
class FormPremMon;
}

class FormPremMon : public QWidget
{
    Q_OBJECT

public:
    explicit FormPremMon(QWidget *parent = nullptr);
    ~FormPremMon();

private:
    Ui::FormPremMon *ui;
};

#endif // FORMPREMMON_H
