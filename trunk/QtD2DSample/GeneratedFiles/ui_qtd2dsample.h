/********************************************************************************
** Form generated from reading UI file 'qtd2dsample.ui'
**
** Created: Wed Dec 15 07:16:03 2010
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTD2DSAMPLE_H
#define UI_QTD2DSAMPLE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QtD2DSampleClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *QtD2DSampleClass)
    {
        if (QtD2DSampleClass->objectName().isEmpty())
            QtD2DSampleClass->setObjectName(QString::fromUtf8("QtD2DSampleClass"));
        QtD2DSampleClass->resize(600, 400);
        menuBar = new QMenuBar(QtD2DSampleClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        QtD2DSampleClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(QtD2DSampleClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        QtD2DSampleClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(QtD2DSampleClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        QtD2DSampleClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(QtD2DSampleClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        QtD2DSampleClass->setStatusBar(statusBar);

        retranslateUi(QtD2DSampleClass);

        QMetaObject::connectSlotsByName(QtD2DSampleClass);
    } // setupUi

    void retranslateUi(QMainWindow *QtD2DSampleClass)
    {
        QtD2DSampleClass->setWindowTitle(QApplication::translate("QtD2DSampleClass", "QtD2DSample", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class QtD2DSampleClass: public Ui_QtD2DSampleClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTD2DSAMPLE_H
