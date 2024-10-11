#ifndef FORMREPNORM_H
#define FORMREPNORM_H

#include <QWidget>
#include <QtGui>
#include <QtSql>
#include <QMessageBox>
#include <QSortFilterProxyModel>
#include "zonwidget.h"
#include "db/dbtablemodel.h"

namespace Ui {
class FormRepNorm;
}

class JobSqlModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    explicit JobSqlModel(QObject *parent = nullptr);
    QVariant data(const QModelIndex &index,int role = Qt::DisplayRole) const;
    void refresh(bool emp, QString zonSuf, bool fsm, QDate dbeg, QDate dend);
    QHash <int, QString> hsm;

private:
    QDate begdate;
    QDate enddate;
    bool fSm;
signals:
    void sigUpd();
};

class FormRepNorm : public QWidget
{
    Q_OBJECT
    
public:
    explicit FormRepNorm(QWidget *parent = 0);
    ~FormRepNorm();
    
private:
    Ui::FormRepNorm *ui;
    ZonWidget *zonWidget;
    JobSqlModel *jobmodel;
    QSortFilterProxyModel *proxyJobModel;
    QString getProf(int id_rb, QDate date);
    QHash <int, QString> hed;
    QString signRep;
    bool getTotalVip(double &vip, int &d, double &kvo, const int id_rb);

private slots:
    void upd();
    void saveXls();
    void saveXlsPer();
    void updTotal(QModelIndex index);
    void goRep();
};

#endif // FORMREPNORM_H
