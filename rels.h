#ifndef RELS_H
#define RELS_H

#include <QObject>
#include "db/dbtablemodel.h"

class Rels : public QObject
{
    Q_OBJECT
public:
    static Rels *instance();
    DbSqlRelation *relLiter;
    DbSqlRelation *relZon;
    DbSqlRelation *relList;
    DbSqlRelation *relEd;
    DbSqlRelation *relConn;
    DbSqlRelation *relKaminEmp;
    DbSqlRelation *relMark;
    DbSqlRelation *relDiam;
    DbSqlRelation *relComp;
    DbSqlRelation *relJobNam;
    DbSqlRelation *relRab;
    DbSqlRelation *relPress;
    DbSqlRelation *relPressFlt;
    DbSqlRelation *relMarkType;
    DbSqlRelation *relElPart;

protected:
    explicit Rels(QObject *parent = nullptr);

private:
    static Rels *rels_instance;

};

#endif // RELS_H
