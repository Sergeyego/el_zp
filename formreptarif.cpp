#include "formreptarif.h"
#include "ui_formreptarif.h"

FormRepTarif::FormRepTarif(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormRepTarif)
{
    ui->setupUi(this);
    ui->dateEdit->setDate(QDate::currentDate());
    modelZon = new ModelZon(tr("Участок"),Rels::instance()->relZon->model(),true,this);
    ui->tableViewZon->setModel(modelZon);
    ui->tableViewZon->setColumnWidth(0,250);

    modelTarif = new ModelRo(this);
    ui->tableViewTarif->setModel(modelTarif);

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->pushButtonSave,SIGNAL(clicked(bool)),this,SLOT(save()));
    connect(ui->tableViewZon->horizontalHeader(),SIGNAL(sectionClicked(int)),modelZon,SLOT(checkAll()));
    connect(modelZon,SIGNAL(supd()),this,SLOT(upd()));
    connect(ui->radioButtonTarif,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->radioButtonMark,SIGNAL(clicked(bool)),this,SLOT(upd()));
}

FormRepTarif::~FormRepTarif()
{
    delete ui;
}

void FormRepTarif::upd()
{
    if (sender()==ui->pushButtonUpd){
        Rels::instance()->relZon->refreshModel();
    }
    QSqlQuery query;
    QString order = ui->radioButtonTarif->isChecked() ? "order by rz.nam, rl.naim, t.tarif, e.marka, rn.dim, m.nam, rr.num" : "order by rz.nam, rl.naim, e.marka, rn.dim, m.nam, rr.num";
    query.prepare("select rz.nam, rl.naim, "
                  "case when rn.id_el<>0 then e.marka else '' end || "
                  "case when rn.id_el<>0 and rn.dim<>0 then ' ' else '' end || "
                  "case when rn.dim<>0 then 'ф'||cast(rn.dim as varchar(3)) else '' end || "
                  "case when rn.id_comp<>0 then m.nam else '' end || "
                  "case when rn.id_trf<>0 then rr.num||' разряд' else '' end as snam, "
                  "t.tarif-(t.tarif*t.k_dpl/100/(1+t.k_dpl/100.0))::numeric(18,2), t.k_dpl, (t.tarif*t.k_dpl/100/(1+t.k_dpl/100.0))::numeric(18,2), t.tarif "
                  "from rw_tarif(:d) as t "
                  "inner join rab_nams rn on rn.lid=t.lid "
                  "inner join rab_liter rl on rl.id=rn.id "
                  "inner join rab_zon rz on rz.id=rl.zon "
                  "left join elrtr e on e.id=rn.id_el "
                  "left join matr m on m.id=rn.id_comp "
                  "left join rab_razr rr on rr.id=rn.id_trf "
                  "where t.tarif>0.0 and rl.zon in "+modelZon->getStr()+" "+order);
    query.bindValue(":d",ui->dateEdit->date());
    if (modelTarif->execQuery(query)){
        modelTarif->setHeaderData(0,Qt::Horizontal,tr("Участок"));
        modelTarif->setHeaderData(1,Qt::Horizontal,tr("Наименование работ"));
        modelTarif->setHeaderData(2,Qt::Horizontal,tr("Марки, разряды"));
        modelTarif->setHeaderData(3,Qt::Horizontal,tr("Тариф, руб."));
        modelTarif->setHeaderData(4,Qt::Horizontal,tr("Вред., %"));
        modelTarif->setHeaderData(5,Qt::Horizontal,tr("Вред., руб"));
        modelTarif->setHeaderData(6,Qt::Horizontal,tr("Тариф с \nвред., руб"));
        ui->tableViewTarif->setColumnWidth(0,200);
        ui->tableViewTarif->setColumnWidth(1,450);
        ui->tableViewTarif->setColumnWidth(2,200);
        ui->tableViewTarif->setColumnWidth(3,100);
        ui->tableViewTarif->setColumnWidth(4,100);
        ui->tableViewTarif->setColumnWidth(5,100);
        ui->tableViewTarif->setColumnWidth(6,100);
    }
}

