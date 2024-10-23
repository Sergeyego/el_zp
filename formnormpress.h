#ifndef FORMNORMPRESS_H
#define FORMNORMPRESS_H

#include <QWidget>
#include "rels.h"
#include "db/dbtablemodel.h"
#include "allpressdialog.h"

namespace Ui {
class FormNormPress;
}

class FormNormPress : public QWidget
{
    Q_OBJECT

public:
    explicit FormNormPress(QWidget *parent = nullptr);
    ~FormNormPress();

private:
    Ui::FormNormPress *ui;
    DbTableModel *modelNorm;

private slots:
    void updCont(QModelIndex ind);
    void updPress();
    void updMarkFinished();
    void normAllPress();
    void upd();
};

#endif // FORMNORMPRESS_H
