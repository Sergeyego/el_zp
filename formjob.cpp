#include "formjob.h"
#include "ui_formjob.h"

FormJob::FormJob(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormJob)
{
    ui->setupUi(this);
    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    ui->dateEditEnd->setDate(QDate::currentDate());

    modelZon = new ModelZon(tr("Участок (F6)"),Rels::instance()->relZon->model(),true,this);
    ui->tableViewZon->setModel(modelZon);
    ui->tableViewZon->setColumnWidth(0,220);

    modelJob = new ModelJob(this);
    ui->tableViewJob->setModel(modelJob);
    ui->tableViewJob->setColumnHidden(0,true);
    ui->tableViewJob->setColumnWidth(1,400);
    ui->tableViewJob->setColumnWidth(2,80);
    ui->tableViewJob->setColumnWidth(3,80);
    ui->tableViewJob->setColumnWidth(4,70);
    ui->tableViewJob->setColumnWidth(5,70);
    ui->tableViewJob->setColumnWidth(6,300);
    ui->tableViewJob->setColumnWidth(7,150);
    ui->tableViewJob->setColumnWidth(8,60);
    ui->tableViewJob->setColumnWidth(9,60);

    connect(ui->tableViewZon->horizontalHeader(),SIGNAL(sectionClicked(int)),modelZon,SLOT(checkAll()));
    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(modelZon,SIGNAL(supd()),this,SLOT(upd()));

    upd();
}

FormJob::~FormJob()
{
    delete ui;
}

void FormJob::upd()
{
    if (sender()==ui->pushButtonUpd){
        Rels::instance()->relZon->refreshModel();
    }
    modelJob->refresh(ui->dateEditBeg->date(),ui->dateEditEnd->date(),modelZon->getStr());
}

ModelJob::ModelJob(QWidget *parent) : DbTableModel("rab_job",parent)
{
    addColumn("id",tr("id"));
    addColumn("lid",tr("Наименование работы"),Rels::instance()->relJobNam);
    addColumn("dat",tr("Дата уч."));
    addColumn("datf",tr("Дата факт."));
    addColumn("kvo",tr("Объем"));
    addColumn("parti",tr("Партия"));
    addColumn("id_rb",tr("Бригадир"),Rels::instance()->relRab);
    addColumn("id_press",tr("Пресс"),Rels::instance()->relPress);
    addColumn("chas_sm",tr("ч/см"));
    addColumn("extr_time",tr("Св.ур,%"));

    setSuffix("left join rab_liter on rab_liter.id = rab_nams.id");
    setDecimals(4,3);
}

void ModelJob::refresh(QDate beg, QDate end, QString zon, int id_rb)
{
    QString flt = QString("rab_job.dat between '%1' and '%2' and rab_liter.zon in %3").arg(beg.toString("yyyy-MM-dd")).arg(end.toString("yyyy-MM-dd")).arg(zon);
    setFilter(flt);
    setSort("rab_job.dat");
    setDefaultValue(2,end);
    setDefaultValue(3,end);
    select();
}

ModelShare::ModelShare(QWidget *parent): DbTableModel("rab_share",parent)
{
    addColumn("id",tr("id"));
    addColumn("id_job",tr("id_job"));
    addColumn("id_rab",tr("Работник"),Rels::instance()->relRab);
    addColumn("koef_prem_kvo",tr("к.прем"));
    addColumn("kvo",tr("Кол-во"));
    addColumn("s_koef",tr("Коэфф"));
    addColumn("prem",tr("Св.ур,%"));
    setSort("rab_share.id");
}

void ModelShare::refresh(int id_job)
{
    setDefaultValue(1,id_job);
    setFilter("rab_share.id_job = "+QString::number(id_job));
    select();
}
