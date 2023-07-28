#include "formcalcwage.h"
#include "ui_formcalcwage.h"

FormCalcWage::FormCalcWage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormCalcWage)
{
    ui->setupUi(this);
}

FormCalcWage::~FormCalcWage()
{
    delete ui;
}
