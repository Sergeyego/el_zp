#ifndef FORMREPMON_H
#define FORMREPMON_H

#include <QWidget>
#include "db/tablemodel.h"

namespace Ui {
class FormRepMon;
}

class FormRepMon : public QWidget
{
    Q_OBJECT

public:
    explicit FormRepMon(QWidget *parent = nullptr);
    ~FormRepMon();
    void setModelMada(const QVector<QVector<QVariant>> &data);
    void setTitle(QString title);

private:
    Ui::FormRepMon *ui;
    TableModel *modelRep;

private slots:
    void save();
};

#endif // FORMREPMON_H
