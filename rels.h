#ifndef RELS_H
#define RELS_H

#include <QObject>
#include "db/dbtablemodel.h"

class Rels : public QObject
{
    Q_OBJECT
public:
    static Rels *instance();
    DbSqlRelation *relProf;
    DbSqlRelation *relRazr;
    DbSqlRelation *relLiter;
    DbSqlRelation *relZon;
    DbSqlRelation *relList;

protected:
    explicit Rels(QObject *parent = 0);

private:
    static Rels *rels_instance;

};

#endif // RELS_H
