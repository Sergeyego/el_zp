#include "formcalcwage.h"
#include "ui_formcalcwage.h"

FormCalcWage::FormCalcWage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormCalcWage)
{
    ui->setupUi(this);
    loadSettings();

    sqlExecutor = new ProgressExecutor(this);

    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    ui->dateEditEnd->setDate(QDate::currentDate());

    modelRab = new ModelRo(this);
    ui->listViewRab->setModel(modelRab);

    modelTableData = new TableModel(this);
    modelTableData->setDecimal(2);
    modelTableData->setDecimalForColumn(2,3);
    modelTableData->setDecimalForColumn(3,3);
    modelTableData->setDecimalForColumn(4,1);
    modelTableData->setDecimalForColumn(5,1);
    modelTableData->setDecimalForColumn(6,1);

    modelTableDataVid = new TableModel(this);
    modelTableDataVid->setDecimal(2);
    modelTableDataVid->setDecimalForColumn(1,3);

    QStringList header;
    header<<tr("Дата")<<tr("Наименование работ")<<tr("Общ.к-во")<<tr("Кол-во")<<tr("% кач.")<<tr("% норм.")<<tr("% мес.")<<tr("Тариф")<<tr("З.пл.")<<tr("Допл.")<<tr("Выход.")<<tr("Ночн.")<<tr("Прем.кач")<<tr("Прем.норм.")<<tr("Прем.мес.")<<tr("К выдаче");
    modelTableData->setHeader(header);

    QStringList headervid;
    headervid<<tr("Наименование работ")<<tr("Кол-во")<<tr("Тариф")<<tr("З.пл.")<<tr("Допл.")<<tr("Выход.")<<tr("Ночн.")<<tr("Прем.кач")<<tr("Прем.норм.")<<tr("Прем.мес.")<<tr("К выдаче");
    modelTableDataVid->setHeader(headervid);

    ui->tableViewCalc->setModel(modelTableData);
    ui->tableViewCalc->resizeToContents();

    connect(ui->pushButtonCalc,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->listViewRab->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updTableData(QModelIndex)));
    connect(ui->pushButtonSave,SIGNAL(clicked(bool)),this,SLOT(saveTabelXlsx()));
    connect(ui->pushButtonTabelShort,SIGNAL(clicked(bool)),this,SLOT(tabelShort()));
    connect(ui->pushButtonTabel,SIGNAL(clicked(bool)),this,SLOT(tabel()));
    connect(ui->checkBoxVid,SIGNAL(clicked(bool)),this,SLOT(setDataModel()));
    connect(sqlExecutor,SIGNAL(finished()),this,SLOT(updFinished()));
}

FormCalcWage::~FormCalcWage()
{
    saveSettings();
    delete ui;
}

void FormCalcWage::setBlock(bool b)
{
    ui->pushButtonSave->setDisabled(b);
    ui->pushButtonTabel->setDisabled(b);
    ui->pushButtonTabelShort->setDisabled(b);
}

void FormCalcWage::loadSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    ui->splitter->restoreState(settings.value("calc_wage_splitter_width").toByteArray());
}

void FormCalcWage::saveSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    settings.setValue("calc_wage_splitter_width",ui->splitter->saveState());
}

FormCalcWage::sumData FormCalcWage::getSum(int id_rab)
{
    sumData sum;
    sum.zpl=0;
    sum.dopl=0;
    sum.extr=0;
    sum.night=0;
    sum.premk=0;
    sum.premn=0;
    sum.prem=0;
    sum.total=0;
    QList<tabelData> list = hlong.values(id_rab);
    for (tabelData data : list){
        sum.zpl+=data.zpl;
        sum.dopl+=data.dopl;
        sum.extr+=data.extr;
        sum.night+=data.night;
        sum.premk+=data.premk;
        sum.premn+=data.premn;
        sum.prem+=data.prem;
        sum.total+=data.total;
    }
    return sum;
}

