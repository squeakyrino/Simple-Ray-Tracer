#include "World.h"

class Work_Order
{
public:
	World* world;
	u32* image;
	u32 image_height;
	u32 image_width;

	u32 min_x;
	u32 max_x;
	u32 min_y;
	u32 max_y;

	Work_Order();
	~Work_Order();
};

