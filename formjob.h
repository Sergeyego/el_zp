#ifndef FORMJOB_H
#define FORMJOB_H

#include <QWidget>
#include "modelzon.h"
#include "db/dbtablemodel.h"
#include "rels.h"

namespace Ui {
class FormJob;
}

class ModelJob : public DbTableModel
{
    Q_OBJECT

public:
    explicit ModelJob(QWidget *parent = nullptr);
    void refresh(QDate beg, QDate end, QString zon, int id_rb=-1);
};

class ModelShare : public DbTableModel
{
    Q_OBJECT

public:
    explicit ModelShare(QWidget *parent = nullptr);
    void refresh(int id_job);
};

class FormJob : public QWidget
{
    Q_OBJECT

public:
    explicit FormJob(QWidget *parent = nullptr);
    ~FormJob();

private:
    Ui::FormJob *ui;
    ModelZon *modelZon;
    ModelJob *modelJob;

private slots:
    void upd();
};

#endif // FORMJOB_H
