#ifndef FORMTARIF_H
#define FORMTARIF_H

#include <QWidget>
#include "modelzon.h"
#include "rels.h"
#include "db/dbtablemodel.h"
#include "modelro.h"
#include "progressexecutor.h"
#include "tndialog.h"

namespace Ui {
class FormTarif;
}

class FormTarif : public QWidget
{
    Q_OBJECT

public:
    explicit FormTarif(QWidget *parent = nullptr);
    ~FormTarif();

private:
    Ui::FormTarif *ui;
    ModelZon *modelZon;
    ModelZon *modelVid;
    ModelZon *modelMark;
    ModelZon *modelDiam;
    ModelZon *modelComp;
    ModelRo *modelJob;
    DbTableModel *modelTarif;
    ProgressExecutor *sqlExecutor;

private slots:
    void upd();
    void updData(QModelIndex ind);
    void updFull();
    void selType();
    void setGrpTarif();
};

#endif // FORMTARIF_H
