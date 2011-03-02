#pragma once

#include <QtGui/QLabel>
#include <QtGui/QFrame>
#include <QtGui/QLineEdit>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QSpinBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QSlider>
#include <QtGui/QFormLayout>
#include <QtGui/QTreeWidget>

#include <QtGui/QColor>
#include <QtGui/QVector2D>
#include <QtGui/QVector3D>
#include <QtGui/QVector4D>
#include <QtGui/QMatrix4x4>

#include <QtCore/QSignalMapper>
#include <QtCore/QVariant>
#include <QtCore/QPointer>

#include "ctkDoubleSlider.h"

class AttrFactory : public QObject
{
	Q_OBJECT
public:
	enum
	{
		GROUP_MARGIN = 2,
		GROUP_SPACING = 0,
		LABEL_MARGIN = 2,
		FIELD_MARGIN = 1,
		FIELD_SPACING = 2,
		SPIN_WIDTH = 66,
		LABEL_WIDTH = 132,
		FIELD_WIDTH = 206,
	};

	explicit AttrFactory(QObject *parent = 0) : QObject(parent)
	{
	}

	virtual	QVariant	data( ) = 0;
	virtual	void		setData(const QVariant &value) = 0;

	virtual QLabel*	createLabel(const QString &key, QWidget *parent = 0)
	{
		QLabel	&label = *(new QLabel(parent));
		label.setText(key);
		label.setWordWrap(true);
		label.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		label.setFixedWidth(LABEL_WIDTH);
		label.setMargin(LABEL_MARGIN);
		return &label;
	}

	virtual QWidget*	createEditor(QWidget *parent = 0)
	{
		QWidget *editor = new QWidget(parent);
		QHBoxLayout *layout = new QHBoxLayout(editor);
		layout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		layout->setSpacing(FIELD_SPACING);
		layout->setMargin(FIELD_MARGIN);
		editor->setLayout(layout);
		return editor;
	}

	virtual void	setupUi( const QString &key, QFormLayout *formLayout )
	{
		QLabel *label = createLabel(key, formLayout->widget());
		QWidget *editor = createEditor(formLayout->widget());

		formLayout->addRow(label, editor);
	}

	virtual void	setupUi( const QString &key, QTreeWidget *treeWidget )
	{
		QWidget *treeWidgetContents = createEditor(treeWidget);

		QLabel *label = new QLabel(treeWidget);
		label->setText(key);
		label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		label->setMargin(LABEL_MARGIN);

		QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget);
		treeWidget->setItemWidget(item, 0, label);
		treeWidget->setItemWidget(item, 1, treeWidgetContents);
	}

};

class BoolAttrFactory : public AttrFactory
{
	Q_OBJECT
public:
	explicit BoolAttrFactory(const QString &_text, QObject *parent = 0) :
		AttrFactory(parent), text(_text)
	{
	}

	virtual	void	setData(const QVariant &value)
	{
		if(value.type() != QVariant::Bool) return;

		checkBox->setChecked(value.toBool());
	}

	virtual	QVariant	data( )
	{
		return QVariant(checkBox->isChecked());
	}

	virtual QWidget*	createEditor(QWidget *parent = 0)
	{
		QWidget *editor = AttrFactory::createEditor(parent);
		checkBox = new QCheckBox(parent);
		checkBox->setText(text);
		connect(checkBox, SIGNAL(toggled(bool)), this, SLOT(slotSetValue(bool)));
		editor->layout()->addWidget(checkBox);
		return editor;
	}

signals:
	void setValue(bool value);

public slots:
	void slotPropertyChanged(bool value)
	{
		checkBox->blockSignals(true);
		checkBox->setChecked(value);
		checkBox->blockSignals(false);
	}

protected slots:
	void slotSetValue(bool value)
	{		
		emit setValue(value);
	}

protected:
	QString		text;
	QPointer<QCheckBox>	checkBox;
};

class StringAttrFactory : public AttrFactory
{
	Q_OBJECT
public:
	explicit StringAttrFactory(QObject *parent = 0) :
		AttrFactory(parent)
	{
	}

	virtual	void	setData(const QVariant &value)
	{
		if(value.type() != QVariant::String) return;

		lineEdit->setText( value.toString() );
	}

	virtual	QVariant	data( )
	{
		return QVariant( lineEdit->text() );
	}

