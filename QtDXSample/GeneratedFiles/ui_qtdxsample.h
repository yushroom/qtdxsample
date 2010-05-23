/********************************************************************************
** Form generated from reading UI file 'qtdxsample.ui'
**
** Created: Mon May 24 00:47:38 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTDXSAMPLE_H
#define UI_QTDXSAMPLE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QtDXSampleClass
{
public:
    QWidget *centralWidget;
    QPushButton *pushButton;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *QtDXSampleClass)
    {
        if (QtDXSampleClass->objectName().isEmpty())
            QtDXSampleClass->setObjectName(QString::fromUtf8("QtDXSampleClass"));
        QtDXSampleClass->resize(400, 360);
        centralWidget = new QWidget(QtDXSampleClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        pushButton = new QPushButton(centralWidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(308, 10, 75, 23));
        pushButton->setAutoFillBackground(false);
        pushButton->setCheckable(true);
        QtDXSampleClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(QtDXSampleClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 400, 24));
        QtDXSampleClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(QtDXSampleClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        QtDXSampleClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(QtDXSampleClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        QtDXSampleClass->setStatusBar(statusBar);

        retranslateUi(QtDXSampleClass);
        QObject::connect(pushButton, SIGNAL(toggled(bool)), QtDXSampleClass, SLOT(toggleAnimation(bool)));

        QMetaObject::connectSlotsByName(QtDXSampleClass);
    } // setupUi

    void retranslateUi(QMainWindow *QtDXSampleClass)
    {
        QtDXSampleClass->setWindowTitle(QApplication::translate("QtDXSampleClass", "QtDXSample", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("QtDXSampleClass", "Animation", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class QtDXSampleClass: public Ui_QtDXSampleClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTDXSAMPLE_H
