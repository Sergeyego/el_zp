#include "tndialog.h"
#include "ui_tndialog.h"

TnDialog::TnDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TnDialog)
{
    ui->setupUi(this);
    ui->dateEdit->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));
}

TnDialog::~TnDialog()
{
    delete ui;
}

QDate TnDialog::getDate()
{
    return ui->dateEdit->date();
}

double TnDialog::getVal()
{
    return ui->lineEdit->text().toDouble();
}

