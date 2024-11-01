#include "formjob.h"
#include "ui_formjob.h"

FormJob::FormJob(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormJob)
{
    ui->setupUi(this);
    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    ui->dateEditEnd->setDate(QDate::currentDate());

    updTempTables();

    ui->comboBoxRab->setModel(Rels::instance()->relRab->model());

    modelZon = new ModelZon(tr("Участок (F6)"),Rels::instance()->relZon->model(),true,this);
    ui->tableViewZon->setModel(modelZon);
    ui->tableViewZon->setColumnWidth(0,220);

    modelJob = new ModelJob(this);
    ui->tableViewJob->setModel(modelJob);
    ui->tableViewJob->setColumnHidden(0,true);
    ui->tableViewJob->setColumnWidth(1,400);
    ui->tableViewJob->setColumnWidth(2,80);
    ui->tableViewJob->setColumnWidth(3,80);
    ui->tableViewJob->setColumnWidth(4,80);
    ui->tableViewJob->setColumnWidth(5,300);
    ui->tableViewJob->setColumnWidth(6,300);
    ui->tableViewJob->setColumnWidth(7,140);
    ui->tableViewJob->setColumnWidth(8,50);
    ui->tableViewJob->setColumnWidth(9,60);

    modelShare = new ModelShare(this);
    ui->tableViewShare->setModel(modelShare);
    ui->tableViewShare->setColumnHidden(0,true);
    ui->tableViewShare->setColumnHidden(1,true);
    ui->tableViewShare->setColumnWidth(2,300);
    ui->tableViewShare->setColumnWidth(3,80);
    ui->tableViewShare->setColumnWidth(4,80);
    ui->tableViewShare->setColumnWidth(5,80);
    ui->tableViewShare->setColumnWidth(6,80);

    connect(ui->tableViewZon->horizontalHeader(),SIGNAL(sectionClicked(int)),modelZon,SLOT(checkAll()));
    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(modelZon,SIGNAL(supd()),this,SLOT(upd()));
    connect(ui->tableViewJob->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updShare(QModelIndex)));
    connect(ui->checkBoxZero,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->checkBoxRab,SIGNAL(clicked(bool)),ui->comboBoxRab,SLOT(setEnabled(bool)));
    connect(ui->checkBoxRab,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->comboBoxRab,SIGNAL(currentIndexChanged(int)),this,SLOT(upd()));
    connect(ui->lineEditPart,SIGNAL(textChanged(QString)),this,SLOT(upd()));
    connect(ui->toolButtonPart,SIGNAL(clicked(bool)),ui->lineEditPart,SLOT(clear()));

    connect(ui->tableViewJob->horizontalHeader(),SIGNAL(sectionClicked(int)),this,SLOT(chkRab(int)));
    connect(ui->toolButtonUp,SIGNAL(clicked(bool)),this,SLOT(upJob()));
    connect(ui->toolButtonDown,SIGNAL(clicked(bool)),this,SLOT(downJob()));
    connect(ui->pushButtonCopy,SIGNAL(clicked(bool)),modelShare,SLOT(copy()));
    connect(ui->pushButtonPaste,SIGNAL(clicked(bool)),modelShare,SLOT(paste()));
    connect(ui->pushButtonOst,SIGNAL(clicked(bool)),this,SLOT(split()));

    upd();
}

FormJob::~FormJob()
{
    delete ui;
}

bool FormJob::updTempTables()
{
    QSqlQuery query;
    query.prepare("select * from rx_lists(:d)");
    query.bindValue(":d",ui->dateEditBeg->date());
    bool ok=query.exec();
    if (!ok){
        QMessageBox::critical(this,tr("Ошибка"),query.lastError().text(),QMessageBox::Cancel);
    }
    return ok;
}

