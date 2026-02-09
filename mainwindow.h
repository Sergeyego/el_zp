#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QSettings>
#include "formcalcwage.h"
#include "formpremmon.h"
#include "formrepnorm.h"
#include "formliter.h"
#include "formrab.h"
#include "formtarif.h"
#include "formreptarif.h"
#include "formjob.h"
#include "formnormpress.h"
#include "formnormpack.h"
#include "formmarktype.h"
#include "formcompare.h"
#include "formrecalc.h"
#include "formboss.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    bool exist(QObject *a);
    void actAction(QAction *a, void (MainWindow::*sl)());
    void addSubWindow(QWidget *w, QObject *a);
    bool setActiveSubWindow(QString t);
    QMap <QString,QAction*> actions;
    void loadSettings();
    void saveSettings();

private slots:
    void closeTab(int index);
    void newFormCalcWage();
    void newFormPremMon();
    void newFormRepNorm();
    void newFormLiter();
    void newFormRab();
    void newFormTarif();
    void newFormRepTarif();
    void newFormJob();
    void newFormNormPress();
    void newFormNormPack();
    void newFormMarkType();
    void newFormCompare();
    void newFormRecalc();
    void newFormBoss();
};

#endif // MAINWINDOW_H
