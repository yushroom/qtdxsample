#ifndef QTD2DSAMPLE_H
#define QTD2DSAMPLE_H

#include "ui_qtd2dsample.h"

#include <QtGui/QMainWindow>
#include <QtCore/QPointer>
#include <QtCore/QTimer>

class DXWidget;
class QtD2DSample : public QMainWindow
{
	Q_OBJECT

public:
	QtD2DSample(QWidget *parent = 0, Qt::WFlags flags = 0);
	~QtD2DSample();

private:
	Ui::QtD2DSampleClass ui;
};

#endif // QTD2DSAMPLE_H
