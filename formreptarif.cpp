#include "formreptarif.h"
#include "ui_formreptarif.h"

FormRepTarif::FormRepTarif(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormRepTarif)
{
    ui->setupUi(this);
    ui->dateEdit->setDate(QDate::currentDate());
    modelZon = new ModelZon(tr("Участок"),Rels::instance()->relZon,this);
    modelZon->checkAll(true);
    ui->tableViewZon->setModel(modelZon);
    ui->tableViewZon->setColumnWidth(0,250);

    modelTarif = new ModelRo(this);
    ui->tableViewTarif->setModel(modelTarif);

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->pushButtonSave,SIGNAL(clicked(bool)),this,SLOT(save()));
    connect(ui->tableViewZon->horizontalHeader(),SIGNAL(sectionClicked(int)),modelZon,SLOT(checkAll()));
    connect(modelZon,SIGNAL(supd()),this,SLOT(upd()));
}

FormRepTarif::~FormRepTarif()
{
    delete ui;
}

void FormRepTarif::upd()
{
    if (sender()==ui->pushButtonUpd){
        Rels::instance()->relZon->refreshModel();
    }
    QSqlQuery query;
    query.prepare("select substring(rn.fnam from coalesce(char_length(rl.liter),0)+1), t.tarif, t.k_dpl "
                  "from rw_tarif(:d) as t "
                  "inner join rab_nams rn on rn.lid=t.lid "
                  "inner join rab_liter rl on rl.id=rn.id "
                  "inner join rab_zon rz on rz.id=rl.zon "
                  "where rl.zon in "+modelZon->getStr()+" "
                  "order by rz.nam");
    query.bindValue(":d",ui->dateEdit->date());
    if (modelTarif->execQuery(query)){
        modelTarif->setHeaderData(0,Qt::Horizontal,tr("Наименование работ"));
        modelTarif->setHeaderData(1,Qt::Horizontal,tr("Тариф, руб."));
        modelTarif->setHeaderData(2,Qt::Horizontal,tr("Вред., %"));
        ui->tableViewTarif->setColumnWidth(0,450);
        ui->tableViewTarif->setColumnWidth(1,100);
        ui->tableViewTarif->setColumnWidth(2,100);
    }
}

void FormRepTarif::save()
{
    QString title = QString("Тарифы на %1").arg(ui->dateEdit->date().toString("dd.MM.yyyy"));
    ui->tableViewTarif->save(title,2,true);
}
