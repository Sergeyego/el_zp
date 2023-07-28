#include "formcalcwage.h"
#include "ui_formcalcwage.h"

FormCalcWage::FormCalcWage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormCalcWage)
{
    ui->setupUi(this);
    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    ui->dateEditEnd->setDate(QDate::currentDate());

    connect(ui->pushButtonCalc,SIGNAL(clicked(bool)),this,SLOT(reCalc()));
}

FormCalcWage::~FormCalcWage()
{
    delete ui;
}

void FormCalcWage::reCalc()
{

}