void FormJob::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()){
    case Qt::Key_F1:
        if (e->modifiers()==Qt::ShiftModifier){
            modelShare->copy();
        }
        break;
    case Qt::Key_F2:
        if (e->modifiers()==Qt::ShiftModifier){
            modelShare->paste();
        }
        break;
    case Qt::Key_F4:
        if (e->modifiers()==Qt::ShiftModifier){
            split();
        } else {
            modelShare->submit();
            ui->tableViewJob->setFocus();
        }
        break;
    case Qt::Key_F5:
        if ((modelJob->isAdd() && modelJob->isEdt()) || modelJob->isEdt()){
            QModelIndex ind=ui->tableViewJob->currentIndex();
            modelJob->submit();
            updShare(ind);
        }
        if (modelJob->isAdd()){
            modelJob->submit();
        }
        ui->tableViewShare->setFocus();
        break;
    case Qt::Key_F6:
        ui->tableViewZon->setFocus();
        break;
    case Qt::Key_F7:
        upJob();
        break;
    case Qt::Key_F8:
        downJob();
        break;
    case Qt::Key_F12:
        if ((modelJob->isAdd() && modelJob->isEdt()) || modelJob->isEdt()){
            QModelIndex ind=ui->tableViewJob->currentIndex();
            modelJob->submit();
            updShare(ind);
        }
        if (!modelJob->isAdd()){
            modelShare->insShare();
        }
        break;
    }
    QWidget::keyPressEvent(e);
}

void FormJob::upd()
{
    if (sender()==ui->pushButtonUpd){
        Rels::instance()->relZon->refreshModel();
        updTempTables();
        ui->comboBoxRab->blockSignals(true);
        modelJob->refreshRelsModel();
        ui->comboBoxRab->blockSignals(false);
    }
    int id_rab = ui->checkBoxRab->isChecked()? ui->comboBoxRab->getCurrentData().val.toInt() : -1;
    modelJob->refresh(ui->dateEditBeg->date(),ui->dateEditEnd->date(),modelZon->getStr(),id_rab,ui->checkBoxZero->isChecked(),ui->lineEditPart->text());
    if (ui->tableViewJob->model()->rowCount()){
        ui->tableViewJob->setCurrentIndex(ui->tableViewJob->model()->index(ui->tableViewJob->model()->rowCount()-1,1));
        ui->tableViewJob->scrollToBottom();
    }
    if (sender()!=ui->lineEditPart){
        ui->tableViewJob->setFocus();
    }
}

void FormJob::updShare(QModelIndex ind)
{
    int id_job=ui->tableViewJob->model()->data(ui->tableViewJob->model()->index(ind.row(),0),Qt::EditRole).toInt();
    modelShare->refresh(id_job);
}

void FormJob::chkRab(int row)
{
    if (row==6){
        modelJob->submit();
        int id_brig = modelJob->getIdBrig()>0 ? -1 : ui->tableViewJob->model()->data(ui->tableViewJob->model()->index(ui->tableViewJob->currentIndex().row(),6),Qt::EditRole).toInt();
        modelJob->setIdBrig(id_brig);
        if (id_brig>0 && ui->checkBoxRab->isChecked()){
            ui->checkBoxRab->setChecked(false);
            ui->checkBoxRab->clicked();
        } else {
            upd();
        }
    }
}

void FormJob::upJob()
{
    QModelIndex curInd = ui->tableViewJob->currentIndex();
    if (curInd.row()>0){
        //ui->tableViewJob->selectRow(curInd.row()-1);
        ui->tableViewJob->setCurrentIndex(ui->tableViewJob->model()->index(curInd.row()-1,1));
        if (ui->tableViewShare->model()->rowCount()){
            ui->tableViewShare->setCurrentIndex(ui->tableViewShare->model()->index(0,2));
        }
        ui->tableViewShare->setFocus();
    }
}

void FormJob::downJob()
{
    QModelIndex curInd = ui->tableViewJob->currentIndex();
    if (curInd.row()<ui->tableViewJob->model()->rowCount()-1){
        //ui->tableViewJob->selectRow(curInd.row()+1);
        ui->tableViewJob->setCurrentIndex(ui->tableViewJob->model()->index(curInd.row()+1,1));
        if (ui->tableViewShare->model()->rowCount()){
            ui->tableViewShare->setCurrentIndex(ui->tableViewShare->model()->index(0,2));
        }
        ui->tableViewShare->setFocus();
    }
}

