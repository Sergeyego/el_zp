#include "modelzon.h"

ModelZon::ModelZon(QObject *parent)
    : ModelRo(parent)
{
    refresh();
}

Qt::ItemFlags ModelZon::flags(const QModelIndex &index) const
{
    if (index.column()==1){
        return Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
    }
    return ModelRo::flags(index);
}

QVariant ModelZon::data(const QModelIndex &item, int role) const
{
    if (role==Qt::CheckStateRole && item.column()==1){
        int id = this->data(this->index(item.row(),0),Qt::EditRole).toInt();
        return sel.contains(id) ? Qt::Checked : Qt::Unchecked;
    }
    return ModelRo::data(item,role);
}

bool ModelZon::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role==Qt::CheckStateRole && index.column()==1){
        int id = this->data(this->index(index.row(),0),Qt::EditRole).toInt();
        if (value.toBool() && !sel.contains(id)){
            sel.push_back(id);
        } else {
            sel.removeAll(id);
        }
        return true;
    }
    return ModelRo::setData(index,value,role);
}

void ModelZon::setList(QList<int> list)
{
    beginResetModel();
    sel=list;
    endResetModel();
}

void ModelZon::refresh()
{
    QSqlQuery query;
    query.prepare("select id, nam from rab_zon order by nam");
    if (execQuery(query)){
        setHeaderData(1,Qt::Horizontal,tr("Участки"));
    }
}
