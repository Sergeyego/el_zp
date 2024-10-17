#include "formrab.h"
#include "ui_formrab.h"

FormRab::FormRab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormRab)
{
    ui->setupUi(this);

    loadSettings();

    pprd = new ProgressReportDialog();

    modelInfo = new ModelRo(this);
    ui->tableViewInf->setModel(modelInfo);

    modelRab = new DbTableModel("rab_rab",this);
    modelRab->addColumn("id",tr("id"));
    modelRab->addColumn("fam",tr("Фамилия"));
    modelRab->addColumn("nam",tr("Имя"));
    modelRab->addColumn("otc",tr("Отчество"));
    modelRab->addColumn("snam",tr("ФИО"));
    modelRab->addColumn("id_kamin",tr("key"),Rels::instance()->relKaminEmp);
    modelRab->setSort("rab_rab.fam, rab_rab.nam, rab_rab.otc");

    ui->tableViewRab->setModel(modelRab);
    ui->tableViewRab->setColumnHidden(0,true);
    ui->tableViewRab->setColumnWidth(1,150);
    ui->tableViewRab->setColumnWidth(2,150);
    ui->tableViewRab->setColumnWidth(3,150);
    for (int i=4; i<ui->tableViewRab->model()->columnCount();i++){
        ui->tableViewRab->setColumnHidden(i,true);
    }

    mapper = new DbMapper(ui->tableViewRab,this);
    mapper->addMapping(ui->lineEditLastNam,1);
    mapper->addMapping(ui->lineEditFirstNam,2);
    mapper->addMapping(ui->lineEditMiddleNam,3);
    mapper->addMapping(ui->lineEditFIO,4);
    mapper->addMapping(ui->comboBoxKey,5);
    mapper->setDefaultFocus(5);
    mapper->addUnLock(ui->pushButtonIns);

    ui->horizontalLayoutMap->insertWidget(0,mapper);

    connect(ui->pushButtonIns,SIGNAL(clicked(bool)),this,SLOT(ins()));
    connect(ui->comboBoxKey,SIGNAL(currentIndexChanged(int)),this,SLOT(cbChanged()));
    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(upd()));

    modelRab->select();
}

FormRab::~FormRab()
{
    saveSettings();
    delete pprd;
    delete ui;
}

void FormRab::loadSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    ui->splitter->restoreState(settings.value("rab_splitter_width").toByteArray());
}

void FormRab::saveSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    settings.setValue("rab_splitter_width",ui->splitter->saveState());
}

void FormRab::ins()
{
    QString id=ui->comboBoxKey->getCurrentData().val.toString();
    QSqlQuery query;
    query.prepare("select ke.surname, ke.first_name, ke.middle_name, ke.snam "
                  "from kamin_empl ke where ke.id = :id");
    query.bindValue(":id",id);
    if (query.exec()){
        if (query.next()){
            ui->lineEditLastNam->setText(query.value(0).toString());
            ui->lineEditFirstNam->setText(query.value(1).toString());
            ui->lineEditMiddleNam->setText(query.value(2).toString());
            ui->lineEditFIO->setText(query.value(3).toString());
        }
    } else {
        QMessageBox::critical(this,tr("Ошибка"),query.lastError().text(),QMessageBox::Cancel);
    }
}

void FormRab::updInfo(QString id)
{
    QSqlQuery query;
    query.prepare("select ki.dat, ki.typ, ki.tabel, kj.nam, ks2.nam, ks.nam "
                  "from kamin_inf ki "
                  "left join kamin_job kj on kj.id= ki.id_job "
                  "left join kamin_sub ks2 on ks2.id = ki.id_sub "
                  "left join kamin_stat ks on ks.id = ki.id_stat "
                  "where ki.id_empl = :id "
                  "order by ki.dat ");
    query.bindValue(":id",id);
    if (modelInfo->execQuery(query)){
        modelInfo->setHeaderData(0,Qt::Horizontal,tr("Дата"));
        modelInfo->setHeaderData(1,Qt::Horizontal,tr("Операция"));
        modelInfo->setHeaderData(2,Qt::Horizontal,tr("Таб.№"));
        modelInfo->setHeaderData(3,Qt::Horizontal,tr("Должность"));
        modelInfo->setHeaderData(4,Qt::Horizontal,tr("Структурное подразделение"));
        modelInfo->setHeaderData(5,Qt::Horizontal,tr("Вид работы"));
    }
    ui->tableViewInf->resizeToContents();
}

void FormRab::cbChanged()
{
    updInfo(ui->comboBoxKey->getCurrentData().val.toString());
}

void FormRab::upd()
{
    int id_rab=mapper->modelData(mapper->currentIndex(),0).toInt();
    pprd->show();
    QByteArray data;
    if (HttpSyncManager::sendGet("/rab/sync",data)){
        modelRab->refreshRelsModel();
        modelRab->select();
    }
    pprd->hide();
    if (id_rab>0){
        for (int i=0; i<ui->tableViewRab->model()->rowCount();i++){
            int id=ui->tableViewRab->model()->data(ui->tableViewRab->model()->index(i,0),Qt::EditRole).toInt();
            if (id==id_rab){
                mapper->setCurrentViewRow(i);
                break;
            }
        }
    }
}
