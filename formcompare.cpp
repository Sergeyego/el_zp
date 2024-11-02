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
    connect(ui->pushButtonSave,SIGNAL(clicked(bool)),this,SLOT(save()));
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

void FormCompare::save()
{
    QString tit = tr("Сравнение данных упаковки");
    tit+=tr(" c ")+ui->dateEditBeg->date().toString("dd.MM.yyyy")+tr(" по ")+ui->dateEditEnd->date().toString("dd.MM.yyyy");
    ui->tableView->save(tit,-1,true);
}

ModelCompare::ModelCompare(QObject *parent) : TableModel(parent)
{
    QStringList head;
    head<<tr("Дата")<<tr("Партия")<<tr("Марка")<<tr("Диаметр")<<tr("З/П, т")<<tr("Пр-во, т")<<tr("Разница, т");
    setHeader(head);
    setDecimalForColumn(4,4);
    setDecimalForColumn(5,4);
    setDecimalForColumn(6,4);
}

QVariant ModelCompare::data(const QModelIndex &index, int role) const
{
    if (role==Qt::BackgroundRole){
        return abs(this->data(this->index(index.row(),this->columnCount()-1),Qt::EditRole).toDouble())<0.00001 ? QVariant(QColor(170,255,170)) : QVariant(QColor(255,170,170));
    }
    return TableModel::data(index,role);
}

void ModelCompare::refresh(QDate beg, QDate end, bool err)
{
    QVector<QVector<QVariant>> data;
    QString flt = err ? "where c.zp<>c.pr " : "";
    QSqlQuery query;
    query.prepare("select c.dat, p.n_s, e.marka, c.dim, c.zp, c.pr, c.pr-c.zp "
                  "from zrw_compare_pack(:d1,:d2) as c "
                  "inner join parti p on p.id=c.id_part "
                  "inner join elrtr e on e.id = c.id_el "+flt+
                  "order by c.dat, e.marka, c.dim, p.n_s");
    query.bindValue(":d1",beg);
    query.bindValue(":d2",end);
    QVector<QVariant> sum;
    if(query.exec()){
        sum.resize(query.record().count());
        sum[0]=tr("Итого");
        while (query.next()){
            QVector<QVariant> row;
            for (int i=0; i<query.record().count();i++){
                row.push_back(query.value(i));
                if (i>=4){
                    sum[i]=sum[i].toDouble()+query.value(i).toDouble();
                }
            }
            data.push_back(row);
        }
        data.push_back(sum);
    } else {
        QMessageBox::critical(nullptr,"Error",query.lastError().text(),QMessageBox::Cancel);
    }
    setModelData(data);
}
