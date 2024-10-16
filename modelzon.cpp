#include "modelzon.h"

ModelZon::ModelZon(QString name, DbSqlRelation *rel, QObject *parent)
    : nam(name), QSortFilterProxyModel(parent)
{
    is_checked=false;
    checkFlg=false;
    is_inital=rel->isInital();
    connect(rel->model(),SIGNAL(searchFinished(QString)),this,SLOT(updFinished()));
    if (!is_inital){
        rel->refreshModel();
    }
    this->setSourceModel(rel->model());
}

Qt::ItemFlags ModelZon::flags(const QModelIndex &/*index*/) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
}

QVariant ModelZon::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation==Qt::Horizontal && role==Qt::DisplayRole){
        return nam;
    }
    return QSortFilterProxyModel::headerData(section,orientation,role);
}

QVariant ModelZon::data(const QModelIndex &item, int role) const
{
    if (role==Qt::CheckStateRole){
        QModelIndex ind = this->mapToSource(item);
        int id = this->sourceModel()->data(this->sourceModel()->index(ind.row(),0),Qt::EditRole).toInt();
        return sel.contains(id) ? Qt::Checked : Qt::Unchecked;
    }
    return QSortFilterProxyModel::data(item,role);
}

bool ModelZon::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role==Qt::CheckStateRole){
        QModelIndex ind = this->mapToSource(index);
        int id = this->sourceModel()->data(this->sourceModel()->index(ind.row(),0),Qt::EditRole).toInt();
        if (value.toBool()){
            sel.insert(id);
        } else {
            sel.remove(id);
        }
        emit supd();
        return true;
    }
    return QSortFilterProxyModel::setData(index,value,role);
}

bool ModelZon::filterAcceptsColumn(int source_column, const QModelIndex &/*source_parent*/) const
{
    return source_column==1;
}

void ModelZon::setSel(QSet<int> set)
{
    beginResetModel();
    sel=set;
    endResetModel();
    emit supd();
}

QString ModelZon::getStr()
{
    QString str="(0";
    for (int val : sel){
        if (!str.isEmpty()){
            str+=", ";
        }
        str+=QString::number(val);
    }
    str+=")";
    return str;
}

QSet<int> ModelZon::getSel()
{
    return sel;
}

void ModelZon::checkAll()
{
    checkAll(!is_checked);
}

void ModelZon::checkAll(bool b)
{
    if (is_inital && this->sourceModel()->rowCount()){
        beginResetModel();
        if (!b){
            sel.clear();
        } else {
            for (int i=0; i<this->sourceModel()->rowCount();i++){
                sel.insert(this->sourceModel()->data(this->sourceModel()->index(i,0),Qt::EditRole).toInt());
            }
        }
        endResetModel();
        emit supd();
        is_checked=!is_checked;
    } else {
        checkFlg=true;
    }
}

void ModelZon::updFinished()
{
    is_inital=true;
    if (checkFlg){
        for (int i=0; i<this->sourceModel()->rowCount();i++){
            sel.insert(this->sourceModel()->data(this->sourceModel()->index(i,0),Qt::EditRole).toInt());
        }
        is_checked=!is_checked;
        checkFlg=false;
    }
}
