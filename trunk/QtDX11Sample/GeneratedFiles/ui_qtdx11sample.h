/********************************************************************************
** Form generated from reading UI file 'qtdx11sample.ui'
**
** Created: Wed Dec 15 07:16:03 2010
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTDX11SAMPLE_H
#define UI_QTDX11SAMPLE_H

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

class Ui_QtDX11SampleClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *QtDX11SampleClass)
    {
        if (QtDX11SampleClass->objectName().isEmpty())
            QtDX11SampleClass->setObjectName(QString::fromUtf8("QtDX11SampleClass"));
        QtDX11SampleClass->resize(600, 400);
        menuBar = new QMenuBar(QtDX11SampleClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        QtDX11SampleClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(QtDX11SampleClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        QtDX11SampleClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(QtDX11SampleClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        QtDX11SampleClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(QtDX11SampleClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        QtDX11SampleClass->setStatusBar(statusBar);

        retranslateUi(QtDX11SampleClass);

        QMetaObject::connectSlotsByName(QtDX11SampleClass);
    } // setupUi

    void retranslateUi(QMainWindow *QtDX11SampleClass)
    {
        QtDX11SampleClass->setWindowTitle(QApplication::translate("QtDX11SampleClass", "QtDX11Sample", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class QtDX11SampleClass: public Ui_QtDX11SampleClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTDX11SAMPLE_H
