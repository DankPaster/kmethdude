#include "Vector.hpp"

#include <cmath>
#include <limits>

inline float sqrt2(float sqr)
{
	float root = 0;

	__asm
	{
		sqrtss xmm0, sqr
		movss root, xmm0
	}

	return root;
}

namespace SourceEngine
{
    void VectorCopy(const Vector& src, Vector& dst)
    {
        dst.x = src.x;
        dst.y = src.y;
        dst.z = src.z;
    }
    void VectorLerp(const Vector& src1, const Vector& src2, float t, Vector& dest)
    {
        dest.x = src1.x + (src2.x - src1.x) * t;
        dest.y = src1.y + (src2.y - src1.y) * t;
        dest.z = src1.z + (src2.z - src1.z) * t;
    }
    float VectorLength(const Vector& v)
    {
        return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
    }
    void VectorCrossProduct(const Vector& a, const Vector& b, Vector& result)
    {
        result.x = a.y*b.z - a.z*b.y;
        result.y = a.z*b.x - a.x*b.z;
        result.z = a.x*b.y - a.y*b.x;
    }
    float NormalizeVector(Vector& v)
    {
        float l = v.Length();
        if(l != 0.0f) {
            v /= l;
        } else {
            v.x = v.y = v.z = 0.0f;
        }
        return l;
    }

    Vector::Vector(void)
    {
        Invalidate();
    }
    Vector::Vector(float X, float Y, float Z)
    {
        x = X;
        y = Y;
        z = Z;
    }
    Vector::Vector(float* clr)
    {
        x = clr[0];
        y = clr[1];
        z = clr[2];
    }
	Vector Vector::Angle(Vector* up)
	{
		if (!x && !y)
			return Vector(0, 0, 0);

		float roll = 0;

		if (up)
		{
			Vector left = (*up).Cross(*this);
			roll = atan2f(left.z, (left.y * x) - (left.x * y)) * 180.0f / M_PI;
		}

		return Vector(atan2f(-z, sqrt2(x*x + y*y)) * 180.0f / M_PI, atan2f(y, x) * 180.0f / M_PI, roll);
	}

    //-----------------------------------------------------------------------------
    // initialization
    //-----------------------------------------------------------------------------

    void Vector::Init(float ix, float iy, float iz)
    {
        x = ix; y = iy; z = iz;
    }

    Vector Vector::Normalized() const
    {
        Vector res = *this;
        float l = res.Length();
        if(l != 0.0f) {
            res /= l;
        } else {
            res.x = res.y = res.z = 0.0f;
        }
        return res;
    }
    float Vector::NormalizeInPlace()
    {
        return NormalizeVector(*this);
    }

