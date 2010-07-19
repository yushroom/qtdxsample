/********************************************************************************
** Form generated from reading UI file 'qtdxsample.ui'
**
** Created: Tue Jul 20 01:40:03 2010
**      by: Qt User Interface Compiler version 4.6.3
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
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QtDXSampleClass
{
public:
    QAction *actionPlayPause;
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *QtDXSampleClass)
    {
        if (QtDXSampleClass->objectName().isEmpty())
            QtDXSampleClass->setObjectName(QString::fromUtf8("QtDXSampleClass"));
        QtDXSampleClass->resize(400, 360);
        actionPlayPause = new QAction(QtDXSampleClass);
        actionPlayPause->setObjectName(QString::fromUtf8("actionPlayPause"));
        actionPlayPause->setCheckable(true);
        actionPlayPause->setChecked(false);
        actionPlayPause->setEnabled(true);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Resources/media-play-16.png"), QSize(), QIcon::Active, QIcon::Off);
        icon.addFile(QString::fromUtf8(":/Resources/media-pause-16.png"), QSize(), QIcon::Active, QIcon::On);
        icon.addFile(QString::fromUtf8(":/Resources/media-pause-16.png"), QSize(), QIcon::Selected, QIcon::Off);
        icon.addFile(QString::fromUtf8(":/Resources/media-play-16.png"), QSize(), QIcon::Selected, QIcon::On);
        actionPlayPause->setIcon(icon);
        centralWidget = new QWidget(QtDXSampleClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
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

        mainToolBar->addAction(actionPlayPause);

        retranslateUi(QtDXSampleClass);
        QObject::connect(actionPlayPause, SIGNAL(triggered(bool)), QtDXSampleClass, SLOT(toggleAnimation(bool)));

        QMetaObject::connectSlotsByName(QtDXSampleClass);
    } // setupUi

    void retranslateUi(QMainWindow *QtDXSampleClass)
    {
        QtDXSampleClass->setWindowTitle(QApplication::translate("QtDXSampleClass", "QtDXSample", 0, QApplication::UnicodeUTF8));
        actionPlayPause->setText(QApplication::translate("QtDXSampleClass", "PlayPause", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionPlayPause->setToolTip(QApplication::translate("QtDXSampleClass", "Start/Pause Animation", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionPlayPause->setShortcut(QApplication::translate("QtDXSampleClass", "Ctrl+P", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class QtDXSampleClass: public Ui_QtDXSampleClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTDXSAMPLE_H
