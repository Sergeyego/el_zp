#include "formnormpack.h"
#include "ui_formnormpack.h"

FormNormPack::FormNormPack(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormNormPack)
{
    ui->setupUi(this);

    ui->listViewMark->setModel(Rels::instance()->relMark->model());
    ui->listViewMark->setModelColumn(1);

    modelNorm = new DbTableModel("upack_norms",this);
    modelNorm->addColumn("id_el","id_el");
    modelNorm->addColumn("dat_nrm",tr("Дата"));
    modelNorm->addColumn("id_dim",tr("Диаметр"),Rels::instance()->relDiam);
    modelNorm->addColumn("norm",tr("Норма, кг"));
    modelNorm->setSort("upack_norms.dat_nrm, diam.diam");
    modelNorm->setDecimals(3,3);
    modelNorm->setDefaultValue(1,QDate(QDate::currentDate().year(),QDate::currentDate().month(),1));

    ui->tableViewNorm->setModel(modelNorm);
    ui->tableViewNorm->setColumnHidden(0,true);
    ui->tableViewNorm->setColumnWidth(1,80);
    ui->tableViewNorm->setColumnWidth(2,80);
    ui->tableViewNorm->setColumnWidth(3,80);

    connect(ui->listViewMark->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updCont(QModelIndex)));
    connect(ui->pushButtonAll,SIGNAL(clicked(bool)),this,SLOT(normAllMark()));
    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(Rels::instance()->relMark->model(),SIGNAL(searchFinished(QString)),this,SLOT(updMarkFinished()));

    if (!Rels::instance()->relMark->isInital()){
        Rels::instance()->relMark->refreshModel();
    } else {
        updMarkFinished();
    }
}

FormNormPack::~FormNormPack()
{
    delete ui;
}

void FormNormPack::updCont(QModelIndex ind)
{
    int id_el=ui->listViewMark->model()->data(ui->listViewMark->model()->index(ind.row(),0),Qt::EditRole).toInt();
    modelNorm->setFilter("upack_norms.id_el = "+QString::number(id_el));
    modelNorm->setDefaultValue(0,id_el);
    modelNorm->select();
}

void FormNormPack::updMarkFinished()
{
    if (ui->listViewMark->model()->rowCount()){
        ui->listViewMark->setCurrentIndex(ui->listViewMark->model()->index(0,1));
    }
}

void FormNormPack::normAllMark()
{
    PackNormDialog d;
    d.exec();
    modelNorm->select();
}

void FormNormPack::upd()
{
    modelNorm->refreshRelsModel();
    Rels::instance()->relMark->refreshModel();
}
