#include "dialogtabel.h"
#include "ui_dialogtabel.h"

DialogTabel::DialogTabel(QAbstractItemModel *model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogTabel)
{
    ui->setupUi(this);
    modelRab = new ModelZon(tr("Выберите рабочих"),model,true,this);
    modelRab->setViewColumn(5);
    ui->tableView->setModel(modelRab);
    ui->tableView->setColumnWidth(0,500);
    connect(ui->tableView->horizontalHeader(),SIGNAL(sectionClicked(int)),modelRab,SLOT(checkAll()));
}

DialogTabel::~DialogTabel()
{
    delete ui;
}

QSet<int> DialogTabel::getSel()
{
    return modelRab->getSel();
}