void FormJob::split()
{
    int id_job=ui->tableViewJob->model()->data(ui->tableViewJob->model()->index(ui->tableViewJob->currentIndex().row(),0),Qt::EditRole).toInt();
    if (id_job>0){
        DialogSplit d(id_job);
        if (d.exec()==QDialog::Accepted){
            modelJob->select();
        }
    }
}

ModelJob::ModelJob(QWidget *parent) : DbTableModel("rab_job",parent)
{
    executorNorm = new Executor(this);
    executorPart = new Executor(this);
    id_brig=-1;
    addColumn("id",tr("id"));
    addColumn("lid",tr("Наименование работы (F4)"),Rels::instance()->relJobNam);
    addColumn("dat",tr("Дата уч."));
    addColumn("datf",tr("Дата факт."));
    addColumn("kvo",tr("Объем"));
    addColumn("id_part",tr("Партия"),Rels::instance()->relElPart);
    addColumn("id_rb",tr("Бригадир"),Rels::instance()->relRab);
    addColumn("id_press",tr("Пресс"),Rels::instance()->relPress);
    addColumn("chas_sm",tr("ч/см"));
    addColumn("extr_time",tr("Св.ур,%"));
    //addColumn("parti",tr("Партия"));

    setSuffix("left join rab_nams on rab_nams.lid = rab_job.lid left join rab_liter on rab_liter.id = rab_nams.id left join parti as p on p.id = rab_job.id_part");
    setDecimals(4,4);

    connect(this,SIGNAL(sigRefresh()),this,SLOT(refreshState()));
    connect(this,SIGNAL(sigUpd()),this,SLOT(refreshState()));
    connect(executorNorm,SIGNAL(finished()),this,SLOT(stNormFinished()));
    connect(executorPart,SIGNAL(finished()),this,SLOT(stPartFinished()));
}

QVariant ModelJob::data(const QModelIndex &index, int role) const
{
    if (role==Qt::BackgroundRole){
        int id_job=this->data(this->index(index.row(),0),Qt::EditRole).toInt();
        if (notOkNorm.contains(id_job)){
            return QVariant(QColor(255,200,100));
        } else if (notOkPart.contains(id_job)){
            return QVariant(QColor(255,170,170));
        }
        if (index.column()==6 && id_brig>0){
            return QVariant(QColor(255,255,175));
        }
    }
    return DbTableModel::data(index,role);
}

void ModelJob::refresh(QDate beg, QDate end, QString zon, int id_rb, bool zero, QString parti)
{
    dbeg=beg;
    dend=end;
    QString flt = QString("rab_job.dat between '%1' and '%2' and rab_liter.zon in %3").arg(beg.toString("yyyy-MM-dd")).arg(end.toString("yyyy-MM-dd")).arg(zon);
    if (id_rb>0){
        id_brig=-1;
        flt+=QString(" and (rab_job.id_rb = %1 or exists (select rab_share.id from rab_share where rab_share.id_job=rab_job.id and rab_share.id_rab = %1))").arg(id_rb);
    }
    if (zero){
        flt+=" and (rab_job.kvo = 0.0 or coalesce((select min(rab_share.kvo) from rab_share where rab_share.id_job=rab_job.id),0.0) = 0.0)";
    }
    if (!parti.isEmpty()){
        flt+=QString(" and p.n_s ILIKE '%1%'").arg(parti);
    }
    if (id_brig>0){
        flt+=QString(" and rab_job.id_rb = %1").arg(id_brig);
    }
    setFilter(flt);
    setSort("rab_job.dat, rab_job.id");
    setDefaultValue(2,end);
    setDefaultValue(3,end);
    select();
}

void ModelJob::setIdBrig(int id)
{
    id_brig=id;
    if (id_brig>0){
        setDefaultValue(6,id_brig);
    } else {
        setDefaultValue(6,this->nullVal(6));
    }
}

int ModelJob::getIdBrig()
{
    return id_brig;
}

