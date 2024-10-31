#include "formcompare.h"
#include "ui_formcompare.h"

FormCompare::FormCompare(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormCompare)
{
    ui->setupUi(this);
    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    ui->dateEditEnd->setDate(QDate::currentDate());

    modelCompare = new ModelCompare(this);
    ui->tableView->setModel(modelCompare);

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->checkBoxErr,SIGNAL(clicked(bool)),this,SLOT(upd()));
}

FormCompare::~FormCompare()
{
    delete ui;
}

void FormCompare::upd()
{
    modelCompare->refresh(ui->dateEditBeg->date(),ui->dateEditEnd->date(),ui->checkBoxErr->isChecked());
    ui->tableView->resizeToContents();
}

ModelCompare::ModelCompare(QWidget *parent) : TableModel(parent)
{
    QStringList head;
    head<<tr("Дата")<<tr("Партия")<<tr("Марка")<<tr("Диаметр")<<tr("Зарплата")<<tr("Производство");
    setHeader(head);
}

void ModelCompare::refresh(QDate beg, QDate end, bool err)
{
    QString flt = err ? "where c.zp<>c.pr " : "";
    QSqlQuery query;
    query.prepare("select c.dat, p.n_s, e.marka, c.dim, c.zp, c.pr "
                  "from zrw_compare_pack(:d1,:d2) as c "
                  "inner join parti p on p.id=c.id_part "
                  "inner join elrtr e on e.id = c.id_el "+flt+
                  "order by c.dat, e.marka, c.dim, p.n_s");
    query.bindValue(":d1",beg);
    query.bindValue(":d2",end);
}
