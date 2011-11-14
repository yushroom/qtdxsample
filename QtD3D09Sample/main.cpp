#include "stdafx.h"
#include "../GUI/qtdxsample.h"
#include <QtGui/QApplication>
#include <QtGui/QMessageBox>

#include "mydx9widget.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QtDXSample w;

	HRESULT hr;
	if( FAILED(hr=w.setCanvas( new MyDX9Widget(&w))) )
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
