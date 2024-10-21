#include "formliter.h"
#include "ui_formliter.h"

FormLiter::FormLiter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormLiter)
{
    ui->setupUi(this);

    sqlExecutor = new ProgressExecutor(this);

    modelLiter = new DbTableModel("rab_liter", this);
    modelLiter->addColumn("id",tr("id"));
    modelLiter->addColumn("naim",tr("Наименование"));
    modelLiter->addColumn("id_ed",tr("Ед. изм."),Rels::instance()->relEd);
    modelLiter->addColumn("k_dopl",tr("Вред.,%"));
    modelLiter->addColumn("liter",tr("Лит."));
    modelLiter->addColumn("conn",tr("Вид связи"),Rels::instance()->relConn);
    modelLiter->addColumn("zon",tr("Участок"),Rels::instance()->relZon);
    modelLiter->addColumn("is_nrm",tr("Нормируемая"));
    modelLiter->setFilter("rab_liter.id<>0");

    ui->tableViewLiter->setModel(modelLiter);
    ui->tableViewLiter->setColumnHidden(0,true);
    ui->tableViewLiter->setColumnWidth(1,400);
    ui->tableViewLiter->setColumnWidth(2,80);
    ui->tableViewLiter->setColumnWidth(3,90);
    ui->tableViewLiter->setColumnWidth(4,70);
    ui->tableViewLiter->setColumnWidth(5,220);
    ui->tableViewLiter->setColumnWidth(6,200);
    ui->tableViewLiter->setColumnWidth(7,100);

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->pushButtonFullUpd,SIGNAL(clicked(bool)),this,SLOT(fullUpd()));
    connect(ui->radioButtonNam,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->radioButtonZon,SIGNAL(clicked(bool)),this,SLOT(upd()));

    upd();
}

FormLiter::~FormLiter()
{
    delete ui;
}

void FormLiter::upd()
{
    if (sender()==ui->pushButtonUpd){
        modelLiter->refreshRelsModel();
    }
    if (ui->radioButtonNam->isChecked()){
        modelLiter->setSort("rab_liter.naim");
    } else {
        modelLiter->setSort("rab_zon.nam, rab_liter.naim");
    }
    modelLiter->select();
}

void FormLiter::fullUpd()
{
    sqlExecutor->setQuery("select * from rx_nams()");
    sqlExecutor->start();
    modelLiter->refreshRelsModel();
    upd();
}
