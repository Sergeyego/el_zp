#ifndef FORMPREMMON_H
#define FORMPREMMON_H

#include <QWidget>
#include "rels.h"
#include "db/dbmapper.h"

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
    DbTableModel *modelPremNorm;
    DbTableModel *modelList;
    DbTableModel *modelListData;
    DbMapper *mapper;

private slots:
    void upd();
    void updZonFinished();
    void updZonCont(QModelIndex ind);
    void updListCont(int ind);
    void setPageNorm();
    void setPageList();
};

#endif // FORMPREMMON_H
