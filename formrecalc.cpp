#include "formrecalc.h"
#include "ui_formrecalc.h"

FormRecalc::FormRecalc(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormRecalc)
{
    ui->setupUi(this);
    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    ui->dateEditEnd->setDate(QDate::currentDate());
    modelVip = new ModelVip(this);
    ui->tableView->setModel(modelVip);
    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->pushButtonRecalc,SIGNAL(clicked(bool)),this,SLOT(recalc()));
    connect(modelVip,SIGNAL(sigSum(QString)),ui->labelSum,SLOT(setText(QString)));
}

FormRecalc::~FormRecalc()
{
    delete ui;
}

void FormRecalc::upd()
{
    modelVip->refresh(ui->dateEditBeg->date(),ui->dateEditEnd->date());
    if (modelVip->columnCount()){
        ui->tableView->setColumnHidden(0,true);
        ui->tableView->resizeToContents();
    }
}

void FormRecalc::recalc()
{
    QDate beg=ui->dateEditBeg->date();
    QDate end=ui->dateEditEnd->date();
    QString err;
    int n = QMessageBox::warning(this,tr("Внимание"),tr("Скорректировать выпуск элестродов по упаковке c ")+beg.toString("dd.MM.yyyy")+tr("г. по ")+end.toString("dd.MM.yyyy")+tr("г.?"),QMessageBox::Yes,QMessageBox::No);
    if (n==QMessageBox::Yes){
        QProgressDialog* pprd = new QProgressDialog(tr("Идет пересчет..."),"", 0, modelVip->rowCount(), this);
        pprd->setCancelButton(0);
        pprd->setMinimumDuration(0);
        pprd->setWindowTitle(tr("Пожалуйста, подождите"));
        for (int i=0; i<modelVip->rowCount();i++){
            QCoreApplication::processEvents();
            pprd->setValue(i);
            int id_part=modelVip->data(modelVip->index(i,0),Qt::EditRole).toUInt();
            double vip=modelVip->data(modelVip->index(i,6),Qt::EditRole).toDouble();
            double up=modelVip->data(modelVip->index(i,7),Qt::EditRole).toDouble();
            if (vip>0){
                QSqlQuery query;
                query.prepare("update rab_job set kvo = kvo * :k "
                              "where id in( "
                              "select j.id "
                              "from rab_job j "
                              "inner join rab_nams n on n.lid=j.lid "
                              "inner join rab_liter l on l.id=n.id "
                              "inner join rab_rab r on j.id_rb=r.id "
                              "where l.grp = 21 "
                              "and j.dat between :d1 and :d2 "
                              "and j.id_part = :id_part )");
                query.bindValue(":k",up/vip);
                query.bindValue(":d1",beg);
                query.bindValue(":d2",end);
                query.bindValue(":id_part",id_part);
                if (!query.exec()){
                    err=query.lastError().text();
                    break;
                }
            }
        }
        upd();
        delete pprd;
        if (!err.isEmpty()){
            QMessageBox::critical(this,tr("Ошибка"),err,QMessageBox::Cancel);
        }
    }
}

ModelVip::ModelVip(QObject *parent) : ModelRo(parent)
{
    setDecimalForColumn(4,1);
    setDecimalForColumn(5,4);
    setDecimalForColumn(6,4);
    setDecimalForColumn(7,4);
    setDecimalForColumn(8,2);
}

QVariant ModelVip::data(const QModelIndex &item, int role) const
{
    if (role==Qt::BackgroundRole){
        double dt = this->data(this->index(item.row(),8),Qt::EditRole).toDouble();
        return (dt<20) ? QVariant(QColor(170,255,170)) : QVariant(QColor(255,170,170));
    }
    return ModelRo::data(item,role);
}

void ModelVip::refresh(QDate beg, QDate end)
{
    QSqlQuery query;
    query.prepare("select p.id, p.n_s, pr.snam, e.marka, p.diam, pr.oldfact, pr.vip, up.upack, (abs(up.upack-pr.vip)*100/up.upack)::numeric(18,2) "
                  "from parti p "
                  "inner join elrtr e on e.id = p.id_el "
                  "inner join "
                  "(select j.id_part as id_part, r.snam as snam, sum(j.kvo) as vip, sum(j.kvo_fact) as oldfact "
                  "from rab_job j "
                  "inner join rab_nams n on n.lid=j.lid "
                  "inner join rab_liter l on l.id=n.id "
                  "inner join rab_rab r on j.id_rb=r.id "
                  "where l.grp=21 "
                  "and dat between :d1 and :d2 "
                  "group by j.id_part, r.snam "
                  ") as pr on pr.id_part=p.id "
                  "inner join "
                  "(select j.id_part as id_part, sum(j.kvo) as upack "
                  "from rab_job j "
                  "inner join rab_nams n on n.lid=j.lid "
                  "inner join rab_liter l on l.id=n.id "
                  "where l.grp in (61,62) "
                  "and dat between :d1 and :d2 "
                  "group by j.id_part "
                  ") as up on up.id_part=p.id "
                  "order by p.n_s");
    query.bindValue(":d1",beg);
    query.bindValue(":d2",end);
    if (execQuery(query)){
        setHeaderData(1,Qt::Horizontal,tr("Партия"));
        setHeaderData(2,Qt::Horizontal,tr("Прессовщик"));
        setHeaderData(3,Qt::Horizontal,tr("Марка"));
        setHeaderData(4,Qt::Horizontal,tr("Диам."));
        setHeaderData(5,Qt::Horizontal,tr("От мастера, т"));
        setHeaderData(6,Qt::Horizontal,tr("Выпуск, т"));
        setHeaderData(7,Qt::Horizontal,tr("Упаковка, т"));
        setHeaderData(8,Qt::Horizontal,tr("Отклонение, %"));
    }
    calcSum();
}

void ModelVip::calcSum()
{
    double sumVip=0;
    double sumUp=0;
    for (int i=0; i<rowCount(); i++){
        sumVip+=data(index(i,6),Qt::EditRole).toDouble();
        sumUp+=data(index(i,7),Qt::EditRole).toDouble();
    }
    QString s = QString("Выпуск итого: %1 т; Упаковка итого: %2 т").arg(QLocale().toString(sumVip,'f',4)).arg(QLocale().toString(sumUp,'f',4));
    emit sigSum(s);
}
