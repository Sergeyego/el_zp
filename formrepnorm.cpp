#include "formrepnorm.h"
#include "ui_formrepnorm.h"

FormRepNorm::FormRepNorm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormRepNorm)
{
    ui->setupUi(this);

    modelZon = new ModelZon(tr("Участки"),Rels::instance()->relZon->model(),false, this);
    QSet<int> set;
    set<<2<<6;
    modelZon->setSel(set);
    ui->tableViewZon->setModel(modelZon);
    ui->tableViewZon->setColumnWidth(0,270);

    QSqlQuery query;
    query.prepare("select sign_rep_el from hoz where id=11");
    if (query.exec()){
        while (query.next()) {
            signRep=query.value(0).toString();
        }
    } else {
        QMessageBox::critical(this,"Error",query.lastError().text(),QMessageBox::Cancel);
    }
    QSqlQuery queryed;
    queryed.prepare("select id, nam from wire_rab_ed order by nam");
    if (queryed.exec()){
        while (queryed.next()){
            hed.insert(queryed.value(0).toInt(),queryed.value(1).toString());
        }
    } else {
        QMessageBox::critical(NULL,tr("Ошибка"),queryed.lastError().text(),QMessageBox::Cancel);
    }

    ui->dateBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    ui->dateEnd->setDate(QDate::currentDate());

    jobmodel = new JobSqlModel(this);


    proxyJobModel = new QSortFilterProxyModel(this);
    proxyJobModel->setSourceModel(jobmodel);

    ui->jobView->setModel(proxyJobModel);

    connect(ui->cmdRefresh, SIGNAL(clicked()),this,SLOT(upd()));
    connect(ui->radioButtonEmp,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->radioButtonLine,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->radioButtonSm,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->cmdOtchPer,SIGNAL(clicked()),this,SLOT(goRep()));
    connect(modelZon,SIGNAL(supd()),this,SLOT(upd()));
    connect(ui->jobView->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updTotal(QModelIndex)));
    connect(ui->tableViewZon->horizontalHeader(),SIGNAL(sectionClicked(int)),modelZon,SLOT(checkAll()));
}

FormRepNorm::~FormRepNorm()
{
    delete ui;
}

QString FormRepNorm::getProf(int id_rb, QDate date)
{
    QSqlQuery query;
    QString prof;
    query.prepare("select rr.snam, kj.nam "
                  "from rab_rab rr "
                  "left join kamin_get_empl(:date,:date) as k on k.id_empl = rr.id_kamin "
                  "left join kamin_job kj on kj.id = k.id_job "
                  "where rr.id = :id_rb");
    query.bindValue(":date",date);
    query.bindValue(":id_rb",id_rb);
    if (!query.exec()){
        QMessageBox::critical(this,"Error",query.lastError().text(),QMessageBox::Cancel);
    } else {
        if (query.next()){
            prof=query.value(1).toString();
        }
    }
    return prof;
}

bool FormRepNorm::getTotalVip(double &vip, int &d, double &kvo, const int id_rb)
{
    QSqlQuery query;
    QString qu="select sum(j.kvo*100/get_norm_el(j.id, j.chas_sm))/count(distinct j.datf), (select count(distinct datf) from rab_job where datf between :d1 and :d2 and id_rb = :id_rb) , sum(j.kvo_fact) "
               "from rab_job as j "
               "inner join rab_nams as n on j.lid=n.lid "
               "inner join rab_liter as lt on n.id=lt.id "
               "where lt.id_ed=1 and j.datf between :d1 and :d2 and j.id_rb = :id_rb ";
    qu+=" and lt.zon in "+modelZon->getStr();
    query.prepare(qu);
    query.bindValue(":d1",ui->dateBeg->date());
    query.bindValue(":d2",ui->dateEnd->date());
    query.bindValue(":id_rb",id_rb);
    bool ok=query.exec();
    if (!ok){
        QMessageBox::critical(this,"Error",query.lastError().text(),QMessageBox::Cancel);
    } else {
        while (query.next()){
            vip=query.value(0).toDouble();
            d=query.value(1).toInt();
            kvo=query.value(2).toDouble();
        }
    }
    return ok;
}

