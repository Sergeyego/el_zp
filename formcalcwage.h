#ifndef FORMCALCWAGE_H
#define FORMCALCWAGE_H

#include <QWidget>
#include "modelro.h"
#include <QSqlQuery>
#include <QMultiHash>
#include "db/tablemodel.h"

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
    TableModel *modelTableDataVid;
    void setBlock(bool b);
    struct tabelData {
        QString name;
        double kvo;
        double tarif;
        double zpl;
        double dopl;
        double extr;
        double bonus;
        double nrm;
        double total;
    };
    QMultiHash<int,tabelData> mapData;
    QString orgName, sign;

private slots:
    void reCalc();
    void updTableData(QModelIndex ind);
    void saveTabelXlsx();
    void tabelShort();
    void tabel();
    void setDataModel();
    void updTitle();
};

#endif // FORMCALCWAGE_H
