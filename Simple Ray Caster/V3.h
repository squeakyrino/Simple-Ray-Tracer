#pragma once
#include "Types.h"

class V3
{
public:
	f32 x, y, z;	
	V3();
	V3(f32 x, f32 y, f32 z);
	~V3();
	float dot(const V3 vector2);
	V3 cross(const V3 vector);
	V3 hadamard(const V3 u);
	V3 NOZ();
	V3 lerp(f32 t, V3 b);
	f32 lengthSquare();
	V3 operator*(float scalar);
	V3 operator-(V3 w);
	V3 operator+(V3 w);
	bool operator==(V3 w);
	V3 operator/=(float scalar);
};
V3 operator*(float scalar, V3 vector);
