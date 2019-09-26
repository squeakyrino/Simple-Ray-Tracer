#include "Ray.h"

Ray::Ray()
{
	o = V3(0, 0, 0);
	d = V3(1, 0, 0);
}

Ray::Ray(V3 o, V3 d) : o(o), d(d)
{
}


Ray::~Ray()
{
}
