#ifndef DIALOGSPLIT_H
#define DIALOGSPLIT_H

#include <QDialog>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>

namespace Ui {
class DialogSplit;
}

class DialogSplit : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSplit(int id_j, QWidget *parent = nullptr);
    ~DialogSplit();

private:
    int id_job;
    Ui::DialogSplit *ui;
    void insNew();
    void updOld();

private slots:
    void calcUpk();
    void accept();
};

#endif // DIALOGSPLIT_H
