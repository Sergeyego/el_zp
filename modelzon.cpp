#include "modelzon.h"

ModelZon::ModelZon(QString name, QAbstractItemModel *srcModel, bool chkAll, QObject *parent)
    : QSortFilterProxyModel(parent), nam(name)
{
    is_checked=chkAll;
    checkFlg=chkAll;
    viewCol=1;
    this->setSourceModel(srcModel);
    DbSqlLikeModel *likeModel = qobject_cast <DbSqlLikeModel *>(srcModel);
    if (likeModel){
        connect(likeModel,SIGNAL(searchFinished(QString)),this,SLOT(updFinished()));
        if (!likeModel->getRelation()->isInital()){
            likeModel->getRelation()->refreshModel();
        } else if (likeModel->rowCount()){
            updFinished();
        }
    } else {
        updFinished();
    }
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
        emit dataChanged(this->index(0,0),this->index(rowCount()-1,0));
        emit supd();
        return true;
    }
    return QSortFilterProxyModel::setData(index,value,role);
}

bool ModelZon::filterAcceptsColumn(int source_column, const QModelIndex &/*source_parent*/) const
{
    return source_column==viewCol;
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

void ModelZon::setViewColumn(int c)
{
    viewCol=c;
}

void ModelZon::checkAll()
{
    checkAll(!is_checked);
}

void ModelZon::checkAll(bool b)
{
    beginResetModel();
    if (!b){
        sel.clear();
    } else {
        setCheckAll();
    }
    endResetModel();
    emit supd();
    is_checked=!is_checked;
}

void ModelZon::setCheckAll()
{
    for (int i=0; i<this->sourceModel()->rowCount();i++){
        sel.insert(this->sourceModel()->data(this->sourceModel()->index(i,0),Qt::EditRole).toInt());
    }
}

void ModelZon::updFinished()
{
    if (checkFlg){
        beginResetModel();
        setCheckAll();
        endResetModel();
        checkFlg=false;
    }
}
