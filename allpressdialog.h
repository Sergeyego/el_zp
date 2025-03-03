#ifndef ALLPRESSDIALOG_H
#define ALLPRESSDIALOG_H

#include <QDialog>
#include <QSqlQuery>
#include "rels.h"
#include <QTableWidgetItem>
#include <QListWidgetItem>
#include <QSqlQuery>
#include "qdoubledelegate.h"
#include "modelzon.h"

namespace Ui {
class AllPressDialog;
}

class AllPressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AllPressDialog(QWidget *parent = 0);
    ~AllPressDialog();

private:
    Ui::AllPressDialog *ui;
    QDoubleDelegate *delegate;
    ModelZon *modelMark;

private slots:
    void go();
    void clearTbl();
    void selType();
};

#endif // ALLPRESSDIALOG_H
