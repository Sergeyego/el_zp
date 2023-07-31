#ifndef FORMCALCWAGE_H
#define FORMCALCWAGE_H

#include <QWidget>
#include "modelro.h"
#include <QSqlQuery>

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
    ModelRo *modelRab;
    ModelRo *modelTableData;
    void setBlock(bool b);

private slots:
    void reCalc();
    void updTableData(QModelIndex ind);
    void saveTabelXlsx();
    void tabelShort();
    void tabel();
};

#endif // FORMCALCWAGE_H
