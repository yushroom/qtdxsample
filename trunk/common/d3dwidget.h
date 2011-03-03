#pragma once

#include "common.h"

#include <QWidget>
#include <QResizeEvent>
#include <QtGui/QMainWindow>
#include <QtGui/QStatusBar>
#include <QtGui/QVector2D>
#include <QtGui/QVector3D>
#include <QtGui/QVector4D>

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
	}
	virtual ~DXWidget()
	{
		_aligned_free(m_camera);
		//SAFE_DELETE(m_camera);
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
		m_camera->m_aspect = 1.0f;
		m_camera->m_eye = vmVector3(0.0f, 0.0f, 0.0f);
		m_camera->m_target = vmVector3(0.0f, 0.0f, 0.0f);
		m_camera->m_savedTarget = vmVector3(0.0f, 0.0f, 0.0f);
		m_camera->m_xAxis = vmVector3(1.0f, 0.0f, 0.0f);
		m_camera->m_yAxis = m_camera->m_targetYAxis = vmVector3(0.0f, 1.0f, 0.0f);
		m_camera->m_zAxis = vmVector3(0.0f, 0.0f, 1.0f);
		m_camera->m_viewDir = vmVector3(0.0f, 0.0f, 1.0f);
		m_camera->m_centerOfInterest = 1.0f;

		m_camera->m_orientation = vmQuat::identity();
		m_camera->m_savedOrientation = vmQuat::identity();

		m_camera->m_viewMatrix = vmMatrix4::identity();
		m_camera->m_projMatrix = vmMatrix4::identity();

		float fAspect = width() / (float)height();
		perspective( 45.0f, fAspect, 0.1f, 5000.0f );

		lookAtCamera( vmVector3( 3.857, 2.5, -3.857 ), vmVector3( 0, 0, 0 ), vmVector3( 0, 1, 0 ) );
	}

	void setAspect(float aspect)
	{
		m_camera->m_aspect = aspect;
		updateProjectionMatrix();
	}

	void perspective(float fovx, float aspect, float znear, float zfar)
	{
		// Construct a projection matrix based on the horizontal field of view
		// 'fovx' rather than the more traditional vertical field of view 'fovy'.

		m_camera->m_fovx = fovx;
		emit setAngleOfView((double)fovx);

		m_camera->m_znear = znear;
		emit setNearClipPlane((double)znear);
		
		m_camera->m_zfar = zfar;
		emit setFarClipPlane((double)zfar);

		m_camera->m_aspect = aspect;

		updateProjectionMatrix();
	}

	void moveCamera(float dx, float dy, float dz)
	{
		// Moves the camera by dx world units to the left or right; dy
		// world units upwards or downwards; and dz world units forwards
		// or backwards.

		// Orbiting camera is always positioned relative to the
		// target position. See updateViewMatrix().

		m_camera->m_target += m_camera->m_xAxis * dx;
		m_camera->m_target += m_camera->m_yAxis * dy;
		m_camera->m_target += m_camera->m_zAxis * dz;
		emit setCameraTranslate(QVector3D(m_camera->m_target[0], m_camera->m_target[1], m_camera->m_target[2]));

		updateViewMatrix();
	}

	void rotateCamera(float headingDegrees, float pitchDegrees, float rollDegrees)
	{
		// Rotates the camera based on its current behavior.
		// Note that not all behaviors support rolling.

		pitchDegrees = -pitchDegrees;
		headingDegrees = -headingDegrees;

		float heading = btRadians(headingDegrees);
		float pitch = btRadians(pitchDegrees);

		vmQuat rot;
		const vmVector3 WORLD_XAXIS(1.0f, 0.0f, 0.0f);

		if (heading != 0.0f)
		{
			rot = vmQuat::rotation(heading, m_camera->m_targetYAxis);
			m_camera->m_orientation = m_camera->m_orientation * rot;
		}

		if (pitch != 0.0f)
		{
			rot = vmQuat::rotation(pitch, WORLD_XAXIS);
			m_camera->m_orientation = rot * m_camera->m_orientation;
		}

		{
			vmVector3 euler;
			if( QuaternionToYawPitchRoll(euler, m_camera->m_orientation) )
			{
				emit setCameraRotate(QVector3D(btDegrees(euler[0]), btDegrees(euler[1]), btDegrees(euler[2])));
			}
		}

		updateViewMatrix();
	}

	void zoomCamera(float zoom)
	{
		m_camera->m_centerOfInterest *= zoom;
		emit setCenterOfInterest((double)m_camera->m_centerOfInterest);
		updateViewMatrix();
	}

	void lookAtCamera(const vmVector3 &eye, const vmVector3 &target, const vmVector3 &up)
	{
		m_camera->m_eye = eye;
		m_camera->m_target = target;
		emit setCameraTranslate(QVector3D(m_camera->m_target[0], m_camera->m_target[1], m_camera->m_target[2]));

		m_camera->m_zAxis = target - eye;
		m_camera->m_centerOfInterest = length(m_camera->m_zAxis);
		emit setCenterOfInterest((double)m_camera->m_centerOfInterest);
		m_camera->m_zAxis = normalize(m_camera->m_zAxis);

		m_camera->m_viewDir = m_camera->m_zAxis;

		m_camera->m_xAxis = normalize(cross(up, m_camera->m_zAxis));
		m_camera->m_yAxis = normalize(cross(m_camera->m_zAxis, m_camera->m_xAxis));

		//m_camera->m_viewMatrix = vmMatrix4::lookAt(vmPoint3(eye), vmPoint3(target), up);

		m_camera->m_viewMatrix.setRow( 0, vmVector4(m_camera->m_xAxis, -dot(m_camera->m_xAxis, eye)) );
		m_camera->m_viewMatrix.setRow( 1, vmVector4(m_camera->m_yAxis, -dot(m_camera->m_yAxis, eye)) );
		m_camera->m_viewMatrix.setRow( 2, vmVector4(m_camera->m_zAxis, -dot(m_camera->m_zAxis, eye)) );
		m_camera->m_viewMatrix.setRow( 3, vmVector4(0,0,0,1) );
	
		m_camera->m_orientation = vmQuat(m_camera->m_viewMatrix.getUpper3x3());

		vmVector3 euler;
		if( QuaternionToYawPitchRoll(euler, m_camera->m_orientation) )
		{
			emit setCameraRotate(QVector3D(btDegrees(euler[0]), btDegrees(euler[1]), btDegrees(euler[2])));
		}

		emit setCameraScale(QVector3D(1, 1, 1));
	}

	static vmQuat YawPitchRollToQuaternion(const vmVector3 &radian)
	{
		float xRadian = radian[0] * 0.5f;
		float yRadian = radian[1] * 0.5f;
		float zRadian = radian[2] * 0.5f;
		float sinX = sin(xRadian);
		float cosX = cos(xRadian);
		float sinY = sin(yRadian);
		float cosY = cos(yRadian);
		float sinZ = sin(zRadian);
		float cosZ = cos(zRadian);
		float x = (float)(sinX * cosY * cosZ - cosX * sinY * sinZ);
		float y = (float)(cosX * sinY * cosZ + sinX * cosY * sinZ);
		float z = (float)(cosX * cosY * sinZ - sinX * sinY * cosZ);
		float w = (float)(cosX * cosY * cosZ + sinX * sinY * sinZ);
		return vmQuat(x,y,z,w);
	}

	static bool QuaternionToYawPitchRoll(vmVector3 &radian, const vmQuat &q)
	{
		float x = q[0];
		float y = q[1];
		float z = q[2];
		float w = q[3];

		//
		float x2 = x + x;
		float y2 = y + y;
		float z2 = z + z;
		float xz2 = x * z2;
		float wy2 = w * y2;
		float temp = -(xz2 - wy2);
		//
		if(temp >= 1){ temp = 1; }
		else if(temp <= -1){ temp = -1; }
		float yRadian = asin(temp);
		radian[1] = (float)yRadian;
		//
		float xx2 = x * x2;
		float xy2 = x * y2;
		float zz2 = z * z2;
		float wz2 = w * z2;
		if(yRadian < SIMD_PI * 0.5)
		{
			if(yRadian > -SIMD_PI * 0.5)
			{
				float yz2 = y * z2;
				float wx2 = w * x2;
				float yy2 = y * y2;
				radian[0] = (float)atan2((yz2 + wx2), (1 - (xx2 + yy2)));
				radian[2]= (float)atan2((xy2 + wz2), (1 - (yy2 + zz2)));
				return true;
			}
			else
			{
				radian[0] = -(float)atan2((xy2 - wz2), (1 - (xx2 + zz2)));
				radian[2] = 0;
				return false;
			}
		}
		else
		{
			radian[0] = (float)atan2((xy2 - wz2), (1 - (xx2 + zz2)));
			radian[2] = 0;
			return false;
		}
	}//method

	void updateViewMatrix()
	{
		// Reconstruct the view matrix.

		m_camera->m_orientation = normalize(m_camera->m_orientation);
		m_camera->m_viewMatrix = vmMatrix4::rotation(m_camera->m_orientation);

		m_camera->m_xAxis = m_camera->m_viewMatrix.getRow(0).getXYZ();
		m_camera->m_yAxis = m_camera->m_viewMatrix.getRow(1).getXYZ();
		m_camera->m_zAxis = m_camera->m_viewMatrix.getRow(2).getXYZ();
		m_camera->m_viewDir = m_camera->m_zAxis;

		m_camera->m_eye = m_camera->m_target + m_camera->m_viewDir * -m_camera->m_centerOfInterest;

		m_camera->m_viewMatrix[3][0] = -dot(m_camera->m_xAxis, m_camera->m_eye);
		m_camera->m_viewMatrix[3][1] = -dot(m_camera->m_yAxis, m_camera->m_eye);
		m_camera->m_viewMatrix[3][2] = -dot(m_camera->m_zAxis, m_camera->m_eye);
	}

	void updateProjectionMatrix()
	{
		float e = 1.0f / tanf(btRadians(m_camera->m_fovx) / 2.0f);
		float aspectInv = 1.0f / m_camera->m_aspect;
		float fovy = 2.0f * atanf(aspectInv / e);
		float xScale = 1.0f / tanf(0.5f * fovy);
		float yScale = xScale / aspectInv;

		m_camera->m_projMatrix[0][0] = xScale;
		m_camera->m_projMatrix[1][0] = 0.0f;
		m_camera->m_projMatrix[2][0] = 0.0f;
		m_camera->m_projMatrix[3][0] = 0.0f;

		m_camera->m_projMatrix[0][1] = 0.0f;
		m_camera->m_projMatrix[1][1] = yScale;
		m_camera->m_projMatrix[2][1] = 0.0f;
		m_camera->m_projMatrix[3][1] = 0.0f;

		m_camera->m_projMatrix[0][2] = 0.0f;
		m_camera->m_projMatrix[1][2] = 0.0f;
		m_camera->m_projMatrix[2][2] = m_camera->m_zfar / (m_camera->m_zfar - m_camera->m_znear);
		m_camera->m_projMatrix[3][2] = -m_camera->m_znear * m_camera->m_zfar / (m_camera->m_zfar - m_camera->m_znear);

		m_camera->m_projMatrix[0][3] = 0.0f;
		m_camera->m_projMatrix[1][3] = 0.0f;
		m_camera->m_projMatrix[2][3] = 1.0f;
		m_camera->m_projMatrix[3][3] = 0.0f;
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
		m_camera->m_target = vmVector3(p.x(), p.y(), p.z());
		updateViewMatrix();
		update();
	}

	void	cameraRotateChanged(QVector3D p)
	{
		m_camera->m_orientation = YawPitchRollToQuaternion(vmVector3(btRadians(p.x()), btRadians(p.y()), btRadians(p.z())));
		updateViewMatrix();
		update();
	}

	void	cameraScaleChanged(QVector3D p)
	{
	}

	void	angleOfViewChanged(double value)
	{
		m_camera->m_fovx = value;
		updateProjectionMatrix();
		update();
	}

	void	nearClipPlaneChanged(double value)
	{
		m_camera->m_znear = value;
		updateProjectionMatrix();
		update();
	}

	void	farClipPlaneChanged(double value)
	{
		m_camera->m_zfar = value;
		updateProjectionMatrix();
		update();
	}

	void	centerOfInterestChanged(double value)
	{
		m_camera->m_centerOfInterest = value;
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

	vmMatrix4& ViewMatrix()
	{
		return m_camera->m_viewMatrix;
	}

	vmMatrix4& ProjMatrix()
	{
		return m_camera->m_projMatrix;
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
			m_camera->m_savedTarget = m_camera->m_target;
			m_camera->m_savedOrientation = m_camera->m_orientation;
			m_camera->m_savedCenterOfInterest = m_camera->m_centerOfInterest;

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
				m_camera->m_orientation = m_camera->m_savedOrientation;
				rotateCamera( delta.x(), delta.y(), 0.0f );
				update();
			}
			else if((e->buttons() & Qt::RightButton) && !(e->buttons() & Qt::LeftButton))
			{
				QPointF delta = (e->posF() - m_clickPos) / (float)height() * m_camera->m_centerOfInterest;
				m_camera->m_target = m_camera->m_savedTarget;
				moveCamera( 0, 0, delta.y() );
				update();
			}
			else if(e->buttons() & Qt::MiddleButton)
			{
				QPointF delta = (e->posF() - m_clickPos) / (float)height() * m_camera->m_centerOfInterest;
				m_camera->m_target = m_camera->m_savedTarget;
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

	ATTRIBUTE_ALIGNED16(struct Camera)
	{
		// View matrix
		vmMatrix4	m_viewMatrix;

		// Projection matrix
		vmMatrix4	m_projMatrix;

		//! Camera Parameters
		vmVector3 m_target;
		vmVector3 m_savedTarget;
		vmVector3 m_eye;
		vmVector3 m_targetYAxis;
		vmVector3 m_xAxis;
		vmVector3 m_yAxis;
		vmVector3 m_zAxis;
		vmVector3 m_viewDir;
		vmQuat m_orientation;
		vmQuat m_savedOrientation;

		float m_aspect;
		float m_fovx;
		float m_znear;
		float m_zfar;

		float m_centerOfInterest;
		float m_savedCenterOfInterest;
	};

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