void FormRepNorm::saveXls()
{
    int i=0;
    int id_rb=0, id_rb_old=-1;
    if (jobmodel->rowCount()){

        QString title=QString("Выполнение норм с %1 по %2").arg(ui->dateBeg->date().toString("dd.MM.yy")).arg(ui->dateEnd->date().toString("dd.MM.yy"));

        Document xlsx;
        Worksheet *ws=xlsx.currentWorksheet();
        XlsxPageSetup pageSetup;
        pageSetup.fitToPage=true;
        pageSetup.fitToWidth=1;
        pageSetup.fitToHeight=0;
        pageSetup.orientation=XlsxPageSetup::portrait;
        ws->setPageSetup(pageSetup);

        XlsxPageMargins margins=ws->pageMargins();
        margins.bottom=0.817361111111111;
        ws->setPageMargins(margins);

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
        ws->setColumnWidth(1,1,10);
        ws->setColumnWidth(2,2,18);
        ws->setColumnWidth(3,3,62);
        ws->setColumnWidth(4,4,11);
        ws->setColumnWidth(5,5,10);
        ws->setColumnWidth(6,6,10);
        ws->setColumnWidth(7,7,13);
        ws->setColumnWidth(8,8,13);
        ws->setColumnWidth(9,9,13);

        QDate date_old;
        int m_date=3;
        double sumVip=0;

        for(i=0;i<(jobmodel->rowCount());i++){
            QString emp=jobmodel->data(jobmodel->index(i,4),Qt::EditRole).toString();
            QDate date=jobmodel->data(jobmodel->index(i,1),Qt::EditRole).toDate();
            QString line=jobmodel->data(jobmodel->index(i,3),Qt::EditRole).toString();
            QString vid=jobmodel->data(jobmodel->index(i,6),Qt::EditRole).toString();
            double otr=jobmodel->data(jobmodel->index(i,5),Qt::EditRole).toDouble();
            double norm11=jobmodel->data(jobmodel->index(i,7),Qt::EditRole).toDouble();
            double norm=jobmodel->data(jobmodel->index(i,8),Qt::EditRole).toDouble();
            double fact=jobmodel->data(jobmodel->index(i,9),Qt::EditRole).toDouble();
            double vip=jobmodel->data(jobmodel->index(i,10),Qt::EditRole).toDouble();
            int id_ed=jobmodel->data(jobmodel->index(i,13),Qt::EditRole).toInt();

            id_rb=jobmodel->data(jobmodel->index(i,12),Qt::EditRole).toInt();

            if (id_ed!=1){
                vid+=" - "+QString::number(fact)+" "+hed.value(id_ed);
            }

            if (id_rb_old!=id_rb){
                if (m>1){
                    numFormat.setNumberFormat(QString("0.%1").arg((0),1,'d',0,QChar('0')));
                    ws->mergeCells(CellRange(m_date,9,m-1,9),numFormat);
                    ws->writeNumeric(m_date,9,sumVip,numFormat);
                    sumVip=0;

                    ws->insertRowBreak(m);
                    ws->setRowHeight(m,m,20);
                    ws->writeString(m,1,QString("Итого"),strFormat);
                    double vip, kvo;
                    int d;
                    getTotalVip(vip,d,kvo,id_rb_old);
                    numFormat.setNumberFormat(QString("0.%1").arg((0),1,'d',0,QChar('0')));
                    ws->writeNumeric(m,9,vip,numFormat);
                    m++;
                }
                ws->setRowHeight(m,m+1,40);
                ws->mergeCells(CellRange(m,1,m,9),titleFormat);
                ws->writeString(m,1,title+"\n"+emp+" "+getProf(id_rb,ui->dateEnd->date()) ,titleFormat);
                m++;
                ws->writeString(m,1,"Дата",headerFormat);
                ws->writeString(m,2,"Оборудование",headerFormat);
                ws->writeString(m,3,"Вид работ",headerFormat);
                ws->writeString(m,4,"Отработал часов",headerFormat);
                ws->writeString(m,5,"Норма за 11 ч, т",headerFormat);
                ws->writeString(m,6,"Норма за отраб. Часы, т",headerFormat);
                ws->writeString(m,7,"Фактическое выполнение, т",headerFormat);
                ws->writeString(m,8,"% Выполнения",headerFormat);
                ws->writeString(m,9,"% Выполнения за смену",headerFormat);
                m++;
                m_date=m;
            }
            ws->setRowHeight(m,m,20);
            ws->writeString(m,1,date.toString("dd.MM.yy"),strFormat);
            ws->writeString(m,2,line,strFormat);
            ws->writeString(m,3,vid,strFormat);

            numFormat.setNumberFormat("0");
            ws->writeNumeric(m,4,otr,numFormat);

            numFormat.setNumberFormat(QString("0.%1").arg((0),3,'d',0,QChar('0')));
            ws->writeNumeric(m,5,norm11,numFormat);

            ws->writeNumeric(m,6,norm,numFormat);

            if (id_ed==1){
                ws->writeNumeric(m,7,fact,numFormat);
            } else {
                ws->writeBlank(m,7,numFormat);
            }

            numFormat.setNumberFormat(QString("0.%1").arg((0),1,'d',0,QChar('0')));
            ws->writeNumeric(m,8,vip,numFormat);

            if (date_old!=date || id_rb_old!=id_rb){
                if (m>1){
                    numFormat.setNumberFormat(QString("0.%1").arg((0),1,'d',0,QChar('0')));
                    ws->mergeCells(CellRange(m_date,9,m-1,9),numFormat);
                    ws->writeNumeric(m_date,9,sumVip,numFormat);
                    sumVip=0;
                    m_date=m;
                }
            }

            sumVip+=vip;
            id_rb_old=id_rb;
            date_old=date;
            m++;
        }

        numFormat.setNumberFormat(QString("0.%1").arg((0),1,'d',0,QChar('0')));
        ws->mergeCells(CellRange(m_date,9,m-1,9),numFormat);
        ws->writeNumeric(m_date,9,sumVip,numFormat);
        ws->insertRowBreak(m);
        ws->setRowHeight(m,m,20);
        ws->writeString(m,1,QString("Итого"),strFormat);
        double vip, kvo;
        int d;
        getTotalVip(vip,d,kvo,id_rb_old);
        numFormat.setNumberFormat(QString("0.%1").arg((0),1,'d',0,QChar('0')));
        ws->writeNumeric(m,9,vip,numFormat);

        QString footerData=QString("&L%1").arg(signRep);
        ws->setFooterData(footerData);

        QDir dir(QDir::homePath());
        QString filename = QFileDialog::getSaveFileName(nullptr,QString::fromUtf8("Сохранить файл"),
                                                        dir.path()+"/"+title+".xlsx",
                                                        QString::fromUtf8("Documents (*.xlsx)") );
        if (!filename.isEmpty()){
            xlsx.saveAs(filename);
        }
    }
}