    void Vector::Random(float minVal, float maxVal)
    {
        x = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);
        y = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);
        z = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);
    }

    // This should really be a single opcode on the PowerPC (move r0 onto the vec reg)
    void Vector::Zero()
    {
        x = y = z = 0.0f;
    }

    //-----------------------------------------------------------------------------
    // assignment
    //-----------------------------------------------------------------------------

    Vector& Vector::operator=(const Vector &vOther)
    {
        x = vOther.x; y = vOther.y; z = vOther.z;
        return *this;
    }


    //-----------------------------------------------------------------------------
    // Array access
    //-----------------------------------------------------------------------------
    float& Vector::operator[](int i)
    {
        return ((float*)this)[i];
    }

    float Vector::operator[](int i) const
    {
        return ((float*)this)[i];
    }


    //-----------------------------------------------------------------------------
    // Base address...
    //-----------------------------------------------------------------------------
    float* Vector::Base()
    {
        return (float*)this;
    }

    float const* Vector::Base() const
    {
        return (float const*)this;
    }

    //-----------------------------------------------------------------------------
    // IsValid?
    //-----------------------------------------------------------------------------

    bool Vector::IsValid() const
    {
        return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
    }

    //-----------------------------------------------------------------------------
    // Invalidate
    //-----------------------------------------------------------------------------

    void Vector::Invalidate()
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

    bool Vector::operator==(const Vector& src) const
    {
        return (src.x == x) && (src.y == y) && (src.z == z);
    }

    bool Vector::operator!=(const Vector& src) const
    {
        return (src.x != x) || (src.y != y) || (src.z != z);
    }


    //-----------------------------------------------------------------------------
    // Copy
    //-----------------------------------------------------------------------------
    void	Vector::CopyToArray(float* rgfl) const
    {
        rgfl[0] = x, rgfl[1] = y, rgfl[2] = z;
    }

    //-----------------------------------------------------------------------------
    // standard math operations
    //-----------------------------------------------------------------------------
    // #pragma message("TODO: these should be SSE")

    void Vector::Negate()
    {
        x = -x; y = -y; z = -z;
    }

    // get the component of this vector parallel to some other given vector
    Vector Vector::ProjectOnto(const Vector& onto)
    {
        return onto * (this->Dot(onto) / (onto.LengthSqr()));
    }

    // FIXME: Remove
    // For backwards compatability
    void	Vector::MulAdd(const Vector& a, const Vector& b, float scalar)
    {
        x = a.x + b.x * scalar;
        y = a.y + b.y * scalar;
        z = a.z + b.z * scalar;
    }

    Vector VectorLerp(const Vector& src1, const Vector& src2, float t)
    {
        Vector result;
        VectorLerp(src1, src2, t, result);
        return result;
    }

    float Vector::Dot(const Vector& b) const
    {
        return (x*b.x + y*b.y + z*b.z);
    }
    void VectorClear(Vector& a)
    {
        a.x = a.y = a.z = 0.0f;
    }

    float Vector::Length(void) const
    {
        return sqrt(x*x + y*y + z*z);
    }

    // check a point against a box
    bool Vector::WithinAABox(Vector const &boxmin, Vector const &boxmax)
    {
        return (
            (x >= boxmin.x) && (x <= boxmax.x) &&
            (y >= boxmin.y) && (y <= boxmax.y) &&
            (z >= boxmin.z) && (z <= boxmax.z)
            );
    }

    //-----------------------------------------------------------------------------
    // Get the distance from this vector to the other one 
    //-----------------------------------------------------------------------------
    float Vector::DistTo(const Vector &vOther) const
    {
        Vector delta;
        delta = *this - vOther;
        return delta.Length();
    }

    //-----------------------------------------------------------------------------
    // Returns a vector with the min or max in X, Y, and Z.
    //-----------------------------------------------------------------------------
    Vector Vector::Min(const Vector &vOther) const
    {
        return Vector(x < vOther.x ? x : vOther.x,
            y < vOther.y ? y : vOther.y,
            z < vOther.z ? z : vOther.z);
    }

    Vector Vector::Max(const Vector &vOther) const
    {
        return Vector(x > vOther.x ? x : vOther.x,
            y > vOther.y ? y : vOther.y,
            z > vOther.z ? z : vOther.z);
    }


    //-----------------------------------------------------------------------------
    // arithmetic operations
    //-----------------------------------------------------------------------------

    Vector Vector::operator-(void) const
    {
        return Vector(-x, -y, -z);
    }

    Vector Vector::operator+(const Vector& v) const
    {
        return Vector(x + v.x, y + v.y, z + v.z);
    }

    Vector Vector::operator-(const Vector& v) const
    {
        return Vector(x - v.x, y - v.y, z - v.z);
    }

    Vector Vector::operator*(float fl) const
    {
        return Vector(x * fl, y * fl, z * fl);
    }

    Vector Vector::operator*(const Vector& v) const
    {
        return Vector(x * v.x, y * v.y, z * v.z);
    }

    Vector Vector::operator/(float fl) const
    {
        return Vector(x / fl, y / fl, z / fl);
    }

    Vector Vector::operator/(const Vector& v) const
    {
        return Vector(x / v.x, y / v.y, z / v.z);
    }

    Vector operator*(float fl, const Vector& v)
    {
        return v * fl;
    }

    //-----------------------------------------------------------------------------
    // cross product
    //-----------------------------------------------------------------------------

    Vector Vector::Cross(const Vector& vOther) const
    {
        Vector res;
        VectorCrossProduct(*this, vOther, res);
        return res;
    }
}