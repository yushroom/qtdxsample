#include "qtd2dsample.h"
#include <QtGui/QApplication>
#include <QtGui/QMessageBox>

#include "myd2dwidget.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QtD2DSample w;

	MyD2DWidget widget;
	w.setCentralWidget( &widget );

	HRESULT hr;
	if( FAILED(hr=widget.initialize() ) )
	{
		QMessageBox msgBox;
		msgBox.setWindowTitle(QObject::tr("Failed to initialize DirectX"));
		QString msg(DXGetErrorDescriptionA(hr));
		msgBox.setText(msg);
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.exec();

		return -1;
	}
	w.show();
	return a.exec();
}
