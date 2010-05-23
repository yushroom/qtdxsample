#include "qtdxsample.h"
#include <QtGui/QApplication>
#include <QtCore/QTimer>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QtDXSample window;
	//DXWidget widget(window.centralWidget());

	window.show();
	return a.exec();
}
