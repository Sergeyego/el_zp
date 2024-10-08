#ifndef FORMCALCWAGE_H
#define FORMCALCWAGE_H

#include <QWidget>
#include "modelro.h"
#include <QSqlQuery>
#include <QMultiHash>
#include "db/tablemodel.h"
#include "progressexecutor.h"

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
    ProgressExecutor *sqlExecutor;
    ModelRo *modelRab;
    TableModel *modelTableData;
    TableModel *modelTableDataVid;
    int current_id_rab;
    void setBlock(bool b);
    void loadSettings();
    void saveSettings();
    struct tabelData {
        QDate date;
        QString name;
        double s_kvo;
        double kvo;
        double prk;
        double prn;
        double pr;
        double tarif;
        double zpl;
        double dopl;
        double extr;
        double night;
        double premk;
        double premn;
        double prem;
        double total;
    };
    QMultiHash<int,tabelData> hlong;
    QMultiHash<int,tabelData> hshort;
    QHash<int,tabelData> hsum;
    QString orgName, sign;


private slots:
    void upd();
    void updFinished();
    void updTableData(QModelIndex ind);
    void saveTabelXlsx();
    void tabelShort();
    void tabel();
    void setDataModel();
    void updTitle();
};

#endif // FORMCALCWAGE_H
