#ifndef FORMNORMPRESS_H
#define FORMNORMPRESS_H

#include <QWidget>
#include "rels.h"
#include "db/dbtablemodel.h"
#include "modelro.h"

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
    ModelRo *modelPress;

private slots:
    void updCont(QModelIndex ind);
    void updPress();
    void updMarkFinished();
    void updModelPress();
};

#endif // FORMNORMPRESS_H
