#pragma once
#include "Types.h"
#include "V3.h"
#include "Material.h"

class GeometricObject
{
public:

	Material mat;
	GeometricObject(Material mat);
	~GeometricObject();
	virtual f32 hit(V3 rayOrigin, V3 rayDirection) =0;
	virtual V3 nextNormal(V3 intersection) =0;
};

