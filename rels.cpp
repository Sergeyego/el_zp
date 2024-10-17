#include "rels.h"

Rels* Rels::rels_instance=nullptr;

Rels *Rels::instance()
{
    if (rels_instance==nullptr)
        rels_instance = new Rels();
    return rels_instance;
}


Rels::Rels(QObject *parent) : QObject(parent)
{  
    relLiter = new DbSqlRelation("rab_liter","id","naim",this);
    relLiter->setSort("rab_liter.naim");

    relZon = new DbSqlRelation("rab_zon","id","nam",this);
    relZon->setSort("rab_zon.nam");
    relZon->setEditable(true);
    relZon->model()->setAsync(false);

    relList = new DbSqlRelation("rab_prem_list","id","nam",this);
    relList->setSort("rab_prem_list.nam");

    relEd = new DbSqlRelation("wire_rab_ed","id","nam",this);
    relEd->setSort("wire_rab_ed.nam");
    relEd->setEditable(true);

    relConn = new DbSqlRelation("rab_conn","id","tip_conn",this);
    relConn->setSort("rab_conn.tip_conn");

    relKaminEmp = new DbSqlRelation("kamin_empl","id","str",this);
    relKaminEmp->setSort("kamin_empl.str");
    relKaminEmp->model()->setAsync(false);

    relMark = new DbSqlRelation("elrtr","id","marka",this);
    relMark->setFilter("elrtr.id>0");
    relMark->setSort("elrtr.marka");

    relDiam = new DbSqlRelation("diam","id","sdim",this);
    relDiam->setFilter("diam.is_el=1");
    relDiam->setSort("diam.diam");

    relComp = new DbSqlRelation("matr","id","nam",this);
    relComp->setSort("matr.nam");

    relJobNam = new DbSqlRelation("rab_nams","lid","fnam",this);
    relJobNam->setSort("rab_nams.fnam");

    relRab = new DbSqlRelation("rab_rab","id","snam",this);
    relRab->setSort("rab_rab.snam");

    relPress = new DbSqlRelation("pres","id","nam",this);
    relPress->setSort("pres.nam");
}

