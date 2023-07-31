#include "formcalcwage.h"
#include "ui_formcalcwage.h"

FormCalcWage::FormCalcWage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormCalcWage)
{
    ui->setupUi(this);
    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    ui->dateEditEnd->setDate(QDate::currentDate());

    modelRab = new ModelRo(this);
    ui->listViewRab->setModel(modelRab);

    modelTableData = new ModelRo(this);
    ui->tableViewCalc->setModel(modelTableData);

    connect(ui->pushButtonCalc,SIGNAL(clicked(bool)),this,SLOT(reCalc()));
    connect(ui->listViewRab->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updTableData(QModelIndex)));
    connect(ui->pushButtonSave,SIGNAL(clicked(bool)),this,SLOT(saveTabelXlsx()));
    connect(ui->pushButtonTabelShort,SIGNAL(clicked(bool)),this,SLOT(tabelShort()));
    connect(ui->pushButtonTabel,SIGNAL(clicked(bool)),this,SLOT(tabel()));
}

FormCalcWage::~FormCalcWage()
{
    delete ui;
}

void FormCalcWage::setBlock(bool b)
{
    ui->pushButtonJob->setDisabled(b);
    ui->pushButtonSave->setDisabled(b);
    ui->pushButtonTabel->setDisabled(b);
    ui->pushButtonTabelShort->setDisabled(b);
}

void FormCalcWage::reCalc()
{
    modelTableData->clear();
    QSqlQuery queryCre;
    queryCre.prepare("select * from zrw_prep_wage(:d1, :d2)");
    queryCre.bindValue(":d1",ui->dateEditBeg->date());
    queryCre.bindValue(":d2",ui->dateEditEnd->date());
    if (queryCre.exec()){
        QSqlQuery query;
        query.prepare("select rr.id, rr.fam, rr.nam, rr.otc, rr.tabel, rr.snam || ' ' || rp.nam || ' ' || (case when rr2.num<>'-' then '('||rr2.num||' разряд)' else '' end), "
                      "rp.nam || ' ' || (case when rr2.num<>'-' then '('||rr2.num||' разряд)' else '' end), zwc.sdat, zwc.szpl, zwc.sextr, zwc.sbonus, zwc.snrm, zwc.svid "
                      "from rab_rab rr "
                      "inner join (select zw.id as id, "
                      "            count(distinct zw.dat) as sdat, sum(zw.zpl) as szpl, sum(zw.extr) as sextr, sum(zw.bonus) as sbonus, sum(zw.nrm) as snrm, sum(zw.zpl+zw.extr+zw.bonus+zw.nrm) as svid "
                      "            from ztw_wage zw group by zw.id) as zwc on zwc.id = rr.id "
                      "left join (select rq.id_rab as id_rab, max(rq.dat) as mdat from rab_qual rq "
                      "    where rq.dat <=:d3 and rq.id_prof <> 0 "
                      "    group by rq.id_rab ) as rh on rh.id_rab=rr.id "
                      "left join rab_qual rq2 on rq2.id_rab = rh.id_rab and rq2.dat = rh.mdat "
                      "left join rab_prof rp on rp.id = rq2.id_prof "
                      "left join rab_razr rr2 on rr2.id = rq2.id_razr "
                      "order by rr.fam, rr.nam, rr.otc");
        query.bindValue(":d3",ui->dateEditEnd->date());
        if (modelRab->execQuery(query)){
            ui->listViewRab->setModelColumn(5);
        }
        bool ok = ui->listViewRab->model()->rowCount();
        setBlock(!ok);
        if (ok){
            ui->listViewRab->setCurrentIndex(ui->listViewRab->model()->index(0,5));
        } else {
            ui->lineEditWage->clear();
            ui->lineEditExtr->clear();
            ui->lineEditPremQual->clear();
            ui->lineEditPremNorm->clear();
            ui->lineEditTotal->clear();
        }
    } else {
        QMessageBox::critical(this,"Error",queryCre.lastError().text(),QMessageBox::Cancel);
    }
}

