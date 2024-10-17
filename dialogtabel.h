#ifndef DIALOGTABEL_H
#define DIALOGTABEL_H

#include <QDialog>
#include "modelzon.h"

namespace Ui {
class DialogTabel;
}

class DialogTabel : public QDialog
{
    Q_OBJECT

public:
    explicit DialogTabel(QAbstractItemModel *model, QWidget *parent = nullptr);
    ~DialogTabel();
    QSet<int> getSel();

private:
    Ui::DialogTabel *ui;
    ModelZon *modelRab;
};

#endif // DIALOGTABEL_H
