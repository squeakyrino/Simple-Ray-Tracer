#pragma once
#include "V3.h"

class Camera
{
public:
	V3 pos;
	V3 camXAxis;
	V3 camYAxis;
	V3 camZAxis;
	
	
	Camera();
	Camera(V3 pos, V3 camXAxis,	V3 camYAxis, V3 camZAxis);
	~Camera();
};