void FormCalcWage::updTableData(QModelIndex ind)
{

    int id_rb=ui->listViewRab->model()->data(ui->listViewRab->model()->index(ind.row(),0),Qt::EditRole).toInt();

    ui->lineEditWage->setText(QLocale().toString(ui->listViewRab->model()->data(ui->listViewRab->model()->index(ind.row(),8),Qt::EditRole).toDouble(),'f',2));
    ui->lineEditExtr->setText(QLocale().toString(ui->listViewRab->model()->data(ui->listViewRab->model()->index(ind.row(),9),Qt::EditRole).toDouble(),'f',2));
    ui->lineEditPremQual->setText(QLocale().toString(ui->listViewRab->model()->data(ui->listViewRab->model()->index(ind.row(),10),Qt::EditRole).toDouble(),'f',2));
    ui->lineEditPremNorm->setText(QLocale().toString(ui->listViewRab->model()->data(ui->listViewRab->model()->index(ind.row(),11),Qt::EditRole).toDouble(),'f',2));
    ui->lineEditTotal->setText(QLocale().toString(ui->listViewRab->model()->data(ui->listViewRab->model()->index(ind.row(),12),Qt::EditRole).toDouble(),'f',2));

    QSqlQuery query;
    query.prepare("select dat, fnam, s_kvo, kvo, tarif, zpl, extr, bonus, nrm, zpl+extr+bonus+nrm as vidach "
                  "from ztw_wage "
                  "where id = :id_rab "
                  "order by dat");
    query.bindValue(":id_rab",id_rb);
    if (modelTableData->execQuery(query)){
        modelTableData->setHeaderData(0,Qt::Horizontal,tr("Дата"));
        modelTableData->setHeaderData(1,Qt::Horizontal,tr("Наименование работ"));
        modelTableData->setHeaderData(2,Qt::Horizontal,tr("Общ.к-во"));
        modelTableData->setHeaderData(3,Qt::Horizontal,tr("К-во"));
        modelTableData->setHeaderData(4,Qt::Horizontal,tr("Тариф"));
        modelTableData->setHeaderData(5,Qt::Horizontal,tr("З.пл."));
        modelTableData->setHeaderData(6,Qt::Horizontal,tr("Св.ур."));
        modelTableData->setHeaderData(7,Qt::Horizontal,tr("Прем.кач"));
        modelTableData->setHeaderData(8,Qt::Horizontal,tr("Прем.нор."));
        modelTableData->setHeaderData(9,Qt::Horizontal,tr("К выдаче"));
        ui->tableViewCalc->resizeToContents();
    }
}

void FormCalcWage::saveTabelXlsx()
{
    if (ui->tableViewCalc->model()->rowCount()){
        QString fnam = ui->listViewRab->model()->data(ui->listViewRab->model()->index(ui->listViewRab->currentIndex().row(),5),Qt::EditRole).toString();
        ui->tableViewCalc->save(fnam+tr(" c ")+ui->dateEditBeg->date().toString("dd.MM.yyyy")+tr(" по ")+ui->dateEditEnd->date().toString("dd.MM.yyyy")+".xlsx",2,true,Qt::LandscapeOrientation);
    }
}

void FormCalcWage::tabelShort()
{
    int rabcount=ui->listViewRab->model()->rowCount();
    if (!rabcount){
        return;
    }
    int id_rab;
    QString firstName, lastName, middleName, prof;
    int kvoDay;
    double zp, ex, premk, premn, total;

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

        id_rab=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,0),Qt::EditRole).toInt();
        firstName=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,1),Qt::EditRole).toString();
        lastName=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,2),Qt::EditRole).toString();
        middleName=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,3),Qt::EditRole).toString();
        prof=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,6),Qt::EditRole).toString();

        kvoDay=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,7),Qt::EditRole).toInt();
        zp=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,8),Qt::EditRole).toDouble();
        ex=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,9),Qt::EditRole).toDouble();
        premk=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,10),Qt::EditRole).toDouble();
        premn=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,11),Qt::EditRole).toDouble();
        total=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,12),Qt::EditRole).toDouble();


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
        m++;

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
        xlsx.saveAs(filename);
    }
}

