#ifndef FORMNORMPACK_H
#define FORMNORMPACK_H

#include <QWidget>
#include "rels.h"
#include "db/dbtablemodel.h"
#include "packnormdialog.h"

namespace Ui {
class FormNormPack;
}

class FormNormPack : public QWidget
{
    Q_OBJECT

public:
    explicit FormNormPack(QWidget *parent = nullptr);
    ~FormNormPack();

private:
    Ui::FormNormPack *ui;
    DbTableModel *modelNorm;

private slots:
    void updCont(QModelIndex ind);
    void updMarkFinished();
    void normAllMark();
    void upd();
};

#endif // FORMNORMPACK_H
