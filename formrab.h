#ifndef FORMRAB_H
#define FORMRAB_H

#include <QWidget>
#include "db/dbtablemodel.h"
#include "db/dbmapper.h"
#include "rels.h"
#include "modelro.h"
#include "httpsyncmanager.h"
#include "progressreportdialog.h"

namespace Ui {
class FormRab;
}

class FormRab : public QWidget
{
    Q_OBJECT

public:
    explicit FormRab(QWidget *parent = nullptr);
    ~FormRab();

private:
    Ui::FormRab *ui;
    DbTableModel *modelRab;
    DbMapper *mapper;
    ModelRo *modelInfo;
    ProgressReportDialog *pprd;
    void loadSettings();
    void saveSettings();

private slots:
    void ins();
    void updInfo(QString id);
    void cbChanged();
    void upd();
};

#endif // FORMRAB_H