	virtual QWidget*	createEditor(QWidget *parent = 0)
	{
		QWidget *editor = AttrFactory::createEditor(parent);
		lineEdit = new QLineEdit(editor);
		//lineEdit->setFixedWidth( FIELD_WIDTH );
		connect(lineEdit, SIGNAL(textEdited(const QString &)), this, SLOT(slotSetValue(const QString &)));
		editor->layout()->addWidget(lineEdit);
		return editor;
	}

signals:
	void setValue(bool value);

public slots:
	void slotPropertyChanged(const QString &value)
	{
		if(!lineEdit) return;
		lineEdit->blockSignals(true);
		lineEdit->setText(value);
		lineEdit->blockSignals(false);
	}

protected slots:
	void slotSetValue(bool value)
	{		
		emit setValue(value);
	}

protected:
	QPointer<QLineEdit>	lineEdit;
};

class EnumAttrFactory : public AttrFactory
{
	Q_OBJECT
public:
	explicit EnumAttrFactory(const QStringList &_texts, QObject *parent = 0) :
		AttrFactory(parent), texts(_texts)
	{
	}

	virtual	void	setData(const QVariant &value)
	{
		if(value.type() != QVariant::Int) return;

		comboBox->setCurrentIndex(value.toInt());
	}

	virtual	QVariant	data( )
	{
		return QVariant(comboBox->currentIndex());
	}

	virtual QWidget*	createEditor(QWidget *parent = 0)
	{
		QWidget *editor = AttrFactory::createEditor(parent);
		comboBox = new QComboBox(parent);
		comboBox->setFixedWidth(FIELD_WIDTH);
		for(unsigned n=0; n<texts.size(); n++)
		{
			comboBox->setItemText(n, texts[n]);
		}
		connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotSetValue(int)));
		editor->layout()->addWidget(comboBox);
		return editor;
	}

signals:
	void setValue(int value);

public slots:
	void slotPropertyChanged(int value)
	{
		if(!comboBox) return;
		comboBox->blockSignals(true);
		comboBox->setCurrentIndex(value);
		comboBox->blockSignals(false);
	}

protected slots:
	void slotSetValue(bool value)
	{		
		emit setValue(value);
	}

protected:
	QStringList	texts;
	QPointer<QComboBox>	comboBox;
};

class RangeAttrFactory : public AttrFactory
{
	Q_OBJECT
public:
	explicit RangeAttrFactory(QObject *parent = 0) :
		AttrFactory(parent), 
		minimum(0.0), maximum(1.0), singleStep(0.01), pageStep(0.05)
	{
	}

	explicit RangeAttrFactory(double _min, double _max, double _ss, QObject *parent = 0) :
		AttrFactory(parent), 
		minimum(_min), maximum(_max), singleStep(_ss), pageStep(_ss*5.0)
	{
	}

	explicit RangeAttrFactory(double _min, double _max, double _ss, double _ps, QObject *parent = 0) :
		AttrFactory(parent), 
		minimum(_min), maximum(_max), singleStep(_ss), pageStep(_ps)
	{
	}

	virtual	void	setData(const QVariant &value)
	{
		if( value.type() != QVariant::Double ) return;

		spinBox->setValue(value.toDouble());
		slider->setValue(value.toDouble());
	}

	virtual	QVariant	data( )
	{
		return QVariant( spinBox->value() );
	}

	virtual QWidget*	createEditor(QWidget *parent = 0)
	{
		QWidget *editor = AttrFactory::createEditor(parent);
		editor->layout()->setSpacing(4);

		spinBox = new QDoubleSpinBox(parent);
		spinBox->setFixedWidth(SPIN_WIDTH);
		spinBox->setDecimals(3);
		spinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		spinBox->setValue(minimum);
		spinBox->setRange(minimum, maximum);
		spinBox->setSingleStep( singleStep);
		editor->layout()->addWidget(spinBox);

		slider = new ctkDoubleSlider(Qt::Horizontal, parent);
		slider->setFixedWidth(SPIN_WIDTH*2);
		slider->setFocusPolicy(Qt::NoFocus);
		slider->layout()->itemAt(0)->widget()->setFocusPolicy(Qt::NoFocus);
		slider->setValue(minimum);
		slider->setRange(minimum, maximum);
		slider->setSingleStep(singleStep);
		//slider->setPageStep(pageStep);
		editor->layout()->addWidget(slider);

		QObject::connect(spinBox, SIGNAL(editingFinished()), this, SLOT(slotSetSpinBoxValue()));
		QObject::connect(slider, SIGNAL(valueChanged(double)), this, SLOT(slotSetSliderValue(double)));

		return editor;
	}

signals:
	void setValue(double value);

public slots:
	void slotPropertyChanged(double value)
	{
		if(!slider || !spinBox) return;

		slider->blockSignals(true);
		slider->setValue(value);
		slider->blockSignals(false);

		spinBox->blockSignals(true);
		spinBox->setValue(value);
		spinBox->blockSignals(false);
	}

protected slots:
	void slotSetSpinBoxValue()
	{
		if(!slider || !spinBox) return;

		slider->blockSignals(true);
		slider->setValue(spinBox->value());
		slider->blockSignals(false);
		emit setValue(spinBox->value());
	}

