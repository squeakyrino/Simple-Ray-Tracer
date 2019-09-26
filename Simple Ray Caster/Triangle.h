#pragma once
#include "GeometricObject.h"

class Triangle :
	public GeometricObject
{
private:

	bool insideOutsideTest(V3 p, V3 a, V3 b);

public:

	V3 points[3];
	V3 normal;

	Triangle(V3 a, V3 b, V3 c, Material mat);
	~Triangle();
	f32 hit(V3 rayOrigin, V3 rayDirection) override;
	virtual V3 nextNormal(V3 intersection) override;
};

