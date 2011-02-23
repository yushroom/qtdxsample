#include "stdafx.h"
#include "QtDXSample.h"
#include "../common/d3dwidget.h"

QtDXSample::QtDXSample(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);


}

QtDXSample::~QtDXSample()
{
	
}

HRESULT QtDXSample::setCanvas(DXWidget *canvas)
{
	this->setCentralWidget(canvas);

	m_timer.setInterval( 30 ) ; // in msec 
	m_timer.setSingleShot( false ) ; 
	QObject::connect( &m_timer, SIGNAL( timeout() ), this, SLOT( idle() ) ) ; 
	m_fTime = 0.0f;

	ui.paramWidget->setFont(QFont("Tahoma",8));
	ui.parameterScrollLayout->setMargin(4);

	QGroupBox* transformGroup = new QGroupBox(ui.scrollAreaWidgetContents);
	transformGroup->setObjectName(QString::fromUtf8("transformGroup"));
	transformGroup->setTitle("Transform Attributes");
	ui.parameterScrollLayout->addWidget(transformGroup);
	QFormLayout* transformGroupLayout = new QFormLayout(transformGroup);
	transformGroupLayout->setSpacing(AttrFactory::GROUP_SPACING);
	transformGroupLayout->setMargin(AttrFactory::GROUP_MARGIN);
	transformGroupLayout->setObjectName(QString::fromUtf8("transformGroupLayout"));
	transformGroupLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

	{
		Float3AttrFactory* factory = new Float3AttrFactory(transformGroup);
		factory->setupUi( "Translate", transformGroupLayout );

		QObject::connect(factory, SIGNAL(setValue(QVector3D)), canvas, SLOT(cameraTranslateChanged(QVector3D)));
		QObject::connect(canvas, SIGNAL(setCameraTranslate(QVector3D)), factory, SLOT(slotPropertyChanged(QVector3D)));

	}
	{
		Float3AttrFactory* factory = new Float3AttrFactory(transformGroup);
		factory->setupUi( "Rotate", transformGroupLayout );

		QObject::connect(factory, SIGNAL(setValue(QVector3D)), canvas, SLOT(cameraRotateChanged(QVector3D)));
		QObject::connect(canvas, SIGNAL(setCameraRotate(QVector3D)), factory, SLOT(slotPropertyChanged(QVector3D)));
	}
	{
		Float3AttrFactory* factory = new Float3AttrFactory(transformGroup);
		factory->setupUi( "Scale", transformGroupLayout );

		QObject::connect(factory, SIGNAL(setValue(QVector3D)), canvas, SLOT(cameraScaleChanged(QVector3D)));
		QObject::connect(canvas, SIGNAL(setCameraScale(QVector3D)), factory, SLOT(slotPropertyChanged(QVector3D)));
	}

	QGroupBox* cameraGroup = new QGroupBox(ui.scrollAreaWidgetContents);
	cameraGroup->setObjectName(QString::fromUtf8("cameraGroup"));
	cameraGroup->setTitle("Camera Attributes");
	ui.parameterScrollLayout->addWidget(cameraGroup);
	QFormLayout* cameraGroupLayout = new QFormLayout(cameraGroup);
	cameraGroupLayout->setSpacing(AttrFactory::GROUP_SPACING);
	cameraGroupLayout->setMargin(AttrFactory::GROUP_MARGIN);
	cameraGroupLayout->setObjectName(QString::fromUtf8("cameraGroupLayout"));
	cameraGroupLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

	{
		RangeAttrFactory* factory = new RangeAttrFactory(1,108,1,cameraGroup);
		factory->setupUi( "Angle of View", cameraGroupLayout );

		QObject::connect(factory, SIGNAL(setValue(double)), canvas, SLOT(angleOfViewChanged(double)));
		QObject::connect(canvas, SIGNAL(setAngleOfView(double)), factory, SLOT(slotPropertyChanged(double)));
	}
	{
		FloatAttrFactory* factory = new FloatAttrFactory(cameraGroup);
		factory->setupUi( "Near Clip Plane", cameraGroupLayout );

		QObject::connect(factory, SIGNAL(setValue(double)), canvas, SLOT(nearClipPlaneChanged(double)));
		QObject::connect(canvas, SIGNAL(setNearClipPlane(double)), factory, SLOT(slotPropertyChanged(double)));
	}
	{
		FloatAttrFactory* factory = new FloatAttrFactory(cameraGroup);
		factory->setupUi( "Far Clip Plane", cameraGroupLayout );

		QObject::connect(factory, SIGNAL(setValue(double)), canvas, SLOT(farClipPlaneChanged(double)));
		QObject::connect(canvas, SIGNAL(setFarClipPlane(double)), factory, SLOT(slotPropertyChanged(double)));
	}

	QGroupBox* movementGroup = new QGroupBox(ui.scrollAreaWidgetContents);
	movementGroup->setObjectName(QString::fromUtf8("movementGroup"));
	movementGroup->setTitle("Movement Options");
	ui.parameterScrollLayout->addWidget(movementGroup);
	QFormLayout* movementGroupLayout = new QFormLayout(movementGroup);
	movementGroupLayout->setSpacing(AttrFactory::GROUP_SPACING);
	movementGroupLayout->setMargin(AttrFactory::GROUP_MARGIN);
	movementGroupLayout->setObjectName(QString::fromUtf8("movementGroupLayout"));
	movementGroupLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

	{
		FloatAttrFactory* factory = new FloatAttrFactory(movementGroup);
		factory->setupUi( "Center of Interest", movementGroupLayout );

		QObject::connect(factory, SIGNAL(setValue(double)), canvas, SLOT(centerOfInterestChanged(double)));
		QObject::connect(canvas, SIGNAL(setCenterOfInterest(double)), factory, SLOT(slotPropertyChanged(double)));
	}

	QSpacerItem* verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
	ui.parameterScrollLayout->addItem(verticalSpacer);

	return canvas->initialize();
}

void QtDXSample::setVisible(bool visible)
{
	if(visible)
	{
		QWidget::setVisible(visible);
		//m_timer.start();
	}
	else
	{
		//m_timer.stop();
		QWidget::setVisible(visible);
	}
}

void QtDXSample::idle()
{
	m_fTime += m_timer.interval()/1000.0;
	//! DirectX Widget
	DXWidget*	widget = (DXWidget *)this->centralWidget();
	if(widget)
	{
		widget->setTime(m_fTime);
		widget->update();
	}
}

void QtDXSample::toggleAnimation(bool pressed)
{
	if(pressed)
		m_timer.start();
	else
		m_timer.stop();
}
