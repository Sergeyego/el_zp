#ifndef FORMBOSS_H
#define FORMBOSS_H

#include <QWidget>
#include "db/dbtablemodel.h"
#include "rels.h"

namespace Ui {
class FormBoss;
}

class FormBoss : public QWidget
{
    Q_OBJECT

public:
    explicit FormBoss(QWidget *parent = nullptr);
    ~FormBoss();

private:
    Ui::FormBoss *ui;
    DbTableModel *modelBoss;
    DbTableModel *modelNorm;
};

#endif // FORMBOSS_H