void ModelJob::refreshState()
{
    QString queryNorm=QString("select rj.id from rab_job rj "
                          "inner join rab_nams rn on rn.lid = rj.lid "
                          "inner join rab_liter rl on rl.id = rn.id "
                          "where rl.is_nrm=true and rj.dat between '%1' and '%2' "
                          "and get_norm_el(rj.id,rj.chas_sm) is null").arg(dbeg.toString("yyyy-MM-dd")).arg(dend.toString("yyyy-MM-dd"));
    executorNorm->setQuery(queryNorm);
    executorNorm->start();

    QString queryPart=QString("select rj.id from rab_job rj "
                              "inner join rab_nams rn on rn.lid = rj.lid "
                              "inner join parti p on p.id = rj.id_part "
                              "where rj.dat between '%1' and '%2' and "
                              "(rn.dim >0 and (rn.id_el<>p.id_el or rn.dim<>p.diam) or (rn.dim=0 and rn.id_el<>p.id_el))").arg(dbeg.toString("yyyy-MM-dd")).arg(dend.toString("yyyy-MM-dd"));
    executorPart->setQuery(queryPart);
    executorPart->start();
}

void ModelJob::stPartFinished()
{
    QVector<QVector<QVariant>> data = executorPart->getData();
    notOkPart.clear();
    for (QVector<QVariant> row : data){
        notOkPart.insert(row.at(0).toInt());
    }
    emit dataChanged(this->index(0,0),this->index(this->rowCount()-1,this->columnCount()-1));
}

void ModelJob::stNormFinished()
{
    QVector<QVector<QVariant>> data = executorNorm->getData();
    notOkNorm.clear();
    for (QVector<QVariant> row : data){
        notOkNorm.insert(row.at(0).toInt());
    }
    emit dataChanged(this->index(0,0),this->index(this->rowCount()-1,this->columnCount()-1));
}

ModelShare::ModelShare(QWidget *parent): DbTableModel("rab_share",parent)
{
    idCopy=-1;
    addColumn("id",tr("id"));
    addColumn("id_job",tr("id_job"));
    addColumn("id_rab",tr("Работник (F5)"),Rels::instance()->relRab);
    addColumn("koef_prem_kvo",tr("К.прем"));
    addColumn("kvo",tr("Выполн."));
    addColumn("s_koef",tr("Коэфф"));
    addColumn("prem",tr("Св.ур,%"));
    setSort("rab_share.id");
    setDecimals(3,2);
    setDecimals(4,4);
    setDecimals(5,2);
    setDecimals(6,1);
    setDefaultValue(3,1.0);
    setDefaultValue(4,1.0);
    setDefaultValue(5,1.0);
}

void ModelShare::refresh(int id_job)
{
    setDefaultValue(1,id_job);
    setFilter("rab_share.id_job = "+QString::number(id_job));
    select();
}

void ModelShare::insShare()
{
    int id_job=this->defaultValue(1).toInt();
    QSqlQuery query;
    query.prepare("insert into rab_share (id_job, id_rab) values (:id_job, (select id_rb from rab_job where id = :id_job))");
    query.bindValue(":id_job",id_job);
    if (query.exec()){
        select();
    } else {
        QMessageBox::critical(nullptr,"Error",query.lastError().text(),QMessageBox::Cancel);
    }
}

void ModelShare::copy()
{
    idCopy=this->defaultValue(1).toInt();
}

void ModelShare::paste()
{
    if (idCopy>0){
        int id_job=this->defaultValue(1).toInt();
        QSqlQuery query;
        query.prepare("insert into rab_share (id_job, id_rab, koef_prem_kvo, kvo, s_koef, prem) (select :id_job, id_rab, koef_prem_kvo, kvo, s_koef, prem from rab_share where id_job = :id_copy order by id)");
        query.bindValue(":id_job",id_job);
        query.bindValue(":id_copy",idCopy);
        if (query.exec()){
            select();
        } else {
            QMessageBox::critical(nullptr,"Error",query.lastError().text(),QMessageBox::Cancel);
        }
    }
}
