#ifndef FORMCOMPARE_H
#define FORMCOMPARE_H

#include <QWidget>
#include "db/tablemodel.h"
#include <QSqlQuery>

namespace Ui {
class FormCompare;
}

class ModelCompare : public TableModel
{
    Q_OBJECT

public:
    explicit ModelCompare(QWidget *parent = nullptr);
    void refresh(QDate beg, QDate end, bool err);
};

class FormCompare : public QWidget
{
    Q_OBJECT

public:
    explicit FormCompare(QWidget *parent = nullptr);
    ~FormCompare();

private:
    Ui::FormCompare *ui;
    ModelCompare *modelCompare;

private slots:
    void upd();
};

#endif // FORMCOMPARE_H