QVector<QVector<QVariant> > FormCalcWage::getLongData(int id_rab)
{
    QVector<QVector<QVariant>> dataLong;
    QList<tabelData> list = hlong.values(id_rab);
    QList<tabelData>::const_iterator data = list.end();
    while (data != list.begin()) {
        --data;
        QVector<QVariant> row;
        row.push_back((*data).date);
        row.push_back((*data).name);
        row.push_back((*data).s_kvo);
        row.push_back((*data).kvo);
        row.push_back((*data).prk);
        row.push_back((*data).prn);
        row.push_back((*data).pr);
        row.push_back((*data).tarif);
        row.push_back((*data).zpl);
        row.push_back((*data).dopl);
        row.push_back((*data).extr);
        row.push_back((*data).night);
        row.push_back((*data).premk);
        row.push_back((*data).premn);
        row.push_back((*data).prem);
        row.push_back((*data).total);
        dataLong.push_back(row);
    }
    return dataLong;
}

QVector<QVector<QVariant> > FormCalcWage::getShortData(int id_rab)
{
    QVector<QVector<QVariant>> dataShort;
    QList<tabelData> list = hlong.values(id_rab);
    QMap<QString, sumData> map;
    QString sep="^_^";
    for (tabelData data : list){
        QString key = data.name+sep+QString::number(data.tarif,'f',2);
        if (map.contains(key)){
             sumData d = map.value(key);
             d.kvo+=data.kvo;
             d.zpl+=data.zpl;
             d.dopl+=data.dopl;
             d.extr+=data.extr;
             d.night+=data.night;
             d.premk+=data.premk;
             d.premn+=data.premn;
             d.prem+=data.prem;
             d.total+=data.total;
             map.insert(key,d);
        } else {
            sumData d;
            d.kvo=data.kvo;
            d.zpl=data.zpl;
            d.dopl=data.dopl;
            d.extr=data.extr;
            d.night=data.night;
            d.premk=data.premk;
            d.premn=data.premn;
            d.prem=data.prem;
            d.total=data.total;
            map.insert(key,d);
        }
    }
    QList<QString> job = map.keys();
    for (QString j : job){
        QStringList key = j.split(sep);
        if (key.size()==2){
            sumData sum = map.value(j);
            QVector<QVariant> row;
            row.push_back(key[0]);
            row.push_back(sum.kvo);
            row.push_back(key[1].toDouble());
            row.push_back(sum.zpl);
            row.push_back(sum.dopl);
            row.push_back(sum.extr);
            row.push_back(sum.night);
            row.push_back(sum.premk);
            row.push_back(sum.premn);
            row.push_back(sum.prem);
            row.push_back(sum.total);
            dataShort.push_back(row);
        }
    }
    return dataShort;
}

void FormCalcWage::upd()
{
    setBlock(true);
    if (ui->listViewRab->model()->rowCount()){
        current_id_rab=ui->listViewRab->model()->data(ui->listViewRab->model()->index(ui->listViewRab->currentIndex().row(),0),Qt::EditRole).toInt();
    } else {
        current_id_rab=-1;
    }
    modelTableData->clear();
    modelTableDataVid->clear();
    hlong.clear();
    ui->lineEditZpl->clear();
    ui->lineEditDopl->clear();
    ui->lineEditExtr->clear();
    ui->lineEditNight->clear();
    ui->lineEditPremk->clear();
    ui->lineEditPremn->clear();
    ui->lineEditPrem->clear();
    ui->lineEditTotal->clear();
    ui->labelItogo->setText("Итого:");
    QString query = QString("select id, dat, fnam, s_kvo, kvo, prk, prn, pr, tarif, zpl, dopl, extr, night, premk, premn, prem, "
                            "(zpl+dopl+extr+night+premk+premn+prem) as total from zrw_calc_wage('%1', '%2')").arg(ui->dateEditBeg->date().toString("yyyy-MM-dd")).arg(ui->dateEditEnd->date().toString("yyyy-MM-dd"));
    sqlExecutor->setQuery(query);
    sqlExecutor->start();
    updTitle();
}

