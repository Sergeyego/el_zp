#include "formtarif.h"
#include "ui_formtarif.h"

FormTarif::FormTarif(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormTarif)
{
    ui->setupUi(this);

    sqlExecutor = new ProgressExecutor(this);

    Rels::instance()->relMarkType->refreshModel();
    ui->comboBoxType->setModel(Rels::instance()->relMarkType->model());
    ui->comboBoxType->setModelColumn(1);

    modelZon = new ModelZon(tr("Участок"),Rels::instance()->relZon->model(),false,this);
    ui->tableViewZon->setModel(modelZon);
    ui->tableViewZon->setColumnWidth(0,240);

    modelVid = new ModelZon(tr("Вид работ"),Rels::instance()->relLiter->model(),true,this);
    ui->tableViewVid->setModel(modelVid);
    ui->tableViewVid->setColumnWidth(0,240);

    modelMark = new ModelZon(tr("Марка"),Rels::instance()->relMark->model(),true,this);
    ui->tableViewMark->setModel(modelMark);
    ui->tableViewMark->setColumnWidth(0,240);

    modelDiam = new ModelZon(tr("Диаметр"),Rels::instance()->relDiam->model(),true,this);
    ui->tableViewDiam->setModel(modelDiam);
    ui->tableViewDiam->setColumnWidth(0,80);

    modelComp = new ModelZon(tr("Компонент"),Rels::instance()->relComp->model(),true,this);
    ui->tableViewComp->setModel(modelComp);
    ui->tableViewComp->setColumnWidth(0,240);

    modelJob = new ModelRo(this);
    ui->tableViewJob->setModel(modelJob);

    modelTarif = new DbTableModel("rab_pay",this);
    modelTarif->addColumn("lid",tr("lid"));
    modelTarif->addColumn("dat",tr("Дата"));
    modelTarif->addColumn("tarif",tr("Тариф, руб."));
    modelTarif->setSort("rab_pay.dat");
    modelTarif->setDecimals(2,2);
    modelTarif->setDefaultValue(1,QDate(QDate::currentDate().year(),QDate::currentDate().month(),1));

    ui->tableViewTarif->setModel(modelTarif);
    ui->tableViewTarif->setColumnHidden(0,true);
    ui->tableViewTarif->setColumnWidth(1,100);
    ui->tableViewTarif->setColumnWidth(2,100);

    connect(ui->tableViewZon->horizontalHeader(),SIGNAL(sectionClicked(int)),modelZon,SLOT(checkAll()));
    connect(ui->tableViewVid->horizontalHeader(),SIGNAL(sectionClicked(int)),modelVid,SLOT(checkAll()));
    connect(ui->tableViewMark->horizontalHeader(),SIGNAL(sectionClicked(int)),modelMark,SLOT(checkAll()));
    connect(ui->tableViewDiam->horizontalHeader(),SIGNAL(sectionClicked(int)),modelDiam,SLOT(checkAll()));
    connect(ui->tableViewComp->horizontalHeader(),SIGNAL(sectionClicked(int)),modelComp,SLOT(checkAll()));
    connect(modelZon,SIGNAL(supd()),this,SLOT(upd()));
    connect(modelVid,SIGNAL(supd()),this,SLOT(upd()));
    connect(modelMark,SIGNAL(supd()),this,SLOT(upd()));
    connect(modelDiam,SIGNAL(supd()),this,SLOT(upd()));
    connect(modelComp,SIGNAL(supd()),this,SLOT(upd()));
    connect(ui->tableViewJob->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updData(QModelIndex)));
    connect(sqlExecutor,SIGNAL(finished()),this,SLOT(upd()));
    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(updFull()));
    connect(ui->pushButtonSel,SIGNAL(clicked(bool)),this,SLOT(selType()));
    connect(ui->pushButtonGrp,SIGNAL(clicked(bool)),this,SLOT(setGrpTarif()));
}

FormTarif::~FormTarif()
{
    delete ui;
}

void FormTarif::upd()
{
    QString qu=QString("select rn.lid, substring(rn.fnam from coalesce(length(rl.liter),0)+1) as jnam from rab_nams rn "
                       "inner join rab_liter rl on rl.id = rn.id "
                       "inner join diam d on d.diam = rn.dim "
                       "where rn.lid>0 and rl.zon in %1 and rn.id_el in %2 and d.id in %3 and rn.id_comp in %4 and rl.id in %5 "
                       "order by jnam").arg(modelZon->getStr()).arg(modelMark->getStr()).arg(modelDiam->getStr()).arg(modelComp->getStr()).arg(modelVid->getStr());
    QSqlQuery query;
    query.prepare(qu);
    if (modelJob->execQuery(query)){
        modelJob->setHeaderData(1,Qt::Horizontal,tr("Наименование работ"));
        ui->tableViewJob->setColumnHidden(0,true);
        ui->tableViewJob->setColumnWidth(1,530);
        if (ui->tableViewJob->model()->rowCount()){
            ui->tableViewJob->selectRow(0);
        } else {
            modelTarif->setDefaultValue(0,0);
            modelTarif->setFilter("rab_pay.lid = 0");
            modelTarif->select();
        }
        ui->tableViewTarif->setEnabled(ui->tableViewJob->model()->rowCount());
    }
}

void FormTarif::updData(QModelIndex ind)
{
    QVariant lid=ui->tableViewJob->model()->data(ui->tableViewJob->model()->index(ind.row(),0),Qt::EditRole);
    modelTarif->setDefaultValue(0,lid);
    modelTarif->setFilter("rab_pay.lid = "+lid.toString());
    modelTarif->select();
}

void FormTarif::updFull()
{
    sqlExecutor->setQuery("select * from rx_nams()");
    sqlExecutor->start();
    Rels::instance()->relZon->refreshModel();
    Rels::instance()->relMark->refreshModel();
    Rels::instance()->relDiam->refreshModel();
    Rels::instance()->relComp->refreshModel();
    Rels::instance()->relMarkType->refreshModel();
}

void FormTarif::selType()
{
    int id_type = ui->comboBoxType->model()->data(ui->comboBoxType->model()->index(ui->comboBoxType->currentIndex(),0),Qt::EditRole).toInt();
    QSqlQuery query;
    query.prepare("select id, marka from elrtr where id_norm_type = :t order by marka");
    query.bindValue(":t",id_type);
    QSet<int> els;
    if (query.exec()){
        while (query.next()){
            els.insert(query.value(0).toInt());
        }
        modelMark->setSel(els);
    } else {
        QMessageBox::critical(this,tr("Ошибка"),query.lastError().text(),QMessageBox::Cancel);
    }
}

void FormTarif::setGrpTarif()
{
    TnDialog d;
    d.setWindowTitle(tr("Тариф для группы"));
    bool ok;
    if (d.exec()==QDialog::Accepted){
        if (d.getVal()>0){
            for (int i=0; i<modelJob->rowCount(); i++){
                QSqlQuery query;
                query.prepare("insert into rab_pay (lid, dat, tarif) values (:lid, :dat, :tarif)");
                query.bindValue(":lid",modelJob->data(modelJob->index(i,0),Qt::EditRole).toLongLong());
                query.bindValue(":dat",d.getDate());
                query.bindValue(":tarif",d.getVal());
                ok=query.exec();
                if (!ok){
                    QMessageBox::critical(this,"Error", query.lastError().text(), QMessageBox::Ok);
                    break;
                }
            }
        }
        upd();
    }
}
