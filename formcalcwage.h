#ifndef FORMCALCWAGE_H
#define FORMCALCWAGE_H

#include <QWidget>

namespace Ui {
class FormCalcWage;
}

class FormCalcWage : public QWidget
{
    Q_OBJECT

public:
    explicit FormCalcWage(QWidget *parent = 0);
    ~FormCalcWage();

private:
    Ui::FormCalcWage *ui;
};

#endif // FORMCALCWAGE_H
