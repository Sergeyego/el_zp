#include "formrepmon.h"
#include "ui_formrepmon.h"

FormRepMon::FormRepMon(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormRepMon)
{
    ui->setupUi(this);
    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    ui->dateEditEnd->setDate(QDate::currentDate());

    modelRep = new TableModel(this);
    QStringList head;
    head<<tr("id")<<tr("ФИО работника")<<tr("Должность")<<tr("Премия, %");
    modelRep->setHeader(head);
    ui->tableViewRep->setModel(modelRep);
    ui->tableViewRep->setColumnHidden(0,true);

    executor = new ProgressExecutor(this);

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->pushButtonSav,SIGNAL(clicked(bool)),this,SLOT(save()));
    connect(executor,SIGNAL(finished()),this,SLOT(updFinished()));
}

FormRepMon::~FormRepMon()
{
    delete ui;
}

void FormRepMon::upd()
{
    QString query=QString("select c.id, c.snam, c.prof, c.prem "
            "from zrw_get_prem('%1','%2') as c").arg(ui->dateEditBeg->date().toString("yyyy-MM-dd")).arg(ui->dateEditEnd->date().toString("yyyy-MM-dd"));
    executor->setQuery(query);
    executor->start();
}

void FormRepMon::updFinished()
{
    QVector<QVector<QVariant>> data = executor->getData();
    modelRep->setModelData(data);
    ui->tableViewRep->resizeToContents();
}

void FormRepMon::save()
{
    QString tit=QString("Премия за месяц с %1 по %2").arg(ui->dateEditBeg->date().toString("dd.MM.yy")).arg(ui->dateEditEnd->date().toString("dd.MM.yy"));
    ui->tableViewRep->save(tit,1,true);
}
