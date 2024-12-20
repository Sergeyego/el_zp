#include "formnormpress.h"
#include "ui_formnormpress.h"

FormNormPress::FormNormPress(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormNormPress)
{
    ui->setupUi(this);

    ui->comboBoxPress->setModel(Rels::instance()->relPressFlt->model());
    ui->comboBoxPress->setModelColumn(1);

    ui->listViewMark->setModel(Rels::instance()->relMark->model());
    ui->listViewMark->setModelColumn(1);

    modelNorm = new DbTableModel("norm_smen",this);
    modelNorm->addColumn("id_el","id_el");
    modelNorm->addColumn("id_pr","id_pr");
    modelNorm->addColumn("dat_nrm",tr("Дата"));
    modelNorm->addColumn("id_dim",tr("Диаметр"),Rels::instance()->relDiam);
    modelNorm->addColumn("norm",tr("Норма, т"));
    modelNorm->setSort("norm_smen.dat_nrm, diam.diam");
    modelNorm->setDecimals(4,3);
    modelNorm->setDefaultValue(2,QDate(QDate::currentDate().year(),QDate::currentDate().month(),1));
    ui->tableViewNorm->setModel(modelNorm);
    ui->tableViewNorm->setColumnHidden(0,true);
    ui->tableViewNorm->setColumnHidden(1,true);
    ui->tableViewNorm->setColumnWidth(2,80);
    ui->tableViewNorm->setColumnWidth(3,80);
    ui->tableViewNorm->setColumnWidth(4,80);

    connect(ui->listViewMark->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updCont(QModelIndex)));
    connect(ui->comboBoxPress,SIGNAL(currentIndexChanged(int)),this,SLOT(updPress()));
    connect(Rels::instance()->relMark->model(),SIGNAL(searchFinished(QString)),this,SLOT(updMarkFinished()));
    connect(ui->pushButtonAll,SIGNAL(clicked(bool)),this,SLOT(normAllPress()));
    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(upd()));

    if (!Rels::instance()->relMark->isInital()){
        Rels::instance()->relMark->refreshModel();
    } else {
        updMarkFinished();
    }
}

FormNormPress::~FormNormPress()
{
    delete ui;
}

void FormNormPress::updCont(QModelIndex ind)
{
    int id_el=ui->listViewMark->model()->data(ui->listViewMark->model()->index(ind.row(),0),Qt::EditRole).toInt();
    int id_pr=ui->comboBoxPress->model()->data(ui->comboBoxPress->model()->index(ui->comboBoxPress->currentIndex(),0),Qt::EditRole).toInt();
    modelNorm->setFilter("norm_smen.chas = 11 and norm_smen.id_pr = "+QString::number(id_pr)+" and norm_smen.id_el = "+QString::number(id_el));
    modelNorm->setDefaultValue(0,id_el);
    modelNorm->setDefaultValue(1,id_pr);
    modelNorm->select();
}

void FormNormPress::updPress()
{
    QModelIndex index=ui->listViewMark->model()->index(ui->listViewMark->currentIndex().row(),0);
    updCont(index);
}

void FormNormPress::updMarkFinished()
{
    if (ui->listViewMark->model()->rowCount()){
        ui->listViewMark->setCurrentIndex(ui->listViewMark->model()->index(0,1));
    }
}

void FormNormPress::normAllPress()
{
    AllPressDialog d;
    d.exec();
    updPress();
}

void FormNormPress::upd()
{
    modelNorm->refreshRelsModel();
    Rels::instance()->relPressFlt->refreshModel();
    Rels::instance()->relMark->refreshModel();
}
