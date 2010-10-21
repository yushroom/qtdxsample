/********************************************************************************
** Form generated from reading UI file 'qtdx09sample.ui'
**
** Created: Thu Oct 21 20:22:42 2010
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTDX09SAMPLE_H
#define UI_QTDX09SAMPLE_H

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

class Ui_QtDX09SampleClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *QtDX09SampleClass)
    {
        if (QtDX09SampleClass->objectName().isEmpty())
            QtDX09SampleClass->setObjectName(QString::fromUtf8("QtDX09SampleClass"));
        QtDX09SampleClass->resize(600, 400);
        menuBar = new QMenuBar(QtDX09SampleClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        QtDX09SampleClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(QtDX09SampleClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        QtDX09SampleClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(QtDX09SampleClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        QtDX09SampleClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(QtDX09SampleClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        QtDX09SampleClass->setStatusBar(statusBar);

        retranslateUi(QtDX09SampleClass);

        QMetaObject::connectSlotsByName(QtDX09SampleClass);
    } // setupUi

    void retranslateUi(QMainWindow *QtDX09SampleClass)
    {
        QtDX09SampleClass->setWindowTitle(QApplication::translate("QtDX09SampleClass", "QtDX09Sample", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class QtDX09SampleClass: public Ui_QtDX09SampleClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTDX09SAMPLE_H
