#ifndef MODELZON_H
#define MODELZON_H

#include <QObject>
#include "modelro.h"

class ModelZon : public ModelRo
{
public:
    explicit ModelZon(QObject *parent = nullptr);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &item, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    void setList(QList<int> list);
public slots:
    void refresh();
private:
    QList<int> sel;
};

#endif // MODELZON_H
