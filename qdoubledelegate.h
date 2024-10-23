#ifndef QDOUBLEDELEGATE_H
#define QDOUBLEDELEGATE_H

#include <QItemDelegate>
#include <QLineEdit>
#include "doublelineedit.h"

class QDoubleDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit QDoubleDelegate(QObject *parent = nullptr);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    
};

#endif // QDOUBLEDELEGATE_H
