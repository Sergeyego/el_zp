#include "doublelineedit.h"

DoubleLineEdit::DoubleLineEdit(QWidget *parent) : QLineEdit(parent)
{
    dval = new QDoubleValidator(0,999999999,4,this);
    dval->setLocale(QLocale::English);
    setValidator(dval);
}

void DoubleLineEdit::keyPressEvent(QKeyEvent *e)
{
    if (e->text()==",") {
        insert(".");
        e->ignore();
    } else {
        return QLineEdit::keyPressEvent(e);
    }
}

void DoubleLineEdit::setRange(double min, double max, int decimal)
{
    dval->setRange(min,max,decimal);
}