void FormCalcWage::updFinished()
{
    QVector<QVector<QVariant>> data = sqlExecutor->getData();
    QSqlQuery query;
    query.prepare("select rr.id, rr.fam, rr.nam, rr.otc, rr.tabel, rr.snam || ' ' || rp.nam || ' ' || (case when rr2.num<>'-' then '('||rr2.num||' разряд)' else '' end), "
                  "rp.nam || ' ' || (case when rr2.num<>'-' then '('||rr2.num||' разряд)' else '' end), zwc.sdat "
                  "from rab_rab rr "
                  "inner join ( "
                  "    select rs.id_rab as id_rab, count(distinct rj.dat) as sdat from rab_share rs "
                  "    inner join rab_job rj on rj.id = rs.id_job "
                  "    where rj.dat between :d1 and :d2 "
                  "    group by rs.id_rab ) as zwc on zwc.id_rab = rr.id "
                  "left join (select rq.id_rab as id_rab, max(rq.dat) as mdat from rab_qual rq "
                  "    where rq.dat <= :d3 and rq.id_prof <> 0 "
                  "    group by rq.id_rab ) as rh on rh.id_rab=rr.id "
                  "left join rab_qual rq2 on rq2.id_rab = rh.id_rab and rq2.dat = rh.mdat "
                  "left join rab_prof rp on rp.id = rq2.id_prof "
                  "left join rab_razr rr2 on rr2.id = rq2.id_razr "
                  "order by rr.fam, rr.nam, rr.otc");
    query.bindValue(":d1",ui->dateEditBeg->date());
    query.bindValue(":d2",ui->dateEditEnd->date());
    query.bindValue(":d3",ui->dateEditEnd->date());
    if (modelRab->execQuery(query)){
        ui->listViewRab->setModelColumn(5);
    }
    bool ok = ui->listViewRab->model()->rowCount();
    setBlock(!ok);
    sumData sum;
    sum.zpl=0;
    sum.dopl=0;
    sum.extr=0;
    sum.night=0;
    sum.premk=0;
    sum.premn=0;
    sum.prem=0;
    sum.total=0;

    for (QVector<QVariant> row : data){
        tabelData d;
        d.date=row[1].toDate();
        d.name=row[2].toString();
        d.s_kvo=row[3].toDouble();
        d.kvo=row[4].toDouble();
        d.prk=row[5].toDouble();
        d.prn=row[6].toDouble();
        d.pr=row[7].toDouble();
        d.tarif=row[8].toDouble();
        d.zpl=row[9].toDouble();
        d.dopl=row[10].toDouble();
        d.extr=row[11].toDouble();
        d.night=row[12].toDouble();
        d.premk=row[13].toDouble();
        d.premn=row[14].toDouble();
        d.prem=row[15].toDouble();
        d.total=row[16].toDouble();
        hlong.insert(row[0].toInt(),d);

        sum.zpl+=d.zpl;
        sum.dopl+=d.dopl;
        sum.extr+=d.extr;
        sum.night+=d.night;
        sum.premk+=d.premk;
        sum.premn+=d.premn;
        sum.prem+=d.prem;
        sum.total+=d.total;
    }

    ui->labelItogo->setText(QString("З.пл: %1 ₽; Допл.: %2 ₽; Вых.: %3 ₽; Ноч.: %4 ₽; Прем.кач.: %5 ₽; Прем.норм.: %6 ₽; Прем.мес.: %7 ₽; Итого: %8 ₽;")
                            .arg(QLocale().toString(sum.zpl,'f',2))
                            .arg(QLocale().toString(sum.dopl,'f',2))
                            .arg(QLocale().toString(sum.extr,'f',2))
                            .arg(QLocale().toString(sum.night,'f',2))
                            .arg(QLocale().toString(sum.premk,'f',2))
                            .arg(QLocale().toString(sum.premn,'f',2))
                            .arg(QLocale().toString(sum.prem,'f',2))
                            .arg(QLocale().toString(sum.total,'f',2)));
    int currentRow=0;
    if (current_id_rab>=0){
        for (int i=0; i<ui->listViewRab->model()->rowCount(); i++){
            if (ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,0),Qt::EditRole).toInt()==current_id_rab){
                currentRow=i;
                break;
            }
        }
    }
    if (ui->listViewRab->model()->rowCount()){
        ui->listViewRab->setCurrentIndex(ui->listViewRab->model()->index(currentRow,5));
    }
}

void FormCalcWage::setDataModel()
{
    if (ui->checkBoxVid->isChecked()){
        ui->tableViewCalc->setModel(modelTableDataVid);
    } else {
        ui->tableViewCalc->setModel(modelTableData);
    }
    ui->tableViewCalc->resizeToContents();
}

void FormCalcWage::updTitle()
{
    QSqlQuery query;
    query.prepare("select fnam, sign_norm_el from hoz where id=11");
    if (query.exec()){
        while (query.next()){
            orgName=query.value(0).toString();
            sign=query.value(1).toString();
        }
    } else {
        QMessageBox::critical(NULL,"Error",query.lastError().text(),QMessageBox::Cancel);
    }
}


