#pragma once

#include "../common/common.h"
#include "../common/camera.h"

#include <QWidget.h>
#include <QtGui/QResizeEvent>
#include <QtGui/QMainWindow.h>
#include <QtGui/QStatusBar.h>
#include <QtGui/QVector2D.h>
#include <QtGui/QVector3D.h>
#include <QtGui/QVector4D.h>

class DXWidget : public QWidget
{
	Q_OBJECT

public:
	BT_DECLARE_ALIGNED_ALLOCATOR()

	DXWidget( QWidget *parent = 0, Qt::WFlags flags = 0 ) : QWidget( parent, flags )
	{
		setAttribute(Qt::WA_PaintOnScreen);
		setAttribute(Qt::WA_NoSystemBackground);

		m_standBy = false;
		m_lastRendered = 0;
		m_fTime = 0;
		m_camera = (Camera*)_aligned_malloc(sizeof(Camera),16);
		m_camera->initialize();
	}
	virtual ~DXWidget()
	{
		_aligned_free(m_camera);
	}

	virtual void	setVisible(bool visible)
	{
		if(visible)
		{
			QWidget::setVisible(visible);
			//initialize();
		}
		else
		{
			//uninitialize();
			QWidget::setVisible(visible);
		}
	}

	virtual HRESULT	initialize() = 0;
	virtual void	uninitialize() = 0;

	virtual HRESULT	restoreDeviceObjects() = 0;
	virtual HRESULT	invalidateDeviceObjects() = 0;

	virtual void setTime(double fTime)
	{
		m_fTime = fTime;
	}

	virtual HRESULT	render()
	{
		return S_OK;
	}

	virtual HRESULT	present()
	{
		return S_OK;
	}

	virtual void	initCamera()
	{
		perspective( 45.0f, width() / (float)height(), 0.1f, 5000.0f );
		lookAtCamera( vmVector3( 3.857, 2.5, -3.857 ), vmVector3( 0, 0, 0 ), vmVector3( 0, 1, 0 ) );
	}

	void setAspect(float aspect)
	{
		m_camera->setAspect(aspect);
	}

	void perspective(float fovx, float aspect, float znear, float zfar)
	{
		m_camera->perspective(fovx, aspect, znear, zfar);
		emit setAngleOfView((double)fovx);
		emit setNearClipPlane((double)znear);
		emit setFarClipPlane((double)zfar);
	}

	void moveCamera(float dx, float dy, float dz)
	{
		m_camera->move(dx, dy, dz);

		vmVector3 target = m_camera->getTarget();
		emit setCameraTranslate(QVector3D(target[0], target[1], target[2]));
	}

	void rotateCamera(float headingDegrees, float pitchDegrees, float rollDegrees)
	{
		m_camera->rotate(headingDegrees, pitchDegrees, rollDegrees);

		vmVector3 euler;
		if(m_camera->getEulerAngle(euler))
		{
			emit setCameraRotate(QVector3D(btDegrees(euler[0]), btDegrees(euler[1]), btDegrees(euler[2])));
		}
	}

	void zoomCamera(float zoom)
	{
		m_camera->zoom(zoom);

		emit setCenterOfInterest((double)m_camera->getCenterOfInterest());
	}

	void lookAtCamera(const vmVector3 &eye, const vmVector3 &target, const vmVector3 &up)
	{
		m_camera->lookAt(eye, target, up);

		emit setCameraTranslate(QVector3D(target[0], target[1], target[2]));

		emit setCenterOfInterest((double)m_camera->getCenterOfInterest());

		vmVector3 euler;
		if(m_camera->getEulerAngle(euler))
		{
			emit setCameraRotate(QVector3D(btDegrees(euler[0]), btDegrees(euler[1]), btDegrees(euler[2])));
		}

		emit setCameraScale(QVector3D(1, 1, 1));
	}

signals:
	void	setCameraTranslate(QVector3D);
	void	setCameraRotate(QVector3D);
	void	setCameraScale(QVector3D);
	void	setCenterOfInterest(double);
	void	setAngleOfView(double);
	void	setNearClipPlane(double);
	void	setFarClipPlane(double);

public slots:
	void	cameraTranslateChanged(QVector3D p)
	{
		m_camera->setTarget(vmVector3(p.x(), p.y(), p.z()));
		update();
	}

	void	cameraRotateChanged(QVector3D p)
	{
		m_camera->setEulerAngle(vmVector3(btRadians(p.x()), btRadians(p.y()), btRadians(p.z())));
		update();
	}

	void	cameraScaleChanged(QVector3D p)
	{
	}

