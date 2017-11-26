#pragma once

#include "Vector.hpp"
#include "Definitions.hpp"
#include <sstream>
#define M_PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679821480865132823066470938446095505822317253594081284811174502841027019385211055596446229489549303819644288109756659334461284756482337867831652712019091456485669234603486104543266482133936072602491412737245870066063155881748815209209628292540917153643678925903600113305305488204665213841469519415116094330572703657595919530921861173819326117931051185480744623799627495673518857527248912279381830119491298336733624406566430860213949463952247371907021798609437027705392171762931767523846748184676694051320005681271452635608277857713427577896091736371787214684409012249534301465495853710507922796892589235420199561121290219608640344181598136297747713099605187072113499999983729780499510597317328160963185950244594553469083026425223082533446850352619311881710100031378387528865875332083814206171776691473035982534904287554687311595628638823537875937519577818577805321712268066130019278766111959092164201989380952572010654858632788659361533818279682303019520353018529689957736225994138912497217752834791315155748572424541506959508295331168617278558890750983817546374649393192550604009277016711390098488240128583616035637076601047101819429555961989467678374494482553797747268471040475346462080466842590694912933136770289891521047521620569660240580

namespace SourceEngine
{
	// 3D ANGLE
	class QAngle
	{
	public:
		// Members
		float x, y, z;

		// Construction/destruction:
		QAngle(void);
		QAngle(float X, float Y, float Z);
		QAngle(float* clr);
		QAngle(Vector A);

		// Initialization
		void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f);
		// TODO (Ilya): Should there be an init that takes a single float for consistency?

		// Got any nasty NAN's?
		bool IsValid() const;
		void Invalidate();

		// array access...
		float operator[](int i) const;
		float& operator[](int i);

		// Base address...
		float* Base();
		float const* Base() const;

		// Initialization methods
		void Random(float minVal, float maxVal);
		void Zero(); ///< zero out a vector

		QAngle& Normalized()
		{
			while (x > 89.0f)
				x -= 180.f;
			while (x < -89.0f)
				x += 180.f;
			while (y > 180.f)
				y -= 360.f;
			while (y < -180.f)
				y += 360.f;
			return *this;
		}

		// equality
		bool operator==(const Vector& v) const;
		bool operator!=(const Vector& v) const;

		// arithmetic operations
		QAngle& operator+=(const Vector& v)
		{
			x += v.x; y += v.y; z += v.z;
			return *this;
		}

		QAngle& operator-=(const Vector& v)
		{
			x -= v.x; y -= v.y; z -= v.z;
			return *this;
		}

		QAngle& operator*=(float fl)
		{
			x *= fl;
			y *= fl;
			z *= fl;
			return *this;
		}

		QAngle& operator*=(const Vector& v)
		{
			x *= v.x;
			y *= v.y;
			z *= v.z;
			return *this;
		}

		QAngle& operator/=(const Vector& v)
		{
			x /= v.x;
			y /= v.y;
			z /= v.z;
			return *this;
		}

		// this ought to be an opcode.
		QAngle&	operator+=(float fl)
		{
			x += fl;
			y += fl;
			z += fl;
			return *this;
		}

		// this ought to be an opcode.
		QAngle&	operator/=(float fl)
		{
			x /= fl;
			y /= fl;
			z /= fl;
			return *this;
		}
		QAngle&	operator-=(float fl)
		{
			x -= fl;
			y -= fl;
			z -= fl;
			return *this;
		}

		// arithmetic operations
		QAngle& operator+=(const QAngle& v)
		{
			x += v.x; y += v.y; z += v.z;
			return *this;
		}

		QAngle& operator-=(const QAngle& v)
		{
			x -= v.x; y -= v.y; z -= v.z;
			return *this;
		}

		QAngle& operator*=(const QAngle& v)
		{
			x *= v.x;
			y *= v.y;
			z *= v.z;
			return *this;
		}

		QAngle& operator/=(const QAngle& v)
		{
			x /= v.x;
			y /= v.y;
			z /= v.z;
			return *this;
		}

		bool operator==(const QAngle& v)
		{
			return (x == v.x && y == v.y && z == v.z);
		}

		bool operator!=(const QAngle& v)
		{
			return !(x == v.x && y == v.y && z == v.z);
		}

		Vector Direction()
		{
			return SourceEngine::Vector(cos(y * M_PI / 180.0f) * cos(x * M_PI / 180.0f), sin(y * M_PI / 180.0f) * cos(x * M_PI / 180.0f), sin(-x * M_PI / 180.0f)).Normalized();
		}

		Vector Forward()
		{
			return Direction();
		}

		Vector Up()
		{
			return SourceEngine::Vector(cos(y * M_PI / 180.0f) * cos(x * M_PI / 180.0f), sin(y * M_PI / 180.0f) * cos(x * M_PI / 180.0f), sin(-(x - 90.0f) * M_PI / 180.0f)).Normalized();
		}

		Vector Right()
		{
			return SourceEngine::Vector(cos(y * M_PI / 180.0f) * cos(x * M_PI / 180.0f), sin((y + 90.0f)* M_PI / 180.0f) * cos(x * M_PI / 180.0f), sin(-x * M_PI / 180.0f)).Normalized();
		}

		// negate the vector components
		void	Negate();

		// Get the vector's magnitude squared.
		float LengthSqr(void) const
		{
			return (x*x + y*y + z*z);
		}

		// return true if this vector is (0,0,0) within tolerance
		bool IsZero(float tolerance = 0.01f) const
		{
			return (x > -tolerance && x < tolerance &&
				y > -tolerance && y < tolerance &&
				z > -tolerance && z < tolerance);
		}

		// Copy
		void	CopyToArray(float* rgfl) const;

		// Multiply, add, and assign to this (ie: *this = a + b * scalar). This
		// is about 12% faster than the actual vector equation (because it's done per-component
		// rather than per-vector).
		void	MulAdd(const Vector& a, const Vector& b, float scalar);

		// assignment
		QAngle& operator=(const QAngle &vOther);

		// copy constructors
		//	Vector(const Vector &vOther);

		// arithmetic operations
		QAngle	operator-(void) const;
		QAngle	operator+(const Vector& v) const;
		QAngle	operator-(const Vector& v) const;
		QAngle	operator*(const Vector& v) const;
		QAngle	operator/(const Vector& v) const;
		QAngle	operator*(float fl) const;
		QAngle	operator/(float fl) const;
		QAngle	operator+(const QAngle& v) const;
		QAngle	operator-(const QAngle& v) const;
		QAngle	operator*(const QAngle& v) const;
		QAngle	operator/(const QAngle& v) const;
	};
}