#include "V3.h"
#include <math.h>

V3::V3()
{
	x = y = z = 0;
}



V3::V3(f32 x, f32 y, f32 z) : x(x), y(y), z(z)
{
}


V3::~V3()
{
}

float V3::dot(const V3 vector2)
{
	return x * vector2.x + y * vector2.y + z * vector2.z;
}

V3 V3::cross(const V3 vector)
{
	V3 result = V3(y*vector.z - z * vector.y,
		z * vector.x - x * vector.z,
		x*vector.y - y * vector.x);

	return result;
}

V3 V3::hadamard(const V3 u)
{
	return V3(x * u.x, y * u.y, z * u.z);
}

f32 V3::lengthSquare()
{
	return dot(*this);
}

V3 V3::operator*(float scalar)
{
	V3 result = V3(x * scalar, y * scalar, z * scalar);
	return result;
}

V3 V3::operator-(V3 w)
{
	V3 result = V3(x - w.x, y - w.y,z - w.z);
	return result;
}

V3 V3::operator+(V3 w)
{
	V3 result = V3(x + w.x, y + w.y, z + w.z);
	return result;
}

bool V3::operator==(V3 w)
{
	return x == w.x && y == w.y && z == w.z;
}

V3 V3::operator/=(float scalar)
{
	x /= scalar;
	y /= scalar;
	z /= scalar;
	return *this;
}

V3 V3::NOZ()
{
	V3 result;

	f32 lengthS = lengthSquare();
	if (lengthS > 0.0001f * 0.0001f)
	{
		result = V3(x,y,z) * (1.0f / sqrtf(lengthS));
		
	}
	
	return result;
}

V3 V3::lerp(f32 t, V3 b)
{
	V3 result = (1.0f - t)**this + t * b;

	return result;
}

V3 operator*(float scalar, V3 vector)
{
	V3 result = V3(vector.x * scalar, vector.y * scalar, vector.z * scalar);
	return result;
}
