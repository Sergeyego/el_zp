#ifndef FORMRECALC_H
#define FORMRECALC_H

#include <QWidget>
#include <QMessageBox>
#include <QProgressDialog>
#include "modelro.h"

namespace Ui {
class FormRecalc;
}

class ModelVip : public ModelRo
{
    Q_OBJECT

public:
    explicit ModelVip(QObject *parent = nullptr);
    QVariant data(const QModelIndex &item, int role) const;
    void refresh(QDate beg, QDate end);
};

class FormRecalc : public QWidget
{
    Q_OBJECT

public:
    explicit FormRecalc(QWidget *parent = nullptr);
    ~FormRecalc();

private:
    Ui::FormRecalc *ui;
    ModelVip *modelVip;

private slots:
    void upd();
    void recalc();
};

#endif // FORMRECALC_H
