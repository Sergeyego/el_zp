#include "formrepmon.h"
#include "ui_formrepmon.h"

FormRepMon::FormRepMon(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormRepMon)
{
    ui->setupUi(this);

    modelRep = new TableModel(this);
    QStringList head;
    head<<tr("id")<<tr("ФИО работника")<<tr("Должность")<<tr("Премия, %");
    modelRep->setHeader(head);
    ui->tableViewRep->setModel(modelRep);
    ui->tableViewRep->setColumnHidden(0,true);

    connect(ui->pushButtonSav,SIGNAL(clicked(bool)),this,SLOT(save()));
}

FormRepMon::~FormRepMon()
{
    delete ui;
}

void FormRepMon::setModelMada(const QVector<QVector<QVariant> > &data)
{
    modelRep->setModelData(data);
    ui->tableViewRep->resizeToContents();
}

void FormRepMon::setTitle(QString title)
{
    this->setWindowTitle(title);
    ui->labelTit->setText(title);
}

void FormRepMon::save()
{
    ui->tableViewRep->save(ui->labelTit->text(),1,true);
}
