#include "dialogsplit.h"
#include "ui_dialogsplit.h"

DialogSplit::DialogSplit(int id_j, QWidget *parent) :
    QDialog(parent),  id_job(id_j),
    ui(new Ui::DialogSplit)
{
    ui->setupUi(this);
    QSqlQuery query;
    query.prepare("select rr.snam, rn.fnam, rj.dat, rj.kvo, rj.parti "
                  "from rab_job rj "
                  "inner join rab_nams rn on rn.lid = rj.lid "
                  "inner join rab_rab rr on rr.id = rj.id_rb "
                  "where rj.id = :id_job");
    query.bindValue(":id_job",id_job);
    if (query.exec()){
        if (query.next()){
            QDate dat = query.value(2).toDate();
            ui->labelRab->setText(query.value(0).toString());
            ui->labelJob->setText(query.value(1).toString());
            ui->dateEdit->setDate(QDate(dat.year(),dat.month(),1).addMonths(1));
            ui->dateEditFact->setDate(dat);
            ui->lineEditPress->setText(QString::number(query.value(3).toDouble(),'f',4));
            ui->lineEditPack->setText(QString::number(query.value(3).toDouble(),'f',4));
            ui->labelPart->setText(query.value(4).toString());
        }
    } else {
        QMessageBox::critical(this,"Error",query.lastError().text(),QMessageBox::Cancel);
    }
    connect(ui->lineEditOst,SIGNAL(textChanged(QString)),this,SLOT(calcUpk()));
}

DialogSplit::~DialogSplit()
{
    delete ui;
}

void DialogSplit::insNew()
{
    int newId=-1;
    QSqlQuery query;
    query.prepare("insert into rab_job (id_norm, dat, datf, kvo, prem, prim, id_rb, parti, kvo_fact, id_rcp, kfmp, le, lo, id_prov, id_gl, "
                  "liq_glass, dry_rst, lid, kvo_break, press, dens, visc, hmdt, smena, chas_sm, id_prov_part, id_gl_part, prem_nrm, extr_time, "
                  "kvo_break_on_up, kvo_break_on_dry, id_press, id_rub ) "
                  "(select id_norm, dat, datf, kvo, prem, prim, id_rb, parti, kvo_fact, id_rcp, kfmp, le, lo, id_prov, id_gl, "
                  "liq_glass, dry_rst, lid, kvo_break, press, dens, visc, hmdt, smena, chas_sm, id_prov_part, id_gl_part, prem_nrm, extr_time, "
                  "kvo_break_on_up, kvo_break_on_dry, id_press, id_rub from rab_job where id = :id) returning id");
    query.bindValue(":id",id_job);
    if (query.exec()){
        if (query.next()){
            newId=query.value(0).toInt();
            query.clear();
            query.prepare("update rab_job set kvo = :kvo, dat = :dat where id = :id");
            query.bindValue(":kvo",ui->lineEditOst->text().toDouble());
            query.bindValue(":dat",ui->dateEdit->date());
            query.bindValue(":id",newId);
            if (query.exec()){
                query.clear();
                query.prepare("insert into rab_share (id_job, id_rab, koef_prem_kvo, kvo, s_koef, prem) (select :id_job, id_rab, koef_prem_kvo, kvo, s_koef, prem from rab_share where id_job = :id_copy)");
                query.bindValue(":id_job",newId);
                query.bindValue(":id_copy",id_job);
                if (!query.exec()){
                    QMessageBox::critical(this,"Error",query.lastError().text(),QMessageBox::Cancel);
                }
            } else {
                QMessageBox::critical(this,"Error",query.lastError().text(),QMessageBox::Cancel);
            }
        }
    } else {
        QMessageBox::critical(this,"Error",query.lastError().text(),QMessageBox::Cancel);
    }
}

void DialogSplit::updOld()
{
    QSqlQuery query;
    query.prepare("update rab_job set kvo = :kvo where id = :id");
    query.bindValue(":kvo",ui->lineEditPack->text().toDouble());
    query.bindValue(":id",id_job);
    if (!query.exec()){
        QMessageBox::critical(this,"Error",query.lastError().text(),QMessageBox::Cancel);
    }
}

void DialogSplit::calcUpk()
{
    double pack=ui->lineEditPress->text().toDouble()-ui->lineEditOst->text().toDouble();
    ui->lineEditPack->setText(QString::number(pack,'f',4));
}

void DialogSplit::accept()
{
    insNew();
    updOld();
    QDialog::accept();
}
