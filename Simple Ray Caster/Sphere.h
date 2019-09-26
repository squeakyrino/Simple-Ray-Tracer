#pragma once
#include "GeometricObject.h"
#include "V3.h"

class Sphere :
	public GeometricObject
{
public:
	V3 center;
	f32 radius;

	Sphere(V3 center, f32 radius, Material mat);
	~Sphere();
	f32 hit(V3 rayOrigin, V3 rayDirection) override;
	virtual V3 nextNormal(V3 intersection) override;
};