void FormCalcWage::updTableData(QModelIndex ind)
{
    int id_rb=ui->listViewRab->model()->data(ui->listViewRab->model()->index(ind.row(),0),Qt::EditRole).toInt();

    sumData sum = getSum(id_rb);
    ui->lineEditZpl->setText(QLocale().toString(sum.zpl,'f',2));
    ui->lineEditDopl->setText(QLocale().toString(sum.dopl,'f',2));
    ui->lineEditExtr->setText(QLocale().toString(sum.extr,'f',2));
    ui->lineEditNight->setText(QLocale().toString(sum.night,'f',2));
    ui->lineEditPremk->setText(QLocale().toString(sum.premk,'f',2));
    ui->lineEditPremn->setText(QLocale().toString(sum.premn,'f',2));
    ui->lineEditPrem->setText(QLocale().toString(sum.prem,'f',2));
    ui->lineEditTotal->setText(QLocale().toString(sum.total,'f',2));

    modelTableData->setModelData(getLongData(id_rb));
    modelTableDataVid->setModelData(getShortData(id_rb));

    ui->tableViewCalc->resizeToContents();
}

void FormCalcWage::saveTabelXlsx()
{
    if (ui->tableViewCalc->model()->rowCount()){
        QString fnam = ui->listViewRab->model()->data(ui->listViewRab->model()->index(ui->listViewRab->currentIndex().row(),5),Qt::EditRole).toString();
        ui->tableViewCalc->save(fnam+tr(" c ")+ui->dateEditBeg->date().toString("dd.MM.yyyy")+tr(" по ")+ui->dateEditEnd->date().toString("dd.MM.yyyy"),-1,true,Qt::LandscapeOrientation);
    }
}

