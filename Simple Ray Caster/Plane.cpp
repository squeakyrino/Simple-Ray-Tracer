#include "Plane.h"


Plane::Plane(V3 normal, f32 distance, Material mat) 
	: normal(normal), distance(distance), GeometricObject(mat)
{
	
}


Plane::~Plane()
{
}

//Returns the distance if it hit. If it doesn't hit return the max float value
f32 Plane::hit(V3 rayOrigin, V3 rayDirection)
{
	f32 result = FLT_MAX;

	//This can return 0 if both vectors are parallel
	//Or, simply, the ray will never hit the plane :)
	f32 denom = normal.dot(rayDirection);
	f32 epsilon = 0.0001f;

	if (denom > epsilon || denom < -epsilon) {

		//We can calculate the hit distance now
		result = (-distance - normal.dot(rayOrigin)) / denom;


	}

	return result;
}

V3 Plane::nextNormal(V3)
{
	return normal;
}

