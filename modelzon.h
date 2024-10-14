#ifndef MODELZON_H
#define MODELZON_H

#include <QObject>
#include "modelro.h"

class ModelZon : public ModelRo
{
    Q_OBJECT
public:
    explicit ModelZon(QObject *parent = nullptr);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &item, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    void setSel(QSet<int> set);
    QString getStr();
    bool pres();
    bool pack();
public slots:
    void refresh();
private:
    QSet<int> sel;
signals:
    void supd();
};

#endif // MODELZON_H
