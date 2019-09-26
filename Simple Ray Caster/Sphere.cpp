#include "Sphere.h"
#include <math.h>

Sphere::Sphere(V3 center, f32 radius, Material mat) :
	center(center), radius(radius), GeometricObject(mat)
{
}

Sphere::~Sphere()
{
}

f32 Sphere::hit(V3 rayOrigin, V3 rayDirection)
{
	f32 result = FLT_MAX;

	V3 relativeOrigin = rayOrigin - center;
	f32 a = rayDirection.dot(rayDirection);
	f32 b = 2.0f * relativeOrigin.dot(rayDirection);
	f32 c = relativeOrigin.dot(relativeOrigin) - radius * radius;
	f32 rootTerm = sqrtf(b * b - 4 * a * c);
	f32 denom = 2 * a;

	f32 tolerance = 0.0001f;

	//we hit a sphere
	if (rootTerm > tolerance)
	{
		f32 t0 = (-b + rootTerm) / denom;
		f32 t1 = (-b - rootTerm) / denom;

		result = t0;

		if (t1 > tolerance && t1 < t0) {
			result = t1;
		}
	}
	
	return result;
}

V3 Sphere::nextNormal(V3 intersection)
{
	V3 result = intersection - center;

	return result.NOZ();	

}
