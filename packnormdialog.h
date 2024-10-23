#ifndef PACKNORMDIALOG_H
#define PACKNORMDIALOG_H

#include <QDialog>
#include <QSqlQuery>
#include "rels.h"
#include <QTableWidgetItem>
#include <QListWidgetItem>
#include <QSqlQuery>
#include "qdoubledelegate.h"
#include "modelzon.h"

namespace Ui {
class PackNormDialog;
}

class PackNormDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PackNormDialog(QWidget *parent = 0);
    ~PackNormDialog();

private:
    Ui::PackNormDialog *ui;
    QDoubleDelegate *delegate;
    ModelZon *modelMark;

private slots:
    void go();
    void clearTbl();
    void selType();
};

#endif // PACKNORMDIALOG_H
