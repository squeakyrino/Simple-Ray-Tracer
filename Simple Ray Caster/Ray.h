#pragma once
#include "V3.h"

class Ray
{
public:
	V3 o;
	V3 d;

	Ray();
	Ray(V3 o, V3 d);
	~Ray();
};

