#include "formpremmon.h"
#include "ui_formpremmon.h"

FormPremMon::FormPremMon(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormPremMon)
{
    ui->setupUi(this);

    if (!Rels::instance()->relZon->isInital()){
        Rels::instance()->relZon->refreshModel();
    }
    ui->listViewZon->setModel(Rels::instance()->relZon->model());
    ui->listViewZon->setModelColumn(1);
    modelPremNorm = new DbTableModel("rab_prem_zon",this);
    modelPremNorm->addColumn("id_zon",tr("id_zon"));
    modelPremNorm->addColumn("dat",tr("Дата"));
    modelPremNorm->addColumn("id_list",tr("Показатель"),Rels::instance()->relList);
    modelPremNorm->setSort("rab_prem_zon.dat");
    ui->tableViewNorm->setModel(modelPremNorm);
    ui->tableViewNorm->setColumnHidden(0,true);
    ui->tableViewNorm->setColumnWidth(1,100);
    ui->tableViewNorm->setColumnWidth(2,200);

    if (!Rels::instance()->relProf->isInital()){
        Rels::instance()->relProf->refreshModel();
    }
    ui->listViewProf->setModel(Rels::instance()->relProf->model());
    ui->listViewProf->setModelColumn(1);

    modelPremMon = new DbTableModel("rab_prem_mon");
    modelPremMon->addColumn("id_prof",tr("id_prof"));
    modelPremMon->addColumn("dat",tr("Дата"));
    modelPremMon->addColumn("id_razr",tr("Разряд"),Rels::instance()->relRazr);
    modelPremMon->addColumn("prem",tr("Премия, %"));
    modelPremMon->setSort("rab_prem_mon.dat");
    modelPremMon->setDefaultValue(1,QDate(QDate::currentDate().year(),QDate::currentDate().month(),1));

    ui->tableViewPrem->setModel(modelPremMon);
    ui->tableViewPrem->setColumnHidden(0,true);
    ui->tableViewPrem->setColumnWidth(1,100);
    ui->tableViewPrem->setColumnWidth(2,80);
    ui->tableViewPrem->setColumnWidth(3,80);

    modelList = new DbTableModel("rab_prem_list");
    modelList->addColumn("id",tr("id"));
    modelList->addColumn("nam",tr("Название"));
    modelList->setSort("rab_prem_list.id");
    ui->tableViewList->setModel(modelList);
    ui->tableViewList->setColumnHidden(0,true);
    ui->tableViewList->setColumnWidth(1,200);
    mapper = new DbMapper(ui->tableViewList,this);
    mapper->addMapping(ui->lineEditNam,1);
    ui->horizontalLayoutMap->insertWidget(0,mapper);
    mapper->addEmptyLock(ui->tableViewCont);

    modelListData = new DbTableModel("rab_prem_list_data",this);
    modelListData->addColumn("id_list",tr("id_list"));
    modelListData->addColumn("proc",tr("Процент выполнения"));
    modelListData->addColumn("norm",tr("Премия норма, %"));
    modelListData->addColumn("kach",tr("Премия качество, %"));
    modelListData->addColumn("mon",tr("Премия ежемес., %"));
    modelListData->setSort("rab_prem_list_data.proc");
    ui->tableViewCont->setModel(modelListData);
    ui->tableViewCont->setColumnHidden(0,true);
    ui->tableViewCont->setColumnWidth(1,150);
    ui->tableViewCont->setColumnWidth(2,150);
    ui->tableViewCont->setColumnWidth(3,150);
    ui->tableViewCont->setColumnWidth(4,150);

    connect(Rels::instance()->relZon->model(),SIGNAL(searchFinished(QString)),this,SLOT(updZonFinished()));
    connect(ui->listViewZon->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updZonCont(QModelIndex)));

    connect(Rels::instance()->relProf->model(),SIGNAL(searchFinished(QString)),this,SLOT(updProfFinished()));
    connect(ui->listViewProf->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updMonCont(QModelIndex)));

    connect(mapper,SIGNAL(currentIndexChanged(int)),this,SLOT(updListCont(int)));
    connect(modelList,SIGNAL(sigUpd()),Rels::instance()->relList,SLOT(refreshModel()));

    connect(ui->radioButtonNorm,SIGNAL(clicked(bool)),this,SLOT(setPageNorm()));
    connect(ui->radioButtonMon,SIGNAL(clicked(bool)),this,SLOT(setPageMon()));
    connect(ui->radioButtonList,SIGNAL(clicked(bool)),this,SLOT(setPageList()));

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(upd()));

    modelList->select();
}

FormPremMon::~FormPremMon()
{
    delete ui;
}

void FormPremMon::upd()
{
    modelList->select();
    Rels::instance()->relZon->refreshModel();
    Rels::instance()->relProf->refreshModel();
    Rels::instance()->relRazr->refreshModel();
    Rels::instance()->relList->refreshModel();
}

void FormPremMon::updZonFinished()
{
    if (ui->listViewZon->model()->rowCount()){
        ui->listViewZon->setCurrentIndex(ui->listViewZon->model()->index(0,1));
    }
}

void FormPremMon::updZonCont(QModelIndex ind)
{
    int id_zon = ui->listViewZon->model()->data(ui->listViewZon->model()->index(ind.row(),0),Qt::EditRole).toInt();
    modelPremNorm->setFilter("rab_prem_zon.id_zon = "+QString::number(id_zon));
    modelPremNorm->setDefaultValue(0,id_zon);
    modelPremNorm->select();
}

void FormPremMon::updProfFinished()
{
    if (ui->listViewProf->model()->rowCount()){
        ui->listViewProf->setCurrentIndex(ui->listViewProf->model()->index(0,1));
    }
}

void FormPremMon::updMonCont(QModelIndex ind)
{
    int id_prof = ui->listViewProf->model()->data(ui->listViewProf->model()->index(ind.row(),0),Qt::EditRole).toInt();
    modelPremMon->setFilter("rab_prem_mon.id_prof = "+QString::number(id_prof));
    modelPremMon->setDefaultValue(0,id_prof);
    modelPremMon->select();
}

void FormPremMon::updListCont(int ind)
{
    int id_list = mapper->modelData(ind,0).toInt();
    modelListData->setFilter("rab_prem_list_data.id_list = "+QString::number(id_list));
    modelListData->setDefaultValue(0,id_list);
    modelListData->select();
}

void FormPremMon::setPageNorm()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void FormPremMon::setPageMon()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void FormPremMon::setPageList()
{
    ui->stackedWidget->setCurrentIndex(2);
}