void FormCalcWage::tabel()
{
    int rabcount=ui->listViewRab->model()->rowCount();
    int id_rab;
    bool ok=true;

    QString sError;
    QString firstName, lastName, middleName, prof;
    int kvoDay;
    double zp, dopl, extrtime, premk, premn, total;
    QSqlQuery query;

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
    margins.bottom=0.817361111111111;
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
    ws->setColumnWidth(8,8,9);
    ws->setColumnWidth(9,9,9);
    ws->setColumnWidth(10,10,10);
    ws->setColumnWidth(11,11,10);
    ws->setColumnWidth(12,12,10);
    ws->setColumnWidth(13,13,10);
    ws->setColumnWidth(14,14,10);
    ws->setColumnWidth(15,15,10);
    ws->setColumnWidth(16,16,10);

    for (int i=0; i<rabcount; i++){
        QCoreApplication::processEvents();
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
        dopl=0.0;
        extrtime=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,9),Qt::EditRole).toDouble();
        premk=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,10),Qt::EditRole).toDouble();
        premn=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,11),Qt::EditRole).toDouble();
        total=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,12),Qt::EditRole).toDouble();

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

        if (kvoDay!=0){
            numFormat.setNumberFormat(QString("0.%1").arg((0),2,'d',0,QChar('0')));
            ws->writeNumeric(m,12,total/kvoDay,numFormat);
        } else {
            ws->writeBlank(m,12,strFormat);
        }

        ws->writeBlank(m,13,strFormat);
        ws->writeBlank(m,14,strFormat);
        ws->writeBlank(m,15,strFormat);
        ws->writeBlank(m,16,strFormat);

        m++;

        query.prepare("select zw.fnam, sum(zw.kvo) as kvo, zw.tarif, sum(zw.zpl) as zpl, 0, sum(zw.extr) as extr, "
                      "sum(zw.bonus) as bonus, sum(zw.nrm) as nrm, sum(zw.zpl+zw.extr+zw.bonus+zw.nrm) as total "
                      "from ztw_wage zw where zw.id = :id_rab "
                      "group by zw.id, zw.fnam, zw.tarif "
                      "order by zw.id");
        query.bindValue(":id_rab", id_rab);

        int begm=m;
        if (query.exec()){
            while (query.next()) {

                QString nam=query.value(0).toString();

                ws->writeBlank(m,1,numFormat);

                ws->mergeCells(CellRange(m,2,m,7),numFormat);
                ws->writeString(m,2,nam,strFormat);

                numFormat.setNumberFormat(QString("0.%1").arg((0),3,'d',0,QChar('0')));
                ws->writeNumeric(m,8,query.value(1).toDouble(),numFormat);

                numFormat.setNumberFormat(QString("0.%1").arg((0),2,'d',0,QChar('0')));
                ws->writeNumeric(m,9,query.value(2).toDouble(),numFormat);
                ws->writeNumeric(m,10,query.value(3).toDouble(),numFormat);
                ws->writeNumeric(m,11,query.value(4).toDouble(),numFormat);
                ws->writeBlank(m,12,numFormat);
                ws->writeNumeric(m,13,query.value(5).toDouble(),numFormat);
                ws->writeNumeric(m,14,query.value(6).toDouble(),numFormat);
                ws->writeNumeric(m,15,query.value(7).toDouble(),numFormat);
                ws->writeNumeric(m,16,query.value(8).toDouble(),numFormat);

                m++;
            }
        } else {
            ok=false;
            sError=query.lastError().text();
            break;
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
        m++;
    }

    QString year=QString::number(ui->dateEditBeg->date().year());
    QString month=QDate::longMonthName(ui->dateEditBeg->date().month(),QDate::StandaloneFormat);
    QString dat=month+" "+year+QString(" г.");

    QString headerData=QString("&LРасчет заработной платы за %1&C%2&R&D").arg(dat).arg(tr("ООО СЗСМ"));
    QString footerData=QString("&L%1").arg(tr("Начальник производства __________"));

    ws->setHeaderData(headerData);
    ws->setFooterData(footerData);

    if (!ok) {
        QMessageBox::critical(this,"Error",sError,QMessageBox::Cancel);
    } else {
        QDir dir(QDir::homePath());
        QString filename = QFileDialog::getSaveFileName(nullptr,QString::fromUtf8("Сохранить файл"),
                                                        dir.path()+"/"+title+".xlsx",
                                                        QString::fromUtf8("Documents (*.xlsx)") );
        if (!filename.isEmpty()){
            xlsx.saveAs(filename);
        }
    }
}