void FormRepNorm::saveXlsPer()
{
    int i=0;
    int id_rb=0, id_rb_old=-1;
    if (jobmodel->rowCount()){

        QString period, af11, fperiod;
        if (ui->dateBeg->date()==ui->dateEnd->date()){
            period="за "+ui->dateBeg->date().toString("d MMMM yyyy")+"г.";
            fperiod=ui->dateBeg->date().toString("d MMMM yyyy")+"г.";
            af11=" за 11ч.";
        } else {
            period="с "+ui->dateBeg->date().toString("dd.MM.yy")+" по "+ui->dateEnd->date().toString("dd.MM.yy");
            fperiod=period;
        }

        QString title=QString("Отчет работы структурного подразделения \"Цех по производству сварочных электродов\" ")+period;

        Document xlsx;
        Worksheet *ws=xlsx.currentWorksheet();
        XlsxPageSetup pageSetup;
        pageSetup.fitToPage=true;
        pageSetup.fitToWidth=1;
        if (ui->dateBeg->date()==ui->dateEnd->date()){
            pageSetup.fitToHeight=1;
        } else {
            pageSetup.fitToHeight=0;
        }
        pageSetup.orientation=XlsxPageSetup::landscape;
        ws->setPageSetup(pageSetup);

        XlsxPageMargins margins=ws->pageMargins();
        margins.bottom=0.970138888888889;
        ws->setPageMargins(margins);

        QFont defaultFont("Arial", 10);
        QFont titleFont("Arial", 10);
        titleFont.setBold(true);

        Format strFormat;
        strFormat.setBorderStyle(Format::BorderThin);
        strFormat.setFont(defaultFont);
        strFormat.setVerticalAlignment(Format::AlignVCenter);
        Format numFormat;
        numFormat.setBorderStyle(Format::BorderThin);
        numFormat.setFont(defaultFont);
        numFormat.setVerticalAlignment(Format::AlignVCenter);
        Format sumFormat=numFormat;
        sumFormat.setFont(titleFont);

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

        int m=1, m_beg=1, m_line=1, m_otr=1;
        QString smena, smena_old;
        QString line, line_old;
        QDate date, date_old;
        int otr=0, otr_old=0;

        ws->setColumnWidth(1,1,16);
        ws->setColumnWidth(2,2,18);
        ws->setColumnWidth(3,3,62);
        ws->setColumnWidth(4,4,11);
        ws->setColumnWidth(5,5,10);
        ws->setColumnWidth(6,6,13);
        ws->setColumnWidth(7,7,13);
        ws->setColumnWidth(8,8,13);
        ws->setColumnWidth(9,9,13);
        ws->setColumnWidth(10,10,15);

        ws->mergeCells(CellRange(m,1,m,10),titleFormat);
        ws->setRowHeight(m,m,50);
        ws->writeString(m,1,title,titleFormat);
        m++;

        for(i=0;i<(jobmodel->rowCount());i++){
            QString emp=jobmodel->data(jobmodel->index(i,4),Qt::EditRole).toString();
            date=jobmodel->data(jobmodel->index(i,1),Qt::EditRole).toDate();
            smena=jobmodel->data(jobmodel->index(i,2),Qt::EditRole).toString();
            line=jobmodel->data(jobmodel->index(i,3),Qt::EditRole).toString();
            QString vid=jobmodel->data(jobmodel->index(i,6),Qt::EditRole).toString();
            otr=jobmodel->data(jobmodel->index(i,5),Qt::EditRole).toInt();
            double norm11=jobmodel->data(jobmodel->index(i,7),Qt::EditRole).toDouble();
            //double norm=jobmodel->data(jobmodel->index(i,8),Qt::EditRole).toDouble();
            double fact=jobmodel->data(jobmodel->index(i,9),Qt::EditRole).toDouble();
            double vip=jobmodel->data(jobmodel->index(i,10),Qt::EditRole).toDouble();
            QString prim=jobmodel->data(jobmodel->index(i,11),Qt::EditRole).toString();
            id_rb=jobmodel->data(jobmodel->index(i,12),Qt::EditRole).toInt();
            int id_ed=jobmodel->data(jobmodel->index(i,13),Qt::EditRole).toInt();

            vid.replace(", за тонну","");
            vid.replace(", за час","");

            if (id_ed!=1){
                vid+=" - "+QString::number(fact)+" "+hed.value(id_ed);
            }

            if (smena_old!=smena){
                if (m!=2){
                    ws->mergeCells(CellRange(m_line,2,m-1,2),strFormat);
                    ws->writeString(m_line,2,line_old,strFormat);//Оборуд
                    ws->mergeCells(CellRange(m_beg,1,m-1,1),strFormat);
                    numFormat.setNumberFormat("0");
                    ws->mergeCells(CellRange(m_otr,4,m-1,4),numFormat);
                    ws->mergeCells(CellRange(m_beg,7,m-1,7),sumFormat);
                    ws->mergeCells(CellRange(m_beg,9,m-1,9),sumFormat);
                    double total=0, kvo=0;
                    int day=0;
                    getTotalVip(total,day,kvo,id_rb_old);
                    sumFormat.setNumberFormat(QString("0.%1").arg((0),3,'d',0,QChar('0')));
                    ws->writeNumeric(m_beg,7,kvo,sumFormat);//выполнение итого
                    sumFormat.setNumberFormat(QString("0.%1").arg((0),1,'d',0,QChar('0')));
                    ws->writeNumeric(m_beg,9,total,sumFormat);//общий процент
                }
                ws->mergeCells(CellRange(m,1,m,10),titleFormat);
                ws->setRowHeight(m,m,50);
                ws->writeString(m,1,smena,titleFormat);
                m++;

                ws->setRowHeight(m,m,50);
                ws->writeString(m,1,"ФИО работника",headerFormat);
                ws->writeString(m,2,"Пресс",headerFormat);
                ws->writeString(m,3,"Вид работ",headerFormat);
                ws->writeString(m,4,"Кол-во отработанных часов",headerFormat);
                ws->writeString(m,5,"Норма за 11 ч (т)",headerFormat);
                ws->writeString(m,6,"Фактическое выполнение по маркам"+af11+" (т)",headerFormat);
                ws->writeString(m,7,"Фактическое выполнение"+af11+" (т)",headerFormat);
                ws->writeString(m,8,"% выполнения по маркам"+af11,headerFormat);
                ws->writeString(m,9,"Общий % выполнения"+af11,headerFormat);
                ws->writeString(m,10,"Примечание",headerFormat);
                m++;
                m_beg=m, m_line=m, m_otr=m;
            }

            if (ui->dateBeg->date()!=ui->dateEnd->date()){
                vid=date.toString("dd.MM.yy")+" "+vid;
            }

            if (line_old!=line || smena_old!=smena){
                ws->mergeCells(CellRange(m_line,2,m-1,2),strFormat);
                ws->writeString(m_line,2,line_old,strFormat);//Оборуд
                m_line=m;
            }

            if (id_rb_old!=id_rb|| smena_old!=smena){
                ws->mergeCells(CellRange(m_beg,1,m-1,1),strFormat);
                ws->mergeCells(CellRange(m_beg,7,m-1,7),sumFormat);
                ws->mergeCells(CellRange(m_beg,9,m-1,9),sumFormat);
                double total=0, kvo=0;
                int day=0;
                getTotalVip(total,day,kvo,id_rb_old);
                sumFormat.setNumberFormat(QString("0.%1").arg((0),1,'d',0,QChar('0')));
                ws->writeNumeric(m_beg,9,total,sumFormat);//общий процент
                sumFormat.setNumberFormat(QString("0.%1").arg((0),3,'d',0,QChar('0')));
                ws->writeNumeric(m_beg,7,kvo,sumFormat);//выполнение итого
                m_beg=m;
            }

            if (date_old!=date || id_rb_old!=id_rb || otr_old!=otr || smena_old!=smena){
                numFormat.setNumberFormat("0");
                ws->mergeCells(CellRange(m_otr,4,m-1,4),numFormat);
                m_otr=m;
            }

            ws->setRowHeight(m,m,20);
            ws->writeString(m,1,emp,strFormat);//фио
            ws->writeString(m,3,vid,strFormat);//вид работ
            numFormat.setNumberFormat("0");
            ws->writeNumeric(m,4,otr,numFormat);//к-во часов
            ws->writeString(m,5,smena,strFormat);//смена
            numFormat.setNumberFormat(QString("0.%1").arg((0),3,'d',0,QChar('0')));
            ws->writeNumeric(m,5,norm11,numFormat);//норма за 11 часов
            if (id_ed==1){
                ws->writeNumeric(m,6,fact,numFormat);//факт. выполнение
            } else {
                ws->writeBlank(m,6,numFormat);
            }
            numFormat.setNumberFormat(QString("0.%1").arg((0),1,'d',0,QChar('0')));
            ws->writeNumeric(m,8,vip,numFormat);//выполнение по маркам
            ws->writeString(m,10,prim,strFormat);//примечание

            smena_old=smena;
            id_rb_old=id_rb;
            line_old=line;
            date_old=date;
            otr_old=otr;
            m++;
        }

        ws->mergeCells(CellRange(m_line,2,m-1,2),strFormat);
        ws->writeString(m_line,2,line_old,strFormat);//Оборуд
        ws->mergeCells(CellRange(m_beg,1,m-1,1),strFormat);
        numFormat.setNumberFormat("0");
        ws->mergeCells(CellRange(m_otr,4,m-1,4),numFormat);
        ws->mergeCells(CellRange(m_beg,7,m-1,7),sumFormat);
        ws->mergeCells(CellRange(m_beg,9,m-1,9),sumFormat);
        double total=0, kvo=0;
        int day=0;
        getTotalVip(total,day,kvo,id_rb_old);
        sumFormat.setNumberFormat(QString("0.%1").arg((0),3,'d',0,QChar('0')));
        ws->writeNumeric(m_beg,7,kvo,sumFormat);//выполнение итого
        sumFormat.setNumberFormat(QString("0.%1").arg((0),1,'d',0,QChar('0')));
        ws->writeNumeric(m_beg,9,total,sumFormat);//общий процент

        QString footerData=QString("&L%1").arg(signRep);
        ws->setFooterData(footerData);

        QString name;
        QSet<int> sel=modelZon->getSel();
        if (sel.contains(2) && !sel.contains(6)){
            name=QString("прессовщики ")+fperiod;
        } else if (sel.contains(6) && !sel.contains(2)){
            name=QString("упаковщики ")+fperiod;
        } else {
            name=QString("Отчет работы структурного подразделения цех по производству сварочных электродов ")+period;
        }

        QDir dir(QDir::homePath());
        QString filename = QFileDialog::getSaveFileName(nullptr,QString::fromUtf8("Сохранить файл"),
                                                        dir.path()+"/"+name+".xlsx",
                                                        QString::fromUtf8("Documents (*.xlsx)") );
        if (!filename.isEmpty()){
            xlsx.saveAs(filename);
        }
    }
}

