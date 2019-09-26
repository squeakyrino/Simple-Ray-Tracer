#include <vector>
#include "Work_Order.h"
#include "Camera.h"
class Work_Queue
{
public:
	Work_Order* orders;
	volatile u32 work_order_count;
	volatile u32 next_work_index;
	volatile u32 tile_retired_count;
	
	u32 raysPerPixel;
	V3 filmCenter;
	f32 halfFimlW, halfFilmH;
	Camera* cam;
	Work_Queue();
	~Work_Queue();
};

