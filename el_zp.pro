#-------------------------------------------------
#
# Project created by QtCreator 2023-07-28T15:05:32
#
#-------------------------------------------------

QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = el_zp
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

include(xlsx/qtxlsx.pri)

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    allpressdialog.cpp \
    dialogsplit.cpp \
    dialogtabel.cpp \
    doublelineedit.cpp \
    formcompare.cpp \
    formjob.cpp \
    formliter.cpp \
    formmarktype.cpp \
    formnormpack.cpp \
    formnormpress.cpp \
    formpremmon.cpp \
    formrab.cpp \
    formrecalc.cpp \
    formrepmon.cpp \
    formrepnorm.cpp \
    formreptarif.cpp \
    formtarif.cpp \
    httpsyncmanager.cpp \
        main.cpp \
        mainwindow.cpp \
    db/dbcombobox.cpp \
    db/dbdateedit.cpp \
    db/dbdelegate.cpp \
    db/dblogin.cpp \
    db/dbmapper.cpp \
    db/dbrelationeditdialog.cpp \
    db/dbtablemodel.cpp \
    db/dbviewer.cpp \
    db/executor.cpp \
    db/tablemodel.cpp \
    formcalcwage.cpp \
    modelzon.cpp \
    packnormdialog.cpp \
    progressexecutor.cpp \
    progressreportdialog.cpp \
    qdoubledelegate.cpp \
    rels.cpp \
    tableview.cpp \
    modelro.cpp \
    tndialog.cpp

HEADERS += \
    allpressdialog.h \
    dialogsplit.h \
    dialogtabel.h \
    doublelineedit.h \
    formcompare.h \
    formjob.h \
    formliter.h \
    formmarktype.h \
    formnormpack.h \
    formnormpress.h \
    formpremmon.h \
    formrab.h \
    formrecalc.h \
    formrepmon.h \
    formrepnorm.h \
    formreptarif.h \
    formtarif.h \
    httpsyncmanager.h \
        mainwindow.h \
    db/dbcombobox.h \
    db/dbdateedit.h \
    db/dbdelegate.h \
    db/dblogin.h \
    db/dbmapper.h \
    db/dbrelationeditdialog.h \
    db/dbtablemodel.h \
    db/dbviewer.h \
    db/executor.h \
    db/tablemodel.h \
    formcalcwage.h \
    modelzon.h \
    packnormdialog.h \
    progressexecutor.h \
    progressreportdialog.h \
    qdoubledelegate.h \
    rels.h \
    tableview.h \
    modelro.h \
    tndialog.h

FORMS += \
    allpressdialog.ui \
    dialogsplit.ui \
    dialogtabel.ui \
    formcompare.ui \
    formjob.ui \
    formliter.ui \
    formmarktype.ui \
    formnormpack.ui \
    formnormpress.ui \
    formpremmon.ui \
    formrab.ui \
    formrecalc.ui \
    formrepmon.ui \
    formrepnorm.ui \
    formreptarif.ui \
    formtarif.ui \
        mainwindow.ui \
    db/dblogin.ui \
    db/dbrelationeditdialog.ui \
    formcalcwage.ui \
    packnormdialog.ui \
    progressreportdialog.ui \
    tndialog.ui

RC_FILE = ico.rc

DISTFILES += \
    ico.rc

RESOURCES += \
    res.qrc