	void	angleOfViewChanged(double value)
	{
		m_camera->setFovx(value);
		update();
	}

	void	nearClipPlaneChanged(double value)
	{
		m_camera->setZnear(value);
		update();
	}

	void	farClipPlaneChanged(double value)
	{
		m_camera->setZfar(value);
		update();
	}

	void	centerOfInterestChanged(double value)
	{
		m_camera->setCenterOfInterest(value);
		update();
	}

protected:
	QPaintEngine *paintEngine() const { return 0; } 
	virtual void onResize(UINT, UINT) = 0;

	virtual void	paintEvent(QPaintEvent *e)
	{
		Q_UNUSED(e);
		render();
	}

	virtual void	resizeEvent(QResizeEvent *p_event)
	{
		QSize newSize = size();
		if(p_event)
		{
			newSize = p_event->size();
			// if( width()==newSize.width() && height()==newSize.height() ) return;
			QWidget::resizeEvent( p_event );
		}
		onResize(newSize.width(), newSize.height());
	}

	void keyPressEvent(QKeyEvent *e)
	{
		switch (e->key()) {
			//case Qt::Key_Escape:
				break;
			default:
				QWidget::keyPressEvent(e);
		}
	}

	const vmMatrix4& ViewMatrix() const 
	{
		return m_camera->getViewMatrix();
	}

	const vmMatrix4& ProjMatrix() const 
	{
		return m_camera->getProjMatrix();
	}

	static bool isCameraOperation(QMouseEvent *e)
	{
		return true/*e->modifiers() == Qt::AltModifier*/;
	}

	void showStatus(const QString &message)
	{
		qobject_cast<QMainWindow*>(parent())->statusBar()->showMessage(message);
	}

	void mousePressEvent(QMouseEvent *e)
	{
		m_clickPos = e->posF();

		if(isCameraOperation(e))
		{
			m_camera->backup();

			if((e->buttons() & Qt::LeftButton) && !(e->buttons() & Qt::RightButton))
			{
				// Shift to constrain rotaion
				showStatus(tr("Tumble Tool: LMB Drag: Use LMB or MMB to tumble"));
				setCursor(Qt::OpenHandCursor);
			}
			else if((e->buttons() & Qt::RightButton) && !(e->buttons() & Qt::LeftButton))
			{
				showStatus(tr("Dolly Tool: RMB Drag: Use mouse to dolly"));
				setCursor(Qt::SizeVerCursor);
			}
			else if(e->buttons() & Qt::MiddleButton)
			{
				showStatus(tr("Track Tool: MMB Drag: Use LMB or MMB to track"));
				setCursor(Qt::SizeAllCursor);
			}
		}

		QWidget::mousePressEvent(e);
	}

	void mouseMoveEvent(QMouseEvent *e)
	{
		if(isCameraOperation(e) && height()>0)
		{
			if((e->buttons() & Qt::LeftButton) && !(e->buttons() & Qt::RightButton))
			{
				QPointF delta = (e->posF() - m_clickPos) / (float)height() * 180.0f;
				m_camera->recover();
				rotateCamera( delta.x(), delta.y(), 0.0f );
				update();
			}
			else if((e->buttons() & Qt::RightButton) && !(e->buttons() & Qt::LeftButton))
			{
				QPointF delta = (e->posF() - m_clickPos) / (float)height() * m_camera->getCenterOfInterest();
				m_camera->recover();
				moveCamera( 0, 0, delta.y() );
				update();
			}
			else if(e->buttons() & Qt::MiddleButton)
			{
				QPointF delta = (e->posF() - m_clickPos) / (float)height() * m_camera->getCenterOfInterest();
				m_camera->recover();
				moveCamera( -delta.x(), delta.y(), 0.0f );
				update();
			}
		}

		QWidget::mouseMoveEvent(e);
	}

	void mouseReleaseEvent(QMouseEvent *e)
	{
		setCursor(Qt::ArrowCursor);
		showStatus("");
		
		QWidget::mouseReleaseEvent(e);
	}

	void wheelEvent(QWheelEvent *e)
	{
		QWidget::wheelEvent(e);

		zoomCamera(1.0f - (e->delta() / WHEEL_DELTA) * 0.125f);
		update();
	}

	//! Pointer of Camera for 16-byte alignment
	Camera*	m_camera;	

	//! if stand-by mode
	bool	m_standBy;

	//! Last updated time
	double	m_lastRendered;

	//! Clicked mouse position
	QPointF	m_clickPos;

	//! Time
	double	m_fTime;
};