void FormRepTarif::save()
{
    QString title = QString("Тарифы на %1").arg(ui->dateEdit->date().toString("dd.MM.yyyy"));
    if (modelTarif->rowCount()){
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
        strFormat.setVerticalAlignment(Format::AlignTop);
        strFormat.setTextWarp(true);
        Format numFormat;
        numFormat.setBorderStyle(Format::BorderThin);
        numFormat.setFont(defaultFont);
        numFormat.setVerticalAlignment(Format::AlignTop);
        numFormat.setNumberFormat(QString("0.00"));
        Format titleFormat;
        titleFormat.setBorderStyle(Format::BorderNone);
        titleFormat.setFont(titleFont);
        titleFormat.setTextWarp(true);
        titleFormat.setHorizontalAlignment(Format::AlignHCenter);
        titleFormat.setVerticalAlignment(Format::AlignVCenter);

        Format titleFormat2=titleFormat;
        titleFormat2.setFontSize(14);

        Format headerFormat;
        headerFormat.setBorderStyle(Format::BorderThin);
        headerFormat.setFont(titleFont);
        headerFormat.setTextWarp(true);
        headerFormat.setHorizontalAlignment(Format::AlignHCenter);
        headerFormat.setVerticalAlignment(Format::AlignVCenter);

        Format mainTitleFormat;
        mainTitleFormat.setBorderStyle(Format::BorderNone);
        mainTitleFormat.setFont(titleFont);
        mainTitleFormat.setTextWarp(true);
        mainTitleFormat.setHorizontalAlignment(Format::AlignRight);
        mainTitleFormat.setVerticalAlignment(Format::AlignTop);

        Format mainTitleFormat2=mainTitleFormat;
        mainTitleFormat2.setFontBold(false);

        int m=1;
        ws->setColumnWidth(1,1,65);
        ws->setColumnWidth(2,2,25);
        ws->setColumnWidth(3,3,13);
        ws->setColumnWidth(4,4,13);
        ws->setColumnWidth(5,5,13);
        ws->setColumnWidth(6,6,13);

        const int colCount=6;

        ws->mergeCells(CellRange(m,1,m,colCount));
        ws->setRowHeight(m,m,60);
        ws->writeString(m,1,tr("Приложение №1 к Положению по оплате труда, размерах,\nпорядке и условиях применения компенсационных\nи стимулирующих выплат (доплат, надбавок, премий) на ООО «СЗСМ»"),mainTitleFormat);
        m++;
        ws->mergeCells(CellRange(m,1,m,colCount));
        ws->setRowHeight(m,m,20);
        ws->writeString(m,1,tr("УТВЕРЖДЕНО:"),mainTitleFormat);
        m++;
        ws->mergeCells(CellRange(m,1,m,colCount));
        ws->setRowHeight(m,m,40);
        ws->writeString(m,1,tr("Приказом ООО «СЗСМ»\nот ")+ui->dateEdit->date().toString("dd.MM.yyyy")+tr("г №"),mainTitleFormat2);
        m++;
        ws->mergeCells(CellRange(m,1,m,colCount));
        ws->setRowHeight(m,m,20);
        ws->writeString(m,1,tr("Введено в действие с ")+ui->dateEdit->date().toString("dd.MM.yyyy")+tr("г."),mainTitleFormat2);
        m++;
        ws->mergeCells(CellRange(m,1,m,colCount));
        ws->setRowHeight(m,m,60);
        ws->writeString(m,1,tr("Расценки и тарифы на выполнение сдельных и повременных работ\nструктурное подразделение: «Цех по производству сварочных электродов», «Автотранспортный участок»"),titleFormat2);
        m++;

        int i=0;
        QString zon_old, nam_old;
        double tarif_old=-1;
        int beg_nam=m;
        int beg_tarif=m;

        for(i=0;i<(modelTarif->rowCount());i++){
            QString zon=modelTarif->data(modelTarif->index(i,0),Qt::EditRole).toString();
            if (zon_old!=zon){
                tarif_old=-1;

                if ((m-beg_nam)>1){
                    ws->mergeCells(CellRange(beg_nam,1,m-1,1),strFormat);
                }
                if ((m-beg_tarif)>1){
                    for (int j=3; j<=6; j++){
                        ws->mergeCells(CellRange(beg_tarif,j,m-1,j),numFormat);
                    }
                }

                ws->setRowHeight(m,m,20);
                ws->mergeCells(CellRange(m,1,m,colCount));
                ws->writeString(m,1,zon,titleFormat);
                m++;
                ws->setRowHeight(m,m,80);
                ws->writeString(m,1,tr("Наименование работ"),headerFormat);
                ws->writeString(m,2,tr("Марки, разряды"),headerFormat);
                ws->writeString(m,3,tr("Расценка (руб.)"),headerFormat);
                ws->writeString(m,4,tr("Компенсация за работу во  вредных и/или  опасных условиях труда ( в %)"),headerFormat);
                ws->writeString(m,5,tr("Компенсация за работу во вредных  и/или опасных условиях труда (руб.)"),headerFormat);
                ws->writeString(m,6,tr("Расценка с учетом компенсации (руб.)"),headerFormat);
                m++;

                beg_nam=m;
                beg_tarif=m;
            }
            QString nam=modelTarif->data(modelTarif->index(i,1),Qt::EditRole).toString();
            if (nam_old!=nam){
                if ((m-beg_nam)>1){
                    ws->mergeCells(CellRange(beg_nam,1,m-1,1),strFormat);
                }

                beg_nam=m;
                tarif_old=-1;
                ws->writeString(m,1,nam,strFormat);
            }
            QString mark=modelTarif->data(modelTarif->index(i,2),Qt::EditRole).toString();
            double tarif=modelTarif->data(modelTarif->index(i,3),Qt::EditRole).toDouble();
            double proc=modelTarif->data(modelTarif->index(i,4),Qt::EditRole).toDouble();
            double rub=modelTarif->data(modelTarif->index(i,5),Qt::EditRole).toDouble();
            double tot=modelTarif->data(modelTarif->index(i,6),Qt::EditRole).toDouble();

            ws->writeString(m,2,mark,strFormat);
            if (tarif_old!=tarif){
                if ((m-beg_tarif)>1){
                    for (int j=3; j<=6; j++){
                        ws->mergeCells(CellRange(beg_tarif,j,m-1,j),numFormat);
                    }
                }
                ws->writeNumeric(m,3,tarif,numFormat);
                ws->writeNumeric(m,4,proc,numFormat);
                ws->writeNumeric(m,5,rub,numFormat);
                ws->writeNumeric(m,6,tot,numFormat);
                beg_tarif=m;
            }

            zon_old=zon;
            nam_old=nam;
            tarif_old=tarif;
            m++;
        }

        if ((m-beg_nam)>1){
            ws->mergeCells(CellRange(beg_nam,1,m-1,1),strFormat);
        }
        if ((m-beg_tarif)>1){
            for (int j=3; j<=6; j++){
                ws->mergeCells(CellRange(beg_tarif,j,m-1,j),numFormat);
            }
        }

        m++;
        ws->mergeCells(CellRange(m,1,m,colCount),strFormat);
        ws->setRowHeight(m,m,40);
        ws->writeString(m,1,tr("Оплата работ в выходные и праздничные дни, сверхурочные часы производится в соответствии с Трудовым кодексом РФ.\nОплата работ в ночное время производится дополнительно в размере 40% тарифной ставки, оклада."),strFormat);

        QString footerData=QString("&C&12Страница &P");
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
