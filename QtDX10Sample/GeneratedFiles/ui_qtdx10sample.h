/********************************************************************************
** Form generated from reading UI file 'qtdx10sample.ui'
**
** Created: Thu Dec 16 00:33:35 2010
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTDX10SAMPLE_H
#define UI_QTDX10SAMPLE_H

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

class Ui_QtDX10SampleClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *QtDX10SampleClass)
    {
        if (QtDX10SampleClass->objectName().isEmpty())
            QtDX10SampleClass->setObjectName(QString::fromUtf8("QtDX10SampleClass"));
        QtDX10SampleClass->resize(600, 400);
        menuBar = new QMenuBar(QtDX10SampleClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        QtDX10SampleClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(QtDX10SampleClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        QtDX10SampleClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(QtDX10SampleClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        QtDX10SampleClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(QtDX10SampleClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        QtDX10SampleClass->setStatusBar(statusBar);

        retranslateUi(QtDX10SampleClass);

        QMetaObject::connectSlotsByName(QtDX10SampleClass);
    } // setupUi

    void retranslateUi(QMainWindow *QtDX10SampleClass)
    {
        QtDX10SampleClass->setWindowTitle(QApplication::translate("QtDX10SampleClass", "QtDX10Sample", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class QtDX10SampleClass: public Ui_QtDX10SampleClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTDX10SAMPLE_H
