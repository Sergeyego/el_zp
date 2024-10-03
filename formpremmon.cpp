#include "formpremmon.h"
#include "ui_formpremmon.h"

FormPremMon::FormPremMon(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormPremMon)
{
    ui->setupUi(this);
}

FormPremMon::~FormPremMon()
{
    delete ui;
}
