#ifndef FORMMARKTYPE_H
#define FORMMARKTYPE_H

#include <QWidget>
#include "db/dbtablemodel.h"
#include "rels.h"

namespace Ui {
class FormMarkType;
}

class FormMarkType : public QWidget
{
    Q_OBJECT

public:
    explicit FormMarkType(QWidget *parent = nullptr);
    ~FormMarkType();

private:
    Ui::FormMarkType *ui;
    DbTableModel *modelMarkType;
};

#endif // FORMMARKTYPE_H
