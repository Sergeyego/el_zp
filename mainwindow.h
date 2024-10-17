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
};

#endif // MAINWINDOW_H
