#pragma once

#include "../common/common.h"

ATTRIBUTE_ALIGNED16(struct Camera)
{
	Camera()
	{
	}

	void initialize()
	{
		m_aspect = 1.0f;
		m_eye = vmVector3(0.0f, 0.0f, 0.0f);
		m_target = vmVector3(0.0f, 0.0f, 0.0f);
		m_savedTarget = vmVector3(0.0f, 0.0f, 0.0f);
		m_xAxis = vmVector3(1.0f, 0.0f, 0.0f);
		m_yAxis = vmVector3(0.0f, 1.0f, 0.0f);
		m_zAxis = vmVector3(0.0f, 0.0f, 1.0f);
		m_viewDir = vmVector3(0.0f, 0.0f, 1.0f);
		m_centerOfInterest = 1.0f;

		m_orientation = vmQuat::identity();
		m_savedOrientation = vmQuat::identity();

		m_viewMatrix = vmMatrix4::identity();
		m_projMatrix = vmMatrix4::identity();
	}

	bool getEulerAngle(vmVector3 &euler) const
	{
		return QuaternionToYawPitchRoll(euler, m_orientation);
	}

	void setEulerAngle(const vmVector3 &angle)
	{
		m_orientation = YawPitchRollToQuaternion(angle);
		updateViewMatrix();
	}

	const float& getCenterOfInterest() const
	{
		return m_centerOfInterest;
	}

	void setCenterOfInterest(float centerOfInterest)
	{
		m_centerOfInterest = centerOfInterest;
		updateViewMatrix();
	}

	const vmVector3& getTarget() const
	{
		return m_target;
	}

	void setTarget(const vmVector3 &target)
	{
		 m_target = target;
		 updateProjectionMatrix();
	}

	void setAspect(float aspect)
	{
		m_aspect = aspect;
		updateProjectionMatrix();
	}

	const vmMatrix4& getViewMatrix() const
	{
		return m_viewMatrix;
	}

	const vmMatrix4& getProjMatrix() const
	{
		return m_projMatrix;
	}

	void setFovx(float fovx)
	{
		m_fovx = fovx;
		updateProjectionMatrix();
	}

	void setZnear(float znear)
	{
		m_znear = znear;
		updateProjectionMatrix();
	}

	void setZfar(float zfar)
	{
		m_zfar = zfar;
		updateProjectionMatrix();
	}

	void perspective(float fovx, float aspect, float znear, float zfar)
	{
		// Construct a projection matrix based on the horizontal field of view
		// 'fovx' rather than the more traditional vertical field of view 'fovy'.

		m_fovx = fovx;
		m_znear = znear;
		m_zfar = zfar;
		m_aspect = aspect;

		updateProjectionMatrix();
	}

	void move(float dx, float dy, float dz)
	{
		// Moves the camera by dx world units to the left or right; dy
		// world units upwards or downwards; and dz world units forwards
		// or backwards.

		// Orbiting camera is always positioned relative to the
		// target position. See updateViewMatrix().

		m_target += m_xAxis * dx;
		m_target += m_yAxis * dy;
		m_target += m_zAxis * dz;

		updateViewMatrix();
	}

	void rotate(float headingDegrees, float pitchDegrees, float rollDegrees)
	{
		// Rotates the camera based on its current behavior.
		// Note that not all behaviors support rolling.

		pitchDegrees = -pitchDegrees;
		headingDegrees = -headingDegrees;

		float heading = btRadians(headingDegrees);
		float pitch = btRadians(pitchDegrees);

		vmQuat rot;

		const vmVector3 WORLD_XAXIS(1.0f, 0.0f, 0.0f);
		const vmVector3 WORLD_YAXIS(0.0f, 1.0f, 0.0f);

		if (heading != 0.0f)
		{
			rot = vmQuat::rotation(heading, WORLD_YAXIS);
			m_orientation = m_orientation * rot;
		}

		if (pitch != 0.0f)
		{
			rot = vmQuat::rotation(pitch, WORLD_XAXIS);
			m_orientation = rot * m_orientation;
		}

		updateViewMatrix();
	}

	void zoom(float zoom)
	{
		m_centerOfInterest *= zoom;
		updateViewMatrix();
	}

	void lookAt(const vmVector3 &eye, const vmVector3 &target, const vmVector3 &up)
	{
		m_eye = eye;
		m_target = target;

		m_zAxis = target - eye;
		m_centerOfInterest = length(m_zAxis);
		m_zAxis = normalize(m_zAxis);

		m_viewDir = m_zAxis;

		m_xAxis = normalize(cross(up, m_zAxis));
		m_yAxis = normalize(cross(m_zAxis, m_xAxis));

		//m_viewMatrix = vmMatrix4::lookAt(vmPoint3(eye), vmPoint3(target), up);

		m_viewMatrix.setRow( 0, vmVector4(m_xAxis, -dot(m_xAxis, eye)) );
		m_viewMatrix.setRow( 1, vmVector4(m_yAxis, -dot(m_yAxis, eye)) );
		m_viewMatrix.setRow( 2, vmVector4(m_zAxis, -dot(m_zAxis, eye)) );
		m_viewMatrix.setRow( 3, vmVector4(0,0,0,1) );
	
		m_orientation = vmQuat(m_viewMatrix.getUpper3x3());
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

		m_orientation = normalize(m_orientation);
		m_viewMatrix = vmMatrix4::rotation(m_orientation);

		m_xAxis = m_viewMatrix.getRow(0).getXYZ();
		m_yAxis = m_viewMatrix.getRow(1).getXYZ();
		m_zAxis = m_viewMatrix.getRow(2).getXYZ();
		m_viewDir = m_zAxis;

		m_eye = m_target + m_viewDir * -m_centerOfInterest;

		m_viewMatrix[3][0] = -dot(m_xAxis, m_eye);
		m_viewMatrix[3][1] = -dot(m_yAxis, m_eye);
		m_viewMatrix[3][2] = -dot(m_zAxis, m_eye);
	}

	void updateProjectionMatrix()
	{
		float e = 1.0f / tanf(btRadians(m_fovx) / 2.0f);
		float aspectInv = 1.0f / m_aspect;
		float fovy = 2.0f * atanf(aspectInv / e);
		float xScale = 1.0f / tanf(0.5f * fovy);
		float yScale = xScale / aspectInv;

		m_projMatrix[0][0] = xScale;
		m_projMatrix[1][0] = 0.0f;
		m_projMatrix[2][0] = 0.0f;
		m_projMatrix[3][0] = 0.0f;

		m_projMatrix[0][1] = 0.0f;
		m_projMatrix[1][1] = yScale;
		m_projMatrix[2][1] = 0.0f;
		m_projMatrix[3][1] = 0.0f;

		m_projMatrix[0][2] = 0.0f;
		m_projMatrix[1][2] = 0.0f;
		m_projMatrix[2][2] = m_zfar / (m_zfar - m_znear);
		m_projMatrix[3][2] = -m_znear * m_zfar / (m_zfar - m_znear);

		m_projMatrix[0][3] = 0.0f;
		m_projMatrix[1][3] = 0.0f;
		m_projMatrix[2][3] = 1.0f;
		m_projMatrix[3][3] = 0.0f;
	}

	void backup()
	{
		m_savedTarget = m_target;
		m_savedOrientation = m_orientation;
	}

	void recover()
	{
		m_target = m_savedTarget;
		m_orientation = m_savedOrientation;
	}

	// View matrix
	vmMatrix4	m_viewMatrix;

	// Projection matrix
	vmMatrix4	m_projMatrix;

	//! Camera Parameters
	vmVector3 m_target;
	vmVector3 m_savedTarget;
	vmVector3 m_eye;
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
};
