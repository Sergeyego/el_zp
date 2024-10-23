#include "allpressdialog.h"
#include "ui_allpressdialog.h"

AllPressDialog::AllPressDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AllPressDialog)
{
    ui->setupUi(this);
    ui->dateEdit->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));

    delegate = new QDoubleDelegate(this);

    modelMark = new ModelZon(tr("Выберите марку"),Rels::instance()->relMark->model(),false,this);
    ui->tableViewMark->setModel(modelMark);
    ui->tableViewMark->setColumnWidth(0,300);

    QAbstractItemModel *modelDiam = Rels::instance()->relDiam->model();
    ui->tableWidgetNorms->setRowCount(modelDiam->rowCount());
    ui->tableWidgetNorms->setColumnCount(3);
    ui->tableWidgetNorms->setHorizontalHeaderItem(1, new QTableWidgetItem(tr("Диаметр, мм")));
    ui->tableWidgetNorms->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Норма, т")));
    for (int i=0; i<modelDiam->rowCount(); i++){
        ui->tableWidgetNorms->setItem(i,0,new QTableWidgetItem());
        ui->tableWidgetNorms->setItem(i,1,new QTableWidgetItem());
        ui->tableWidgetNorms->setItem(i,2,new QTableWidgetItem());
        ui->tableWidgetNorms->item(i,0)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->tableWidgetNorms->item(i,1)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->tableWidgetNorms->item(i,0)->setData(Qt::EditRole,modelDiam->data(modelDiam->index(i,0)).toInt());
        ui->tableWidgetNorms->item(i,1)->setData(Qt::DisplayRole,modelDiam->data(modelDiam->index(i,1)).toString());
        ui->tableWidgetNorms->item(i,2)->setTextAlignment(Qt::AlignRight);
    }
    ui->tableWidgetNorms->setColumnHidden(0,true);
    ui->tableWidgetNorms->verticalHeader()->hide();
    ui->tableWidgetNorms->setItemDelegateForColumn(2,delegate);
    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(go()));
    connect(ui->pushButtonNew,SIGNAL(clicked()),this,SLOT(clearTbl()));
}

AllPressDialog::~AllPressDialog()
{
    delete ui;
}

void AllPressDialog::go()
{
    QStringList error;
    bool isEdt=false;
    QSet<int> elr=modelMark->getSel();
    for (int i=0; i<Rels::instance()->relPressFlt->model()->rowCount(); i++){
        int id_pr=Rels::instance()->relPressFlt->model()->data(Rels::instance()->relPressFlt->model()->index(i,0)).toInt();
        for (int id_el : elr){
            for (int k=0; k<ui->tableWidgetNorms->rowCount(); k++){
                double norm=ui->tableWidgetNorms->item(k,2)->data(Qt::EditRole).toDouble();
                if (norm!=0.0){
                    isEdt=true;
                    int id_dim=ui->tableWidgetNorms->item(k,0)->data(Qt::EditRole).toInt();
                    QSqlQuery query;
                    query.prepare("delete from norm_smen where id_el=:id_el and id_dim=:id_dim and id_pr=:id_pr and chas=:chas and dat_nrm=:dat_nrm");
                    query.bindValue(":id_el",id_el);
                    query.bindValue(":id_dim",id_dim);
                    query.bindValue(":id_pr",id_pr);
                    query.bindValue(":chas",ui->spinBox->value());
                    query.bindValue(":dat_nrm",ui->dateEdit->date());
                    query.exec();
                    query.clear();
                    query.prepare("insert into norm_smen (id_el, id_dim, id_pr, chas, norm, dat_nrm) values (:id_el, :id_dim, :id_pr, :chas, :norm, :dat_nrm)");
                    query.bindValue(":id_el",id_el);
                    query.bindValue(":id_dim",id_dim);
                    query.bindValue(":id_pr",id_pr);
                    query.bindValue(":chas",ui->spinBox->value());
                    query.bindValue(":norm",norm);
                    query.bindValue(":dat_nrm",ui->dateEdit->date());
                    if (!query.exec()){
                        error<<query.lastError().text();
                    }
                }
            }
        }
    }
    if (error.isEmpty()){
        QString mess= isEdt? tr("Все операции выполнены успешно.") : tr("Нет действий для выполнения.");
        QMessageBox::information(this,tr("Информация"),mess,QMessageBox::Ok);
    } else {
        QString str;
        for (int i=0; i<error.size(); ++i) {
            str+=error.at(i)+"\n";
        }
        QMessageBox *err = new  QMessageBox();
        err->setWindowTitle(tr("Ошибка"));
        err->setIcon(QMessageBox::Critical);
        err->setText(tr("Возникли ошибки во время выполнения операции."));
        err->setDetailedText(str);
        err->addButton(QMessageBox::Ok);
        err->exec();
        connect(err,SIGNAL(finished(int)),err,SLOT(deleteLater()));
    }
}

void AllPressDialog::clearTbl()
{
    modelMark->checkAll(false);
    for (int i=0; i<ui->tableWidgetNorms->rowCount(); i++){
        ui->tableWidgetNorms->item(i,2)->setData(Qt::EditRole,QVariant());
    }
}