void FormRepNorm::updTotal(QModelIndex index)
{
    int id_rb=ui->jobView->model()->data(ui->jobView->model()->index(index.row(),12),Qt::EditRole).toInt();
    double vip=0, kvo=0;
    int d=0;
    getTotalVip(vip,d,kvo,id_rb);
    ui->lineEditVip->setText(QLocale().toString(vip,'f',2));
    ui->lineEditDay->setText(QString::number(d));
}

void FormRepNorm::goRep()
{
    if (!ui->radioButtonSm->isChecked()){
        if (ui->radioButtonLine->isChecked()){
            saveXlsPer();
        } else {
            saveXls();
        }
    } else {
        ui->jobView->save("");
    }
}

void FormRepNorm::upd()
{
    if (sender()==ui->cmdRefresh){
        Rels::instance()->relZon->refreshModel();
    }
    bool by_line=ui->radioButtonLine->isChecked();
    jobmodel->refresh(by_line,modelZon->getStr(),ui->radioButtonSm->isChecked(),ui->dateBeg->date(),ui->dateEnd->date());
    ui->jobView->setColumnHidden(0,true);
    ui->jobView->setColumnHidden(12,true);
    ui->jobView->setColumnHidden(13,true);
    ui->jobView->resizeToContents();
}

JobSqlModel::JobSqlModel(QObject *parent) :
    QSqlQueryModel(parent)
{
    QSqlQuery query;
    query.prepare("select id, fname from wire_smena order by fname");
    if (query.exec()){
        while (query.next()){
            hsm.insert(query.value(0).toInt(),query.value(1).toString());
        }
    } else {
        QMessageBox::critical(nullptr,tr("Ошибка"),query.lastError().text(),QMessageBox::Cancel);
    }
}

