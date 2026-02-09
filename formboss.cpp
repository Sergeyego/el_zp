#include "formboss.h"
#include "ui_formboss.h"

FormBoss::FormBoss(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FormBoss)
{
    ui->setupUi(this);
    modelBoss = new DbTableModel("kamin_mains",this);
    modelBoss->addColumn("id_sub",tr("Подразделение"),Rels::instance()->relKaminSub);
    modelBoss->addColumn("id_empl",tr("Начальник"),Rels::instance()->relKaminEmp);
    modelBoss->addColumn("dat",tr("Дата принятия"));
    modelBoss->setSort("kamin_sub.nam, kamin_mains.dat");
    modelBoss->select();

    ui->tableViewBoss->setModel(modelBoss);
    ui->tableViewBoss->setColumnWidth(0,400);
    ui->tableViewBoss->setColumnWidth(1,400);
    ui->tableViewBoss->setColumnWidth(2,100);

    modelNorm = new DbTableModel("kamin_norm",this);
    modelNorm->addColumn("id_empl",tr("Подписант"),Rels::instance()->relKaminEmp);
    modelNorm->addColumn("dat",tr("Дата принятия"));
    modelNorm->setSort("kamin_norm.dat");
    modelNorm->select();

    ui->tableViewNorm->setModel(modelNorm);
    ui->tableViewNorm->setColumnWidth(0,400);
    ui->tableViewNorm->setColumnWidth(1,100);
}

FormBoss::~FormBoss()
{
    delete ui;
}