void FormCalcWage::tabelShort()
{
    int rabcount=ui->listViewRab->model()->rowCount();
    if (!rabcount){
        return;
    }

    QString title=QString("Начисление заработной платы кратко с %1 по %2").arg(ui->dateEditBeg->date().toString("dd.MM.yy")).arg(ui->dateEditEnd->date().toString("dd.MM.yy"));

    Document xlsx;
    Worksheet *ws=xlsx.currentWorksheet();
    XlsxPageSetup pageSetup;
    pageSetup.fitToPage=true;
    pageSetup.fitToWidth=1;
    pageSetup.fitToHeight=0;
    pageSetup.orientation=XlsxPageSetup::landscape;
    ws->setPageSetup(pageSetup);
    QFont defaultFont("Arial", 10);
    QFont titleFont("Arial", 10);
    titleFont.setBold(true);

    Format strFormat;
    strFormat.setBorderStyle(Format::BorderThin);
    strFormat.setFont(defaultFont);
    Format numFormat;
    numFormat.setBorderStyle(Format::BorderThin);
    numFormat.setFont(defaultFont);
    Format titleFormat;
    titleFormat.setBorderStyle(Format::BorderNone);
    titleFormat.setFont(titleFont);
    titleFormat.setTextWarp(true);
    titleFormat.setHorizontalAlignment(Format::AlignHCenter);
    titleFormat.setVerticalAlignment(Format::AlignVCenter);

    Format headerFormat;
    headerFormat.setBorderStyle(Format::BorderThin);
    headerFormat.setFont(titleFont);
    headerFormat.setTextWarp(true);
    headerFormat.setHorizontalAlignment(Format::AlignHCenter);
    headerFormat.setVerticalAlignment(Format::AlignVCenter);

    int m=1;
    ws->setColumnWidth(1,1,15);
    ws->setColumnWidth(2,2,15);
    ws->setColumnWidth(3,3,15);
    ws->setColumnWidth(4,4,40);
    ws->setColumnWidth(5,5,13);
    ws->setColumnWidth(6,6,13);
    ws->setColumnWidth(7,7,13);
    ws->setColumnWidth(8,8,13);
    ws->setColumnWidth(9,9,13);
    ws->setColumnWidth(10,10,13);

    ws->setRowHeight(m,m+1,40);
    ws->mergeCells(CellRange(m,1,m,10),titleFormat);
    ws->writeString(m,1,title,titleFormat);
    m++;
    ws->writeString(m,1,"Фамилия",headerFormat);
    ws->writeString(m,2,"Имя",headerFormat);
    ws->writeString(m,3,"Отчество",headerFormat);
    ws->writeString(m,4,"Профессия, разряд",headerFormat);
    ws->writeString(m,5,"Отработал дней",headerFormat);
    ws->writeString(m,6,"Зарплата",headerFormat);
    ws->writeString(m,7,"Св. уроч.",headerFormat);
    ws->writeString(m,8,"Премия за качество",headerFormat);
    ws->writeString(m,9,"Премия за норму",headerFormat);
    ws->writeString(m,10,"К выдаче",headerFormat);
    m++;

    for (int i=0; i<rabcount; i++){

        /*QString firstName, lastName, middleName, prof;
        int kvoDay;
        double zp, ex, premk, premn, total;

        //id_rab=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,0),Qt::EditRole).toInt();
        firstName=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,1),Qt::EditRole).toString();
        lastName=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,2),Qt::EditRole).toString();
        middleName=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,3),Qt::EditRole).toString();
        prof=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,6),Qt::EditRole).toString();

        kvoDay=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,7),Qt::EditRole).toInt();
        zp=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,8),Qt::EditRole).toDouble();
        ex=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,10),Qt::EditRole).toDouble();
        premk=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,11),Qt::EditRole).toDouble();
        premn=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,12),Qt::EditRole).toDouble();
        total=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,13),Qt::EditRole).toDouble();


        if (!kvoDay) continue;

        ws->writeString(m,1,firstName,strFormat);
        ws->writeString(m,2,lastName,strFormat);
        ws->writeString(m,3,middleName,strFormat);
        ws->writeString(m,4,prof,strFormat);
        numFormat.setNumberFormat("0");
        ws->writeNumeric(m,5,kvoDay,numFormat);
        //QString fmt=QString("# ### ##0.%1").arg((0),2,'d',0,QChar('0'));
        //numFormat.setNumberFormat(fmt);
        numFormat.setNumberFormatIndex(4);

        ws->writeNumeric(m,6,zp,numFormat);
        ws->writeNumeric(m,7,ex,numFormat);
        ws->writeNumeric(m,8,premk,numFormat);
        ws->writeNumeric(m,9,premn,numFormat);
        ws->writeNumeric(m,10,total,numFormat);
        m++;*/

    }

    ws->write(m,6,QString("=SUM(F3:F%1)").arg(m-1),numFormat);
    ws->write(m,7,QString("=SUM(G3:G%1)").arg(m-1),numFormat);
    ws->write(m,8,QString("=SUM(H3:H%1)").arg(m-1),numFormat);
    ws->write(m,9,QString("=SUM(I3:I%1)").arg(m-1),numFormat);
    ws->write(m,10,QString("=SUM(J3:J%1)").arg(m-1),numFormat);

    QDir dir(QDir::homePath());
    QString filename = QFileDialog::getSaveFileName(nullptr,QString::fromUtf8("Сохранить файл"),
                                                    dir.path()+"/"+title+".xlsx",
                                                    QString::fromUtf8("Documents (*.xlsx)") );
    if (!filename.isEmpty()){
        if (filename.right(5)!=".xlsx"){
            filename+=".xlsx";
        }
        xlsx.saveAs(filename);
    }
}