void JobSqlModel::refresh(bool emp, QString zonSuf, bool fsm, QDate dbeg, QDate dend)
{
    begdate=dbeg;
    enddate=dend;
    fSm=fsm;
    QString order;
    if (!fsm){
        order= emp? " order by sm, p.nam, r.snam, j.datf" : " order by r.snam, j.datf, sm, p.id";
    } else {
        order=" group by j.id_rb, r.snam order by r.snam";
    }

    QString qu;
    if (!fsm){
        qu="select j.id, j.datf, "
           "(CASE WHEN exists "
           "(select rj.id from rab_job as rj inner join rab_nams as rn on rj.lid=rn.lid inner join rab_liter as rl on rl.id=rn.id where rj.datf=j.datf and rj.id_rb=j.id_rb and rl.zon=19) "
           "THEN 1 ELSE 0 END) as sm, "
           "p.nam, r.snam, j.chas_sm, right(n.fnam, length(n.fnam)-COALESCE(length(lt.liter),0)), "
           "get_norm_el(j.id, 11) as norms11, "
           "get_norm_el(j.id, j.chas_sm) as norm, "
           "j.kvo, "
           "j.kvo*100/get_norm_el(j.id, j.chas_sm) as vip, "
           "j.prim, "
           "j.id_rb, lt.id_ed "
           "from rab_job as j "
           "inner join pres as p on j.id_press=p.id "
           "inner join rab_rab as r on j.id_rb=r.id "
           "inner join rab_nams as n on j.lid=n.lid "
           "inner join rab_liter as lt on n.id=lt.id "
           "where j.datf between '"+begdate.toString("yyyy-MM-dd")+"' and '"+enddate.toString("yyyy-MM-dd")+"'";
    } else {
        qu="select NULL, NULL, NULL, NULL, r.snam, NULL, NULL, NULL, NULL, NULL, "
           "sum(j.kvo*100/get_norm_el(j.id, j.chas_sm) ) / count(distinct j.datf) as vip, "
           "NULL, j.id_rb, NULL "
           "from rab_job as j "
           "inner join rab_rab as r on j.id_rb=r.id "
           "inner join rab_nams as n on j.lid=n.lid "
           "inner join rab_liter as lt on n.id=lt.id "
           "where j.datf between '"+begdate.toString("yyyy-MM-dd")+"' and '"+enddate.toString("yyyy-MM-dd")+"' and lt.id_ed=1";
    }
    qu+=" and lt.zon in "+zonSuf;
    setQuery(qu+order);
    if (lastError().isValid()){
        QMessageBox::critical(nullptr,tr("Ошибка"),lastError().text(), QMessageBox::Cancel);
    } else {
        setHeaderData(1, Qt::Horizontal,tr("Дата"));
        setHeaderData(2, Qt::Horizontal,tr("Смена"));
        setHeaderData(3, Qt::Horizontal,tr("Пресс"));
        setHeaderData(4, Qt::Horizontal,tr("Бригадир"));
        setHeaderData(5, Qt::Horizontal,tr("Отр,ч."));
        setHeaderData(6, Qt::Horizontal,tr("Задание"));
        setHeaderData(7, Qt::Horizontal,tr("Нор.11"));
        setHeaderData(8, Qt::Horizontal,tr("Норма"));
        setHeaderData(9, Qt::Horizontal,tr("Вып."));
        setHeaderData(10, Qt::Horizontal,tr("% вып."));
        setHeaderData(11, Qt::Horizontal,tr("Примечание"));
        emit sigUpd();
    }
}

