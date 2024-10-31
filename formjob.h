#ifndef FORMJOB_H
#define FORMJOB_H

#include <QWidget>
#include "modelzon.h"
#include "db/dbtablemodel.h"
#include "rels.h"
#include "db/executor.h"
#include "dialogsplit.h"

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
    Executor *executorNorm;
    Executor *executorPart;
    int id_brig;
    QDate dbeg, dend;
    QSet<int> notOkNorm;
    QSet<int> notOkPart;

private slots:
    void refreshState();
    void stNormFinished();
    void stPartFinished();
};

class ModelShare : public DbTableModel
{
    Q_OBJECT

public:
    explicit ModelShare(QWidget *parent = nullptr);
    void refresh(int id_job);

private:
    int idCopy;

public slots:
    void insShare();
    void copy();
    void paste();
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
    void upJob();
    void downJob();
    void split();
};

#endif // FORMJOB_H
