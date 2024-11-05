#ifndef FORMCOMPARE_H
#define FORMCOMPARE_H

#include <QWidget>
#include "db/tablemodel.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <cmath>

namespace Ui {
class FormCompare;
}

class ModelCompare : public TableModel
{
    Q_OBJECT

public:
    explicit ModelCompare(QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int role) const;
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
    void save();
};

#endif // FORMCOMPARE_H