QVariant JobSqlModel::data(const QModelIndex &index,int role) const
{
    QVariant value = QSqlQueryModel::data(index, role);
    switch (role) {
    case Qt::DisplayRole: // Данные для отображения
    {
        if (index.column()==1) {
            return value.isNull()? QVariant() : QSqlQueryModel::data(index,Qt::EditRole).toDate().toString("dd.MM.yy");
        }
        if (index.column()==2) {
            return value.isNull()? QVariant() : hsm.value(value.toInt());
        }
        if (index.column()>=7 && index.column()<=9) {
            return value.isNull() ? QVariant() : QString("%1").arg(QSqlQueryModel::data(index,Qt::EditRole).toDouble(),0,'f',3);
        }
        if (index.column()==10) {
            return value.isNull() ? QString("-") : QString("%1").arg(value.toDouble(),0,'f',2);
        } else return value;
        break;
    }
    case Qt::EditRole: // Данные для редактирования
    {
        if (index.column()==2) {
            return value.isNull()? QVariant() : hsm.value(value.toInt());
        } else return value;
        break;
    }
    case Qt::TextAlignmentRole: // Выравнивание
        if(index.column() == 5 || (index.column()>=7 && index.column()<=10) )
            return int(Qt::AlignRight | Qt::AlignVCenter);
        else return int(Qt::AlignLeft | Qt::AlignVCenter);
        break;
    }
    return value;
}
