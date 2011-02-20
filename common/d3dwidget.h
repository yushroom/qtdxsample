#pragma once

#include "common.h"
#include "D3DMathHelper.h"
#include "../common/logging.h"

#include <QWidget>
#include <QResizeEvent>
#include <QtGui/QVector2D>
#include <QtGui/QVector3D>
#include <QtGui/QVector4D>

__declspec(align(16)) class DXWidget : public QWidget
{
	Q_OBJECT
public:
	DXWidget( QWidget *parent = 0, Qt::WFlags flags = 0 ) : QWidget( parent, flags )
	{
		setAttribute(Qt::WA_PaintOnScreen);
		setAttribute(Qt::WA_NoSystemBackground);

		m_standBy = false;
		m_lastRendered = 0;
		m_fTime = 0;
	}
	virtual ~DXWidget() {}

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
		m_aspect = 1.0f;
		m_eye = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_target = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_savedTarget = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_xAxis = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
		m_yAxis = m_targetYAxis = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		m_zAxis = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
		m_viewDir = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
		m_centerOfInterest = 1.0f;

		D3DXQuaternionIdentity(&m_orientation);
		D3DXQuaternionIdentity(&m_savedOrientation);

		D3DXMatrixIdentity(&m_viewMatrix);
		D3DXMatrixIdentity(&m_projMatrix);
		D3DXMatrixIdentity(&m_viewProjMatrix);

		float fAspect = width() / (float)height();
		perspective( 45.0f, fAspect, 0.1f, 5000.0f );

		lookAtCamera( D3DXVECTOR3( 3.86, 2.5, -3.86 ), D3DXVECTOR3( 0, 0, 0 ), D3DXVECTOR3( 0, 1, 0 ) );
	}

	void setAspect(float aspect)
	{
		m_aspect = aspect;
		updateProjectionMatrix();
	}

	void perspective(float fovx, float aspect, float znear, float zfar)
	{
		// Construct a projection matrix based on the horizontal field of view
		// 'fovx' rather than the more traditional vertical field of view 'fovy'.

		m_fovx = fovx;
		emit setAngleOfView((double)m_fovx);

		m_znear = znear;
		emit setNearClipPlane((double)m_znear);
		
		m_zfar = zfar;
		emit setFarClipPlane((double)m_zfar);

		m_aspect = aspect;

		updateProjectionMatrix();
	}

	void moveCamera(float dx, float dy, float dz)
	{
		// Moves the camera by dx world units to the left or right; dy
		// world units upwards or downwards; and dz world units forwards
		// or backwards.

		// Orbiting camera is always positioned relative to the
		// target position. See updateViewMatrix().

		m_target += m_xAxis * dx;
		m_target += m_yAxis * dy;
		m_target += m_zAxis * dz;
		emit setCameraTranslate(QVector3D(m_target.x, m_target.y, m_target.z));

		updateViewMatrix();
	}

	void rotateCamera(float headingDegrees, float pitchDegrees, float rollDegrees)
	{
		// Rotates the camera based on its current behavior.
		// Note that not all behaviors support rolling.

		pitchDegrees = -pitchDegrees;
		headingDegrees = -headingDegrees;

		float heading = D3DXToRadian(headingDegrees);
		float pitch = D3DXToRadian(pitchDegrees);

		D3DXQUATERNION rot;
		const D3DXVECTOR3 WORLD_XAXIS(1.0f, 0.0f, 0.0f);

		if (heading != 0.0f)
		{
			D3DXQuaternionRotationAxis(&rot, &m_targetYAxis, heading);
			D3DXQuaternionMultiply(&m_orientation, &rot, &m_orientation);
		}

		if (pitch != 0.0f)
		{
			D3DXQuaternionRotationAxis(&rot, &WORLD_XAXIS, pitch);
			D3DXQuaternionMultiply(&m_orientation, &m_orientation, &rot);
		}

		{
			double x = m_orientation.x;
			double y = m_orientation.y;
			double z = m_orientation.z;
			double w = m_orientation.w;

			double yaw = asin(-2*(x*z - w*y));
			double pitch = atan2(2*(y*z + w*x), w*w - x*x - y*y + z*z);
			double roll = atan2(2*(x*y + w*z), w*w + x*x - y*y - z*z);
			
			emit setCameraRotate(QVector3D(D3DXToDegree(yaw), D3DXToDegree(pitch), D3DXToDegree(roll)));
		}

		updateViewMatrix();
	}

	void zoomCamera(float zoom)
	{
		m_centerOfInterest *= zoom;
		emit setCenterOfInterest((double)m_centerOfInterest);
		updateViewMatrix();
	}

	void lookAtCamera(const D3DXVECTOR3 &eye, const D3DXVECTOR3 &target, const D3DXVECTOR3 &up)
	{
		m_eye = eye;
		m_target = target;
		emit setCameraTranslate(QVector3D(m_target.x, m_target.y, m_target.z));

		m_zAxis = target - eye;
		m_centerOfInterest = D3DXVec3Length(&m_zAxis);
		emit setCenterOfInterest((double)m_centerOfInterest);
		D3DXVec3Normalize(&m_zAxis, &m_zAxis);

		m_viewDir = m_zAxis;

		D3DXVec3Cross(&m_xAxis, &up, &m_zAxis);
		D3DXVec3Normalize(&m_xAxis, &m_xAxis);

		D3DXVec3Cross(&m_yAxis, &m_zAxis, &m_xAxis);
		D3DXVec3Normalize(&m_yAxis, &m_yAxis);
		D3DXVec3Normalize(&m_xAxis, &m_xAxis);

		D3DXMatrixIdentity(&m_viewMatrix);

		m_viewMatrix(0,0) = m_xAxis.x;
		m_viewMatrix(1,0) = m_xAxis.y;
		m_viewMatrix(2,0) = m_xAxis.z;
		m_viewMatrix(3,0) = -D3DXVec3Dot(&m_xAxis, &eye);

		m_viewMatrix(0,1) = m_yAxis.x;
		m_viewMatrix(1,1) = m_yAxis.y;
		m_viewMatrix(2,1) = m_yAxis.z;
		m_viewMatrix(3,1) = -D3DXVec3Dot(&m_yAxis, &eye);

		m_viewMatrix(0,2) = m_zAxis.x;
		m_viewMatrix(1,2) = m_zAxis.y;
		m_viewMatrix(2,2) = m_zAxis.z;
		m_viewMatrix(3,2) = -D3DXVec3Dot(&m_zAxis, &eye);

		m_viewProjMatrix = m_viewMatrix * m_projMatrix;

		D3DXQuaternionRotationMatrix(&m_orientation, &m_viewMatrix);

		double x = m_orientation.x;
		double y = m_orientation.y;
		double z = m_orientation.z;
		double w = m_orientation.w;

		double yaw = asin(-2*(x*z - w*y));
		double pitch = atan2(2*(y*z + w*x), w*w - x*x - y*y + z*z);
		double roll = atan2(2*(x*y + w*z), w*w + x*x - y*y - z*z);
		
		emit setCameraRotate(QVector3D(D3DXToDegree(yaw), D3DXToDegree(pitch), D3DXToDegree(roll)));
	}

	void updateViewMatrix()
	{
		// Reconstruct the view matrix.

		D3DXQuaternionNormalize(&m_orientation, &m_orientation);
		D3DXMatrixRotationQuaternion(&m_viewMatrix, &m_orientation);

		m_xAxis = D3DXVECTOR3(m_viewMatrix(0,0), m_viewMatrix(1,0), m_viewMatrix(2,0));
		m_yAxis = D3DXVECTOR3(m_viewMatrix(0,1), m_viewMatrix(1,1), m_viewMatrix(2,1));
		m_zAxis = D3DXVECTOR3(m_viewMatrix(0,2), m_viewMatrix(1,2), m_viewMatrix(2,2));
		m_viewDir = m_zAxis;

		m_eye = m_target + m_viewDir * -m_centerOfInterest;

		m_viewMatrix(3,0) = -D3DXVec3Dot(&m_xAxis, &m_eye);
		m_viewMatrix(3,1) = -D3DXVec3Dot(&m_yAxis, &m_eye);
		m_viewMatrix(3,2) = -D3DXVec3Dot(&m_zAxis, &m_eye);

		m_viewProjMatrix = m_viewMatrix * m_projMatrix;
	}

	void updateProjectionMatrix()
	{
		float e = 1.0f / tanf(D3DXToRadian(m_fovx) / 2.0f);
		float aspectInv = 1.0f / m_aspect;
		float fovy = 2.0f * atanf(aspectInv / e);
		float xScale = 1.0f / tanf(0.5f * fovy);
		float yScale = xScale / aspectInv;

		m_projMatrix(0,0) = xScale;
		m_projMatrix(1,0) = 0.0f;
		m_projMatrix(2,0) = 0.0f;
		m_projMatrix(3,0) = 0.0f;

		m_projMatrix(0,1) = 0.0f;
		m_projMatrix(1,1) = yScale;
		m_projMatrix(2,1) = 0.0f;
		m_projMatrix(3,1) = 0.0f;

		m_projMatrix(0,2) = 0.0f;
		m_projMatrix(1,2) = 0.0f;
		m_projMatrix(2,2) = m_zfar / (m_zfar - m_znear);
		m_projMatrix(3,2) = -m_znear * m_zfar / (m_zfar - m_znear);

		m_projMatrix(0,3) = 0.0f;
		m_projMatrix(1,3) = 0.0f;
		m_projMatrix(2,3) = 1.0f;
		m_projMatrix(3,3) = 0.0f;

		m_viewProjMatrix = m_viewMatrix * m_projMatrix;
	}