	void slotSetSliderValue(double value)
	{
		if(!spinBox) return;

		spinBox->blockSignals(true);
		spinBox->setValue(value);
		spinBox->blockSignals(false);
		emit setValue(value);
	}

protected:
	double		minimum;
	double		maximum;
	double		singleStep;
	double		pageStep;

	QPointer<QDoubleSpinBox>	spinBox;
	QPointer<ctkDoubleSlider>	slider;
};

class FloatAttrFactory : public AttrFactory
{
	Q_OBJECT
public:
	enum
	{
		Dim=1,
	};

	explicit FloatAttrFactory(QObject *parent = 0) : AttrFactory(parent)
	{	
	}

	virtual	void	setData(const QVariant &value)
	{
		if( value.type() != QVariant::Double ) return;

		spinBox->setValue(value.toDouble());
	}

	virtual	QVariant	data( )
	{
		return QVariant( spinBox->value() );
	}

	virtual QWidget*	createEditor(QWidget *parent = 0)
	{
		QWidget *editor = AttrFactory::createEditor(parent);

		spinBox = new QDoubleSpinBox(parent);
		spinBox->setFixedWidth(SPIN_WIDTH);
		spinBox->setDecimals(3);
		spinBox->setRange(-10000000,10000000);
		spinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		QObject::connect(spinBox, SIGNAL(editingFinished()), this, SLOT(slotSetValue()));
		editor->layout()->addWidget(spinBox);

		return editor;
	}

signals:
	void setValue(double value);

public slots:
	void slotPropertyChanged(double value)
	{
		if(!spinBox) return;

		spinBox->blockSignals(true);
		spinBox->setValue(value);
		spinBox->blockSignals(false);
	}

protected slots:
	void slotSetValue()
	{
		emit setValue(spinBox->value());
	}

protected:
	QPointer<QDoubleSpinBox>	spinBox;
};

class Float2AttrFactory : public AttrFactory
{
	Q_OBJECT
public:
	enum
	{
		Dim=2,
	};

	explicit Float2AttrFactory(QObject *parent = 0) : AttrFactory(parent)
	{	
	}

	virtual	void	setData(const QVariant &value)
	{
		if(value.type() != QVariant::Vector2D) return;

		QVector2D v = qVariantValue<QVector2D>(value);

		spinBoxes[0]->setValue( v.x() );
		spinBoxes[1]->setValue( v.y() );
	}

	virtual	QVariant	data( )
	{
		QVector2D v;
		v.setX( spinBoxes[0]->value() );
		v.setY( spinBoxes[1]->value() );
		return QVariant::fromValue( v );
	}

	virtual QWidget*	createEditor(QWidget *parent = 0)
	{
		QWidget *editor = AttrFactory::createEditor(parent);
		for( unsigned d=0; d<Dim; d++ )
		{
			QDoubleSpinBox *spinBox = new QDoubleSpinBox(parent);
			spinBox->setFixedWidth(SPIN_WIDTH);
			spinBox->setDecimals(3);
			spinBox->setRange(-10000000,10000000);
			spinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			QObject::connect(spinBox, SIGNAL(editingFinished()), this, SLOT(slotSetValue()));
			editor->layout()->addWidget(spinBox);
			spinBoxes.append(spinBox);
		}
		return editor;
	}

signals:
	void setValue(QVector2D value);

public slots:
	void slotPropertyChanged(QVector2D value)
	{
		if(spinBoxes.count()!=Dim) return;

		spinBoxes[0]->blockSignals(true);
		spinBoxes[0]->setValue( value.x() );
		spinBoxes[0]->blockSignals(false);

		spinBoxes[1]->blockSignals(true);
		spinBoxes[1]->setValue( value.y() );
		spinBoxes[1]->blockSignals(false);
	}

protected slots:
	void slotSetValue()
	{
		QVector2D v;
		v.setX( spinBoxes[0]->value() );
		v.setY( spinBoxes[1]->value() );		
		emit setValue(v);
	}

protected:
	QList<QDoubleSpinBox*>	spinBoxes;
};

class Float3AttrFactory : public AttrFactory
{
	Q_OBJECT
public:
	enum
	{
		Dim=3,
	};

	explicit Float3AttrFactory(QObject *parent = 0) : AttrFactory(parent)
	{	
	}

	virtual	void	setData(const QVariant &value)
	{
		if(value.type() != QVariant::Vector3D) return;

		QVector3D v = qVariantValue<QVector3D>(value);
		spinBoxes[0]->setValue( v.x() );
		spinBoxes[1]->setValue( v.y() );
		spinBoxes[2]->setValue( v.z() );
	}

