#include "qdoubledelegate.h"

QDoubleDelegate::QDoubleDelegate(QObject *parent) :
    QItemDelegate(parent)
{
}

QWidget *QDoubleDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/*option*/, const QModelIndex &/*index*/) const
{
    DoubleLineEdit *editor = new DoubleLineEdit(parent);
    return editor;
}

void QDoubleDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    return QItemDelegate::setEditorData(editor,index);
}

void QDoubleDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    return QItemDelegate::setModelData(editor,model,index);
}

void QDoubleDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/*index*/) const
{
    editor->setGeometry(option.rect);
}