void FormCalcWage::tabel()
{
    int rabcount=ui->listViewRab->model()->rowCount();
    if (!rabcount){
        return;
    }

    QString title=QString("Начисление заработной платы с %1 по %2").arg(ui->dateEditBeg->date().toString("dd.MM.yy")).arg(ui->dateEditEnd->date().toString("dd.MM.yy"));

    Document xlsx;
    Worksheet *ws=xlsx.currentWorksheet();
    XlsxPageSetup pageSetup;
    pageSetup.fitToPage=true;
    pageSetup.fitToWidth=1;
    pageSetup.fitToHeight=0;
    pageSetup.orientation=XlsxPageSetup::landscape;
    ws->setPageSetup(pageSetup);
    QFont defaultFont("Arial", 10);
    QFont titleFont("Arial", 10);
    titleFont.setBold(true);

    XlsxPageMargins margins=ws->pageMargins();
    margins.top=0.664583333333333;
    margins.bottom=0.928472222222222;
    ws->setPageMargins(margins);

    Format strFormat;
    strFormat.setBorderStyle(Format::BorderThin);
    strFormat.setFont(defaultFont);
    strFormat.setTextWarp(true);

    Format numFormat;
    numFormat.setBorderStyle(Format::BorderThin);
    numFormat.setFont(defaultFont);
    Format titleFormat;
    titleFormat.setBorderStyle(Format::BorderNone);
    titleFormat.setFont(titleFont);
    titleFormat.setTextWarp(true);
    titleFormat.setHorizontalAlignment(Format::AlignHCenter);
    titleFormat.setVerticalAlignment(Format::AlignVCenter);

    Format headerFormat;
    headerFormat.setBorderStyle(Format::BorderThin);
    headerFormat.setFont(titleFont);
    headerFormat.setTextWarp(true);
    headerFormat.setHorizontalAlignment(Format::AlignHCenter);
    headerFormat.setVerticalAlignment(Format::AlignVCenter);

    int m=1;
    ws->setColumnWidth(1,1,4);
    ws->setColumnWidth(2,2,15);
    ws->setColumnWidth(3,3,15);
    ws->setColumnWidth(4,4,15);
    ws->setColumnWidth(5,5,20);
    ws->setColumnWidth(6,6,11);
    ws->setColumnWidth(7,7,6);
    ws->setColumnWidth(8,8,11);
    ws->setColumnWidth(9,9,9);
    ws->setColumnWidth(10,10,10);
    ws->setColumnWidth(11,11,10);
    ws->setColumnWidth(12,12,10);
    ws->setColumnWidth(13,13,10);
    ws->setColumnWidth(14,14,10);
    ws->setColumnWidth(15,15,10);
    ws->setColumnWidth(16,16,10);

    for (int i=0; i<rabcount; i++){

        /*int id_rab;
        QString firstName, lastName, middleName, prof;
        int kvoDay;
        double zp, dopl, extrtime, premk, premn, total;

        id_rab=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,0)).toInt();
        firstName=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,1)).toString();
        lastName=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,2)).toString();
        middleName=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,3)).toString();
        prof=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,6)).toString();

        ws->mergeCells(CellRange(m,1,m+1,1),headerFormat);
        ws->writeString(m,1,QString("№"),headerFormat);
        ws->mergeCells(CellRange(m,2,m,4),headerFormat);
        ws->writeString(m,2,QString("Фамилия Имя Отчество"),headerFormat);

        ws->mergeCells(CellRange(m+1,2,m+1,4),headerFormat);
        ws->writeString(m+1,2,QString("Наименование работ"),headerFormat);

        ws->mergeCells(CellRange(m,5,m+1,5),headerFormat);
        ws->writeString(m,5,QString("Профессия, разряд"),headerFormat);

        ws->mergeCells(CellRange(m,6,m+1,6),headerFormat);
        ws->writeString(m,6,QString("Начислено"),headerFormat);

        ws->mergeCells(CellRange(m,7,m+1,7),headerFormat);
        ws->writeString(m,7,QString("Р. дн."),headerFormat);

        ws->mergeCells(CellRange(m,8,m+1,8),headerFormat);
        ws->writeString(m,8,QString("Кол-во"),headerFormat);

        ws->mergeCells(CellRange(m,9,m+1,9),headerFormat);
        ws->writeString(m,9,QString("Тариф"),headerFormat);

        ws->mergeCells(CellRange(m,10,m+1,10),headerFormat);
        ws->writeString(m,10,QString("Зар.пл."),headerFormat);

        ws->mergeCells(CellRange(m,11,m+1,11),headerFormat);
        ws->writeString(m,11,QString("В т.ч.допл"),headerFormat);

        ws->mergeCells(CellRange(m,12,m+1,12),headerFormat);
        ws->writeString(m,12,QString("В средн за день"),headerFormat);

        ws->mergeCells(CellRange(m,13,m+1,13),headerFormat);
        ws->writeString(m,13,QString("Сверх уроч."),headerFormat);

        ws->mergeCells(CellRange(m,14,m,15),headerFormat);
        ws->writeString(m,14,QString("Премия"),headerFormat);
        ws->writeString(m+1,14,QString("Кач"),headerFormat);
        ws->writeString(m+1,15,QString("Норм"),headerFormat);

        ws->mergeCells(CellRange(m,16,m+1,16),headerFormat);
        ws->writeString(m,16,QString("К выдаче"),headerFormat);

        m+=2;

        kvoDay=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,7),Qt::EditRole).toInt();
        zp=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,8),Qt::EditRole).toDouble();
        dopl=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,9),Qt::EditRole).toDouble();;
        extrtime=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,10),Qt::EditRole).toDouble();
        premk=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,11),Qt::EditRole).toDouble();
        premn=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,12),Qt::EditRole).toDouble();
        total=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,13),Qt::EditRole).toDouble();

        if (!kvoDay) continue;

        ws->setRowHeight(m,m,35);

        numFormat.setNumberFormat("0");
        ws->writeNumeric(m,1,i+1,numFormat);

        ws->writeString(m,2,firstName,strFormat);
        ws->writeString(m,3,lastName,strFormat);
        ws->writeString(m,4,middleName,strFormat);
        ws->writeString(m,5,prof,strFormat);

        numFormat.setNumberFormat(QString("0.%1").arg((0),2,'d',0,QChar('0')));
        ws->writeNumeric(m,6,total,numFormat);

        numFormat.setNumberFormat("0");
        ws->writeNumeric(m,7,kvoDay,numFormat);

        ws->writeBlank(m,8,strFormat);
        ws->writeBlank(m,9,strFormat);
        ws->writeBlank(m,10,strFormat);
        ws->writeBlank(m,11,strFormat);

        numFormat.setNumberFormat(QString("0.%1").arg((0),2,'d',0,QChar('0')));
        ws->write(m,12,QString("=F%1/G%2").arg(m).arg(m),numFormat);

        ws->writeBlank(m,13,strFormat);
        ws->writeBlank(m,14,strFormat);
        ws->writeBlank(m,15,strFormat);
        ws->writeBlank(m,16,strFormat);

        m++;
        int begm=m;

        QList<tabelData> list = mapData.values(id_rab);
        for (tabelData data : list){

            ws->writeBlank(m,1,numFormat);

            ws->mergeCells(CellRange(m,2,m,7),numFormat);
            ws->writeString(m,2,data.name,strFormat);

            numFormat.setNumberFormat(QString("0.%1").arg((0),3,'d',0,QChar('0')));
            ws->writeNumeric(m,8,data.kvo,numFormat);

            numFormat.setNumberFormat(QString("0.%1").arg((0),2,'d',0,QChar('0')));
            ws->writeNumeric(m,9,data.tarif,numFormat);
            ws->writeNumeric(m,10,data.zpl,numFormat);
            ws->writeNumeric(m,11,data.dopl,numFormat);
            ws->writeBlank(m,12,numFormat);
            ws->writeNumeric(m,13,data.extr,numFormat);
            ws->writeNumeric(m,14,data.bonus,numFormat);
            ws->writeNumeric(m,15,data.nrm,numFormat);
            ws->writeNumeric(m,16,data.total,numFormat);

            m++;
        }

        ws->mergeCells(CellRange(begm,1,m-1,1),strFormat);
        ws->writeString(m,9,QString("ИТОГО:"),strFormat);

        numFormat.setNumberFormat(QString("0.%1").arg((0),2,'d',0,QChar('0')));
        ws->writeNumeric(m,10,zp,numFormat);
        ws->writeNumeric(m,11,dopl,numFormat);
        ws->writeBlank(m,12,numFormat);
        ws->writeNumeric(m,13,extrtime,numFormat);
        ws->writeNumeric(m,14,premk,numFormat);
        ws->writeNumeric(m,15,premn,numFormat);
        ws->writeNumeric(m,16,total,numFormat);

        m++;
        ws->insertRowBreak(m);
        m++;*/
    }

    QString year=QString::number(ui->dateEditBeg->date().year());
    QString month=QDate::longMonthName(ui->dateEditBeg->date().month(),QDate::StandaloneFormat);
    QString dat=month+" "+year+QString(" г.");

    QString headerData=QString("&LРасчет заработной платы за %1&C%2&R&D").arg(dat).arg(orgName);
    QString footerData=QString("&L%1").arg(sign);

    ws->setHeaderData(headerData);
    ws->setFooterData(footerData);

    QDir dir(QDir::homePath());
    QString filename = QFileDialog::getSaveFileName(nullptr,QString::fromUtf8("Сохранить файл"),
                                                    dir.path()+"/"+title+".xlsx",
                                                    QString::fromUtf8("Documents (*.xlsx)") );
    if (!filename.isEmpty()){
        if (filename.right(5)!=".xlsx"){
            filename+=".xlsx";
        }
        xlsx.saveAs(filename);
    }
}