signals:
	void	setCameraTranslate(QVector3D);
	void	setCameraRotate(QVector3D);
	void	setCenterOfInterest(double);
	void	setAngleOfView(double);
	void	setNearClipPlane(double);
	void	setFarClipPlane(double);

public slots:
	void	cameraTranslateChanged(QVector3D p)
	{
		m_target = D3DXVECTOR3(p.x(), p.y(), p.z());
		updateViewMatrix();
		update();
	}

	void	cameraRotateChanged(QVector3D p)
	{
		D3DXQuaternionRotationYawPitchRoll(&m_orientation, D3DXToRadian(p.x()), D3DXToRadian(p.y()), D3DXToRadian(p.z()));
		updateViewMatrix();
		update();
	}

	void	angleOfViewChanged(double value)
	{
		m_fovx = value;
		updateProjectionMatrix();
		update();
	}

	void	nearClipPlaneChanged(double value)
	{
		m_znear = value;
		updateProjectionMatrix();
		update();
	}

	void	farClipPlaneChanged(double value)
	{
		m_zfar = value;
		updateProjectionMatrix();
		update();
	}

	void	centerOfInterestChanged(double value)
	{
		m_centerOfInterest = value;
		updateViewMatrix();
		update();
	}

protected:
	QPaintEngine *paintEngine() const { return 0; } 

	virtual void	paintEvent(QPaintEvent *e)
	{
		Q_UNUSED(e);
		render();
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

	static bool isCameraOperation(QMouseEvent *e)
	{
		return true/*e->modifiers() == Qt::AltModifier*/;
	}

	void mousePressEvent(QMouseEvent *e)
	{
		m_clickPos = e->posF();

		if(isCameraOperation(e))
		{
			m_savedTarget = m_target;
			m_savedOrientation = m_orientation;
			m_savedCenterOfInterest = m_centerOfInterest;

			if((e->buttons() & Qt::LeftButton) && !(e->buttons() & Qt::RightButton))
			{
				setCursor(Qt::OpenHandCursor);
			}
			else if((e->buttons() & Qt::RightButton) && !(e->buttons() & Qt::LeftButton))
			{
				setCursor(Qt::SizeVerCursor);
			}
			else if(e->buttons() & Qt::MiddleButton)
			{
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
				m_orientation = m_savedOrientation;
				rotateCamera( delta.x(), delta.y(), 0.0f );
				update();
			}
			else if((e->buttons() & Qt::RightButton) && !(e->buttons() & Qt::LeftButton))
			{
				QPointF delta = (e->posF() - m_clickPos) / (float)height();
				m_centerOfInterest = m_savedCenterOfInterest;
				zoomCamera( exp(delta.y()) );
				update();
			}
			else if(e->buttons() & Qt::MiddleButton)
			{
				QPointF delta = (e->posF() - m_clickPos) / (float)height() * m_centerOfInterest;
				m_target = m_savedTarget;
				moveCamera( -delta.x(), delta.y(), 0.0f );
				update();
			}
		}

		QWidget::mouseMoveEvent(e);
	}

	void mouseReleaseEvent(QMouseEvent *e)
	{
		setCursor(Qt::ArrowCursor);
		
		QWidget::mouseReleaseEvent(e);
	}

	void wheelEvent(QWheelEvent *e)
	{
		QWidget::wheelEvent(e);

		zoomCamera(1.0f - (e->delta() / WHEEL_DELTA) * 0.125f);
		update();
	}


	//! if stand-by mode
	bool	m_standBy;

	//! Last updated time
	double	m_lastRendered;

	//! Clicked mouse position
	QPointF	m_clickPos;

	//! Camera Parameters
	float m_aspect;
	float m_fovx;
	float m_znear;
	float m_zfar;
	D3DXVECTOR3 m_target;
	D3DXVECTOR3 m_savedTarget;
	D3DXVECTOR3 m_eye;
	D3DXVECTOR3 m_targetYAxis;
	D3DXVECTOR3 m_xAxis;
	D3DXVECTOR3 m_yAxis;
	D3DXVECTOR3 m_zAxis;
	D3DXVECTOR3 m_viewDir;
	D3DXQUATERNION m_orientation;
	D3DXQUATERNION m_savedOrientation;
	float m_centerOfInterest;
	float m_savedCenterOfInterest;

	//! Time
	double	m_fTime;

	// View matrix 
	D3DXMATRIX	m_viewMatrix;

	// Projection matrix
	D3DXMATRIX	m_projMatrix;

	// View x Projection matrix
	D3DXMATRIX	m_viewProjMatrix;
};
