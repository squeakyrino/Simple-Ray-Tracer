#pragma once
#include "GeometricObject.h"
#include "V3.h"
#include "Material.h"

class Plane :
	public GeometricObject
{
public:

	V3 normal;
	f32 distance; //Distance along the normal

	Plane(V3 normal, f32 distance, Material mat);
	~Plane();
	f32 hit(V3 rayOrigin, V3 rayDirection) override;
	virtual V3 nextNormal(V3 intersection) override;
};

