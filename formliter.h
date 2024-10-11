#ifndef FORMLITER_H
#define FORMLITER_H

#include <QWidget>
#include "db/dbtablemodel.h"
#include "rels.h"
#include "progressexecutor.h"

namespace Ui {
class FormLiter;
}

class FormLiter : public QWidget
{
    Q_OBJECT

public:
    explicit FormLiter(QWidget *parent = nullptr);
    ~FormLiter();

private:
    Ui::FormLiter *ui;
    DbTableModel *modelLiter;
    ProgressExecutor *sqlExecutor;

private slots:
    void upd();
    void fullUpd();
};

#endif // FORMLITER_H
