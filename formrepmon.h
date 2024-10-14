#ifndef FORMREPMON_H
#define FORMREPMON_H

#include <QWidget>
#include "db/tablemodel.h"
#include "progressexecutor.h"

namespace Ui {
class FormRepMon;
}

class FormRepMon : public QWidget
{
    Q_OBJECT

public:
    explicit FormRepMon(QWidget *parent = nullptr);
    ~FormRepMon();

private:
    Ui::FormRepMon *ui;
    ProgressExecutor *executor;
    TableModel *modelRep;

private slots:
    void upd();
    void updFinished();
    void save();
};

#endif // FORMREPMON_H