	virtual	QVariant	data( )
	{
		QVector3D v;
		v.setX( spinBoxes[0]->value() );
		v.setY( spinBoxes[1]->value() );
		v.setZ( spinBoxes[2]->value() );
		return QVariant::fromValue( v );
	}

	virtual QWidget*	createEditor(QWidget *parent = 0)
	{
		QWidget *editor = AttrFactory::createEditor(parent);
		for( unsigned d=0; d<Dim; d++ )
		{
			QDoubleSpinBox *spinBox = new QDoubleSpinBox(parent);
			spinBox->setFixedWidth(SPIN_WIDTH);
			spinBox->setDecimals(3);
			spinBox->setRange(-10000000,10000000);
			spinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			QObject::connect(spinBox, SIGNAL(editingFinished()), this, SLOT(slotSetValue()));
			editor->layout()->addWidget(spinBox);
			spinBoxes.append(spinBox);
		}
		return editor;
	}

signals:
	void setValue(QVector3D value);

public slots:
	void slotPropertyChanged(QVector3D value)
	{
		if(spinBoxes.count()!=Dim) return;

		spinBoxes[0]->blockSignals(true);
		spinBoxes[0]->setValue( value.x() );
		spinBoxes[0]->blockSignals(false);

		spinBoxes[1]->blockSignals(true);
		spinBoxes[1]->setValue( value.y() );
		spinBoxes[1]->blockSignals(false);

		spinBoxes[2]->blockSignals(true);
		spinBoxes[2]->setValue( value.z() );
		spinBoxes[2]->blockSignals(false);
	}

protected slots:
	void slotSetValue()
	{
		if(spinBoxes.count()!=Dim) return;

		QVector3D v;
		v.setX( spinBoxes[0]->value() );
		v.setY( spinBoxes[1]->value() );
		v.setZ( spinBoxes[2]->value() );
		emit setValue(v);
	}

protected:
	QList<QDoubleSpinBox*>	spinBoxes;
};

class Float4AttrFactory : public AttrFactory
{
	Q_OBJECT
public:
	enum
	{
		Dim=4,
	};

	explicit Float4AttrFactory(QObject *parent = 0) : AttrFactory(parent)
	{	
	}

	virtual	void	setData(const QVariant &value)
	{
		if(value.type() != QVariant::Vector4D) return;

		QVector4D v = qVariantValue<QVector4D>(value);
		spinBoxes[0]->setValue( v.x() );
		spinBoxes[1]->setValue( v.y() );
		spinBoxes[2]->setValue( v.z() );
		spinBoxes[3]->setValue( v.w() );
	}

	virtual	QVariant	data( )
	{
		QVector4D v;
		v.setX( spinBoxes[0]->value() );
		v.setY( spinBoxes[1]->value() );
		v.setZ( spinBoxes[2]->value() );
		v.setW( spinBoxes[3]->value() );
		return QVariant::fromValue( v );
	}

	virtual QWidget*	createEditor(QWidget *parent = 0)
	{
		QWidget *editor = AttrFactory::createEditor(parent);
		for( unsigned d=0; d<Dim; d++ )
		{
			QDoubleSpinBox *spinBox = new QDoubleSpinBox(parent);
			spinBox->setFixedWidth(SPIN_WIDTH);
			spinBox->setDecimals(3);
			spinBox->setRange(-10000000,10000000);
			spinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			QObject::connect(spinBox, SIGNAL(editingFinished()), this, SLOT(slotSetValue()));
			editor->layout()->addWidget(spinBox);
			spinBoxes.append(spinBox);
		}
		return editor;
	}

signals:
	void setValue(QVector4D value);

public slots:
	void slotPropertyChanged(QVector4D value)
	{
		if(spinBoxes.count()!=Dim) return;

		spinBoxes[0]->blockSignals(true);
		spinBoxes[0]->setValue( value.x() );
		spinBoxes[0]->blockSignals(false);

		spinBoxes[1]->blockSignals(true);
		spinBoxes[1]->setValue( value.y() );
		spinBoxes[1]->blockSignals(false);

		spinBoxes[2]->blockSignals(true);
		spinBoxes[2]->setValue( value.z() );
		spinBoxes[2]->blockSignals(false);

		spinBoxes[3]->blockSignals(true);
		spinBoxes[3]->setValue( value.w() );
		spinBoxes[3]->blockSignals(false);
	}

protected slots:
	void slotSetValue()
	{
		if(spinBoxes.count()!=Dim) return;

		QVector4D v;
		v.setX( spinBoxes[0]->value() );
		v.setY( spinBoxes[1]->value() );
		v.setZ( spinBoxes[2]->value() );
		v.setW( spinBoxes[3]->value() );
		emit setValue(v);
	}

protected:
	QList<QDoubleSpinBox*>	spinBoxes;
};

