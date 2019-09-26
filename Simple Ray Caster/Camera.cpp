#include "Camera.h"

Camera::Camera()
{
	pos = V3(0, 0, 1);
	camZAxis = pos.NOZ();
	camXAxis = (camZAxis.cross(camZAxis).NOZ());
	camYAxis = camZAxis.cross(camXAxis).NOZ();

}

Camera::Camera(V3 pos, V3 camXAxis, V3 camYAxis, V3 camZAxis)
	: pos(pos), camXAxis(camXAxis), camYAxis(camYAxis), camZAxis(camZAxis)
{
}


Camera::~Camera()
{
}
