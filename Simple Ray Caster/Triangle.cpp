#include "Triangle.h"
#include <math.h>

bool Triangle::insideOutsideTest(V3 p, V3 a, V3 b)
{
	//Step 5: Inside-outside test
	V3 c; //Vector perpendicular to triangle's normal

	V3 edge0 = b - a;
	V3 vp0 = p - a;
	c = edge0.cross(vp0);
	if (normal.dot(c) < 0)
		return false; //If the P is on the right side of the edge it's false

	return true;
}

Triangle::Triangle(V3 a, V3 b, V3 c, Material mat) : GeometricObject(mat)
{
	points[0] = a;
	points[1] = b;
	points[2] = c;

	V3 v0v1 = points[1] - points[0];
	V3 v0v2 = points[2] - points[0];
	normal = (v0v1.cross(v0v2)).NOZ();	
}


Triangle::~Triangle()
{

}

f32 Triangle::hit(V3 rayOrigin, V3 rayDirection)
{

	const f32 EPSILON = 0.0000001f;
	//Check if the ray intersects the plane first
	f32 result = FLT_MAX;

	V3 edge1 = points[1] - points[0];
	V3 edge2 = points[2] - points[0];


	V3 h = rayDirection.cross(edge2);
	f32 a = edge1.dot(h);

	if (a < EPSILON && a > -EPSILON) return result;

	f32 f = 1 / a;

	V3 s = rayOrigin - points[0];

	f32 u = f * (s.dot(h));

	if (u < 0 || u > 1.0f) return result;

	V3 q = s.cross(edge1);
	float v = f * rayDirection.dot(q);

	if (v < 0 || v + u > 1.0f) return result;

	// At this stage we can compute t to find out where the intersection point is on the line.
	float t = f * edge2.dot(q);

	if (t > EPSILON) {
		// rey intersection

		result = t;
	}
	
	return result;
}




V3 Triangle::nextNormal(V3 intersection)
{
	return normal;
}
