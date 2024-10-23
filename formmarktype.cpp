#include "formmarktype.h"
#include "ui_formmarktype.h"

FormMarkType::FormMarkType(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormMarkType)
{
    ui->setupUi(this);

    modelMarkType = new DbTableModel("elrtr",this);
    modelMarkType->addColumn("id",tr("id"));
    modelMarkType->addColumn("marka",tr("Марка"));
    modelMarkType->addColumn("id_norm_type",tr("Тип"),Rels::instance()->relMarkType);
    modelMarkType->setSort("elrtr.marka");
    modelMarkType->setFilter("elrtr.id <> 0");
    modelMarkType->setInsertable(false);
    modelMarkType->setColumnFlags(1,Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    modelMarkType->select();

    ui->tableViewMarkType->setModel(modelMarkType);
    ui->tableViewMarkType->setColumnHidden(0,true);
    ui->tableViewMarkType->setColumnWidth(1,200);
    ui->tableViewMarkType->setColumnWidth(2,150);

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),modelMarkType,SLOT(refreshRelsModel()));
    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),modelMarkType,SLOT(select()));
}

FormMarkType::~FormMarkType()
{
    delete ui;
}
