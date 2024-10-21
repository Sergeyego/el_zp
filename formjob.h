#ifndef FORMJOB_H
#define FORMJOB_H

#include <QWidget>
#include "modelzon.h"
#include "db/dbtablemodel.h"
#include "rels.h"
#include "db/executor.h"

namespace Ui {
class FormJob;
}

class ModelJob : public DbTableModel
{
    Q_OBJECT

public:
    explicit ModelJob(QWidget *parent = nullptr);
    QVariant data(const QModelIndex &index, int role) const;
    void refresh(QDate beg, QDate end, QString zon, int id_rb=-1, bool zero=false, QString parti="");
    void setIdBrig(int id);
    int getIdBrig();
private:
    Executor *executorSt;
    int id_brig;
    QDate dbeg, dend;
    QSet<int> notOk;

private slots:
    void refreshState();
    void stFinished();
};

class ModelShare : public DbTableModel
{
    Q_OBJECT

public:
    explicit ModelShare(QWidget *parent = nullptr);
    void refresh(int id_job);
    void insShare();

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
    ModelShare *modelShare;
    bool updTempTables();
protected:
    virtual void keyPressEvent (QKeyEvent * e );

private slots:
    void upd();
    void updShare(QModelIndex ind);
    void chkRab(int row);
};

#endif // FORMJOB_H
