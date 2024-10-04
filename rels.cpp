#include "rels.h"

Rels* Rels::rels_instance=0;

Rels *Rels::instance()
{
    if (rels_instance==0)
        rels_instance = new Rels();
    return rels_instance;
}


Rels::Rels(QObject *parent) : QObject(parent)
{  
    relProf = new DbSqlRelation("rab_prof","id","nam",this);
    relProf->setSort("rab_prof.nam");

    relRazr = new DbSqlRelation("rab_razr","id","num",this);
    relRazr->setSort("rab_razr.num");

    relLiter = new DbSqlRelation("rab_liter","id","naim",this);
    relLiter->setSort("rab_liter.naim");

    relZon = new DbSqlRelation("rab_zon","id","nam",this);
    relZon->setSort("rab_zon.nam");

    relList = new DbSqlRelation("rab_prem_list","id","nam",this);
    relList->setSort("rab_prem_list.nam");
}

