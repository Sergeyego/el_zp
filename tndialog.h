#ifndef TNDIALOG_H
#define TNDIALOG_H

#include <QDialog>
#include <QDate>

namespace Ui {
class TnDialog;
}

class TnDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TnDialog(QWidget *parent = 0);
    ~TnDialog();
    QDate getDate();
    double getVal();

private:
    Ui::TnDialog *ui;
};

#endif // TNDIALOG_H
