#ifndef FORMREPTARIF_H
#define FORMREPTARIF_H

#include <QWidget>
#include "modelzon.h"
#include "modelro.h"
#include "rels.h"

namespace Ui {
class FormRepTarif;
}

class FormRepTarif : public QWidget
{
    Q_OBJECT

public:
    explicit FormRepTarif(QWidget *parent = nullptr);
    ~FormRepTarif();

private:
    Ui::FormRepTarif *ui;
    ModelZon *modelZon;
    ModelRo *modelTarif;

private slots:
    void upd();
    void save();
};

#endif // FORMREPTARIF_H
