#include "formreptarif.h"
#include "ui_formreptarif.h"

FormRepTarif::FormRepTarif(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormRepTarif)
{
    ui->setupUi(this);
    ui->dateEdit->setDate(QDate::currentDate());
    modelZon = new ModelZon(tr("Участок"),Rels::instance()->relZon->model(),true,this);
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
    query.prepare("select rz.nam, rl.naim, "
                  "case when rn.id_el<>0 then e.marka else '' end || "
                  "case when rn.id_el<>0 and rn.dim<>0 then ' ' else '' end || "
                  "case when rn.dim<>0 then 'ф'||cast(rn.dim as varchar(3)) else '' end || "
                  "case when rn.id_comp<>0 then m.nam else '' end || "
                  "case when rn.id_trf<>0 then rr.num||' разряд' else '' end as snam, "
                  "t.tarif-(t.tarif*t.k_dpl/100.0), t.k_dpl, (t.tarif*t.k_dpl/100.0), t.tarif "
                  "from rw_tarif(:d) as t "
                  "inner join rab_nams rn on rn.lid=t.lid "
                  "inner join rab_liter rl on rl.id=rn.id "
                  "inner join rab_zon rz on rz.id=rl.zon "
                  "left join elrtr e on e.id=rn.id_el "
                  "left join matr m on m.id=rn.id_comp "
                  "left join rab_razr rr on rr.id=rn.id_trf "
                  "where t.tarif>0.0 and rl.zon in "+modelZon->getStr()+" "
                  "order by rz.nam, t.tarif, rl.naim, e.marka, rn.dim, m.nam, rr.num");
    query.bindValue(":d",ui->dateEdit->date());
    if (modelTarif->execQuery(query)){
        modelTarif->setHeaderData(0,Qt::Horizontal,tr("Участок"));
        modelTarif->setHeaderData(1,Qt::Horizontal,tr("Наименование работ"));
        modelTarif->setHeaderData(2,Qt::Horizontal,tr("Марки, разряды"));
        modelTarif->setHeaderData(3,Qt::Horizontal,tr("Тариф, руб."));
        modelTarif->setHeaderData(4,Qt::Horizontal,tr("Вред., %"));
        modelTarif->setHeaderData(5,Qt::Horizontal,tr("Вред., руб"));
        modelTarif->setHeaderData(6,Qt::Horizontal,tr("Тариф с \nвред., руб"));
        ui->tableViewTarif->setColumnWidth(0,200);
        ui->tableViewTarif->setColumnWidth(1,450);
        ui->tableViewTarif->setColumnWidth(2,200);
        ui->tableViewTarif->setColumnWidth(3,100);
        ui->tableViewTarif->setColumnWidth(4,100);
        ui->tableViewTarif->setColumnWidth(5,100);
        ui->tableViewTarif->setColumnWidth(6,100);
    }
}

void FormRepTarif::save()
{
    QString title = QString("Тарифы на %1").arg(ui->dateEdit->date().toString("dd.MM.yyyy"));
    ui->tableViewTarif->save(title,2,true);
}
