#include "QAngle.hpp"

#include <cmath>
#include <limits>

namespace SourceEngine
{
	QAngle::QAngle(void)
	{
		Invalidate();
	}
	QAngle::QAngle(float X, float Y, float Z)
	{
		x = X;
		y = Y;
		z = Z;
	}
	QAngle::QAngle(float* clr)
	{
		x = clr[0];
		y = clr[1];
		z = clr[2];
	}
	QAngle::QAngle(Vector A)
	{
		x = A.x;
		y = A.y;
		z = A.z;
	}

	//-----------------------------------------------------------------------------
	// initialization
	//-----------------------------------------------------------------------------

	void QAngle::Init(float ix, float iy, float iz)
	{
		x = ix; y = iy; z = iz;
	}

	void QAngle::Random(float minVal, float maxVal)
	{
		x = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);
		y = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);
		z = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);
	}

	// This should really be a single opcode on the PowerPC (move r0 onto the vec reg)
	void QAngle::Zero()
	{
		x = y = z = 0.0f;
	}

	//-----------------------------------------------------------------------------
	// assignment
	//-----------------------------------------------------------------------------

	QAngle& QAngle::operator=(const QAngle &vOther)
	{
		x = vOther.x; y = vOther.y; z = vOther.z;
		return *this;
	}

	//-----------------------------------------------------------------------------
	// Array access
	//-----------------------------------------------------------------------------
	float& QAngle::operator[](int i)
	{
		return ((float*)this)[i];
	}

	float QAngle::operator[](int i) const
	{
		return ((float*)this)[i];
	}


	//-----------------------------------------------------------------------------
	// Base address...
	//-----------------------------------------------------------------------------
	float* QAngle::Base()
	{
		return (float*)this;
	}

	float const* QAngle::Base() const
	{
		return (float const*)this;
	}

	//-----------------------------------------------------------------------------
	// IsValid?
	//-----------------------------------------------------------------------------

	bool QAngle::IsValid() const
	{
		return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
	}

	//-----------------------------------------------------------------------------
	// Invalidate
	//-----------------------------------------------------------------------------

	void QAngle::Invalidate()
	{
		//#ifdef _DEBUG
		//#ifdef VECTOR_PARANOIA
		x = y = z = std::numeric_limits<float>::infinity();
		//#endif
		//#endif
	}

	//-----------------------------------------------------------------------------
	// comparison
	//-----------------------------------------------------------------------------

	bool QAngle::operator==(const Vector& src) const
	{
		return (src.x == x) && (src.y == y) && (src.z == z);
	}

	bool QAngle::operator!=(const Vector& src) const
	{
		return (src.x != x) || (src.y != y) || (src.z != z);
	}


	//-----------------------------------------------------------------------------
	// Copy
	//-----------------------------------------------------------------------------
	void	QAngle::CopyToArray(float* rgfl) const
	{
		rgfl[0] = x, rgfl[1] = y, rgfl[2] = z;
	}

	//-----------------------------------------------------------------------------
	// standard math operations
	//-----------------------------------------------------------------------------
	// #pragma message("TODO: these should be SSE")

	void QAngle::Negate()
	{
		x = -x; y = -y; z = -z;
	}

	// FIXME: Remove
	// For backwards compatability
	void	QAngle::MulAdd(const Vector& a, const Vector& b, float scalar)
	{
		x = a.x + b.x * scalar;
		y = a.y + b.y * scalar;
		z = a.z + b.z * scalar;
	}

	//-----------------------------------------------------------------------------
	// arithmetic operations
	//-----------------------------------------------------------------------------

	QAngle QAngle::operator-(void) const
	{
		return QAngle(-x, -y, -z);
	}

	QAngle QAngle::operator+(const QAngle& v) const
	{
		return QAngle(x + v.x, y + v.y, z + v.z);
	}

	QAngle QAngle::operator-(const QAngle& v) const
	{
		return QAngle(x - v.x, y - v.y, z - v.z);
	}

	QAngle QAngle::operator*(float fl) const
	{
		return QAngle(x * fl, y * fl, z * fl);
	}

	QAngle QAngle::operator*(const QAngle& v) const
	{
		return QAngle(x * v.x, y * v.y, z * v.z);
	}

	QAngle QAngle::operator/(float fl) const
	{
		return QAngle(x / fl, y / fl, z / fl);
	}

	QAngle QAngle::operator/(const QAngle& v) const
	{
		return QAngle(x / v.x, y / v.y, z / v.z);
	}

	QAngle QAngle::operator+(const Vector& v) const
	{
		return QAngle(x + v.x, y + v.y, z + v.z);
	}

	QAngle QAngle::operator-(const Vector& v) const
	{
		return QAngle(x - v.x, y - v.y, z - v.z);
	}

	QAngle QAngle::operator*(const Vector& v) const
	{
		return QAngle(x * v.x, y * v.y, z * v.z);
	}
	QAngle QAngle::operator/(const Vector& v) const
	{
		return QAngle(x / v.x, y / v.y, z / v.z);
	}
}