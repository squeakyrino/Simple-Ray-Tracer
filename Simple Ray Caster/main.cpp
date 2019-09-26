#include <vector>
#include <stdio.h>
#include "BitMapHeader.h"
#include <stdlib.h>
#include "Material.h"
#include "Plane.h"
#include "World.h"
#include "Camera.h"
#include "Sphere.h"
#include "Triangle.h"
#include <fstream>
#include <strstream>
#include "Work_Queue.h"
#include <Windows.h>


u32 locked_add(volatile u32* value, u32 add)
{
	return InterlockedExchangeAdd(value, add);
}

std::vector<GeometricObject*> load_from_obj(std::string filename, Material mat) {

	std::vector<GeometricObject*> result = std::vector<GeometricObject*>();

	std::ifstream f(filename);
	if (!f.is_open())
		return result;

	// Local cache of verts
	std::vector<V3> verts;

	while (!f.eof())
	{
		char line[128];
		f.getline(line, 128);

		std::strstream s;
		s << line;

		char junk;

		if (line[0] == 'v')
		{
			V3 v;
			s >> junk >> v.x >> v.y >> v.z;
			verts.push_back(v);
		}

		if (line[0] == 'f')
		{
			int f[3];
			s >> junk >> f[0] >> f[1] >> f[2];
			result.push_back(new Triangle(verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1], mat));
		}
	}

	return result;
}

struct image_u32 {
	u32 width;
	u32 height;
	u32 *pixels;
};

image_u32 allocateImage(u32 width, u32 height) {

	image_u32 result;
	result.height = height;
	result.width = width;
	result.pixels = (u32*)malloc(height * width * sizeof(u32));

	return result;
}

void writeImage(image_u32 image, const char *filename)
{
	u32 outputPixelsSize = image.width * image.height * sizeof(u32);

	BitMapHeader header = {};
	header.fileType = 0x4d42;
	header.fileSize = sizeof(header) + outputPixelsSize;
	header.bitmapOffset = sizeof(header);
	header.size = sizeof(header) - 14;
	header.width = image.width;
	header.height = image.height;
	header.planes = 1;
	header.bitsPerPixel = 32;
	header.sizeOfBitMap = outputPixelsSize;


	FILE *outputFile = fopen(filename, "wb");
	if (outputFile) {
		fwrite(&header, sizeof(header), 1, outputFile);
		fwrite(image.pixels, outputPixelsSize, 1, outputFile);
		fclose(outputFile);
	}
}

void fillPixel(u32 *pixel, u8 r, u8 g, u8 b)
{
	//0xAARRGGBB
	u32 rbg = 0;
	rbg |= r << 16;
	rbg |= g << 8;
	rbg |= b;
	*pixel = rbg;
}

static V3 linear(V3 a, V3 b) {

	V3 result = V3(a.x + 0.5f * (b.x - a.x),
		a.y + 0.5f * (a.y - b.z),
		a.z + 0.5f * (a.z - b.z));

	return result;

}

f32 randomBilateral() {

	f32 uniLateral = (f32)rand() / (f32)RAND_MAX;
	return -1.0f + 2.0f * uniLateral;
}

V3 rayCast(World *world, V3 rayOrigin, V3 rayDirection) {
	
	V3 result = V3(0.0f, 0.0f, 0.0f);

	f32 minHitDist = 0.0001f;
	
	V3 attenuation = V3(1, 1, 1);

	for (int i = 0; i < 8; i++)
	{
		f32 closestDistance = FLT_MAX;
		
		bool hitSomething = false;
		V3 nextRayOrigin;
		V3 nextNormal;

		Material mat;

		for (std::list<GeometricObject*>::iterator it = world->objects.begin(); it != world->objects.end(); ++it) {

			GeometricObject *obj = *it;

			f32 distToObj = obj->hit(rayOrigin, rayDirection);

			if (distToObj > minHitDist &&  distToObj < closestDistance) {

				closestDistance = distToObj;
				hitSomething = true;

				nextRayOrigin = rayOrigin + closestDistance * rayDirection;
				nextNormal = obj->nextNormal(nextRayOrigin);

				mat = obj->mat;
				
			}
		}

		if (hitSomething) {

			result = result + attenuation.hadamard(mat.emitColor);

			f32 cosAtten = -1 * rayDirection.dot(nextNormal);

#if 1
			cosAtten = cosAtten < 0 ? 0 : cosAtten;
#endif
			attenuation = attenuation.hadamard(cosAtten * mat.reflectColor);

			rayOrigin = nextRayOrigin;

			V3 pureBounce = rayDirection - 2.0f * rayDirection.dot(nextNormal) * nextNormal; //Not reflected accuretly

			V3 randomBounce = (nextNormal + V3(randomBilateral(), randomBilateral(), randomBilateral())).NOZ(); //Not really accurate

			rayDirection = randomBounce.lerp(mat.specular, pureBounce);
		}
		else {

			result = result + attenuation.hadamard(world->bgMaterial.emitColor);
			break;
		}
	}


	return result;
}

f32 linearTosRGB(f32 l) {

	f32 s;

	if (l < 0.0f) l = 0.0f;
	if (l > 1.0f) l = 1.0f;

	if (l <= 0.0031308f) {

		s = l * 12.92f;
	}
	else {
		
		s = 1.055f * powf(l, 1.0f / 2.4f) - 0.055f;
	}

	return s;
}

u32* get_pixel_pointer(image_u32* image, u32 x, u32 y) {

	return image->pixels + y * image->width + x;

}

bool render_tile(Work_Queue* queue) {

	u32 work_order_index = locked_add(&queue->next_work_index, 1);
	if (work_order_index >= queue->work_order_count) return false;

	Work_Order* order = queue->orders + work_order_index;

	World* world = order->world;

	image_u32 image;
	image.width = order->image_width;
	image.height = order->image_height;
	image.pixels = order->image;
	u32 x_min = order->min_x;
	u32 x_max = order->max_x;
	u32 y_min = order->min_y;
	u32 y_max = order->max_y;
	
	u32 raysPerPixel = queue->raysPerPixel;
	Camera* cam = queue->cam;
	V3 filmCenter = queue->filmCenter;
	f32 halfFilmW = queue->halfFimlW;
	f32 halfFilmH = queue->halfFilmH;

	volatile static f32 raysCast = 0;
	volatile static f32 totalRays = (f32)image.height * (f32)image.width * raysPerPixel;

	for (size_t y = y_min; y < y_max; y++)
	{
		u32 *outputPixels = get_pixel_pointer(&image, x_min, y);

		f32 filmY = -1.0f + 2.0f*((f32)y / (f32)image.height);

		for (size_t x = x_min; x < x_max; x++)
		{
			f32 filmX = -1.0f + 2.0f*((f32)x / (f32)image.width);

			V3 filmPoint = filmCenter + filmX * halfFilmW * cam->camXAxis + filmY * halfFilmH * cam->camYAxis;

			V3 rayOrigin = cam->pos;
			V3 rayDirection = (filmPoint - cam->pos).NOZ();


			V3 color;
			f32 contrib = 1.0f / (f32)raysPerPixel;
			for (int rayIndex = 0; rayIndex < raysPerPixel; rayIndex++)
			{
				color = color + contrib * rayCast(world, rayOrigin, rayDirection);
				raysCast++;

			}

			V3 bmpColor = V3(linearTosRGB(color.x), linearTosRGB(color.y), linearTosRGB(color.z));

			fillPixel(outputPixels, bmpColor.x * 255.0, bmpColor.y * 255.0, bmpColor.z * 255.0);
			outputPixels++;
			
		}

		
	}

	locked_add(&queue->tile_retired_count, 1);
	printf("\rRaycasting %f%%", (float)queue->tile_retired_count * 100.0f / (float)queue->work_order_count);
	fflush(stdout);
	return true;
}

DWORD WINAPI worker_thread(void* lpParameter)
{
	Work_Queue* queue = (Work_Queue*)lpParameter;
	while (render_tile(queue)) {};
	return 0;
}

void create_thread(void* param)
{
	DWORD id;
	HANDLE thread_handle = CreateThread(0, 0, worker_thread, param, 0, &id);
	CloseHandle(thread_handle);
}


int main() {
#if 1
	u32 outputWidth = 1280;//1280; //854;//7680;//
	u32 outputHeight = 720;// 720; //480;//4320;//
	u32 raysPerPixel = 64;
#else
	u32 outputWidth =  7680 * 2;
	u32 outputHeight = 4320 * 2;
	u32 raysPerPixel = 256;
#endif



	image_u32 image = allocateImage(outputWidth, outputHeight);

	//Planes
	Plane plane1 = Plane(V3(0, 0, 1), 0, Material(V3(0, 0, 0), V3(0.5f, 0.5f, 0.5f)));

	Material cube_mat = Material(V3(0, 0, 0), V3(0.45f, 0.45f, 0.10f), 0.99f);
	
	//Triangle cube = (Triangle(V3(1, 0, 0), V3(-1, 0, 0), V3(0, 0, 3), cube_mat));
	std::vector<GeometricObject*> cube = load_from_obj("../../OLC3DEngine/OLC3DEngine/obj/gadd.obj", cube_mat);

	//Spheres
	Sphere sphere1 = Sphere(V3(0, 0, 0), 1, Material(V3(0, 0, 0), V3(0.7f, 0.5f, 0.3f), 1));
	Sphere sphere2 = Sphere(V3(-3.5, 0, 2), 1, Material(V3(0, 0, 0), V3(0, 1, 0), 1));
	Sphere sphere3 = Sphere(V3(1.23, -4, 0.45), 0.47, Material(V3(5, 5, 0), V3(0, 0, 0)));
	Sphere sphere4 = Sphere(V3(-2, -5, -0.5f), 1, Material(V3(3.0f, 0.0f, 3.0f), V3(0, 0, 0)));
	Sphere sphere5 = Sphere(V3(0, 20, 3), 4, Material(V3(0,0,0), V3(1, 0.3, 0.24), 1));
	Sphere sphere6 = Sphere(V3(-2, -3, 0.53), 0.87, Material(V3(0, 0, 0), V3(0.3, 1, 0.5)));
	Sphere sphere7 = Sphere(V3(0.45, -3.5, 0.21), 0.34, Material(V3(0,0,0), V3(127.0f / 255.0f, 1, 212.0f / 255.0f), 0.75));
	Sphere sphere8 = Sphere(V3(0.75, -5, 1.2), 0.25, Material(V3(0,0,0), V3(0.23, 0.60, 0.78), 0.95));


	World world = World();
	world.addObject(&plane1);
	world.addObject(&sphere1);
	world.addObject(&sphere2);
	world.addObject(&sphere3);
	world.addObject(&sphere4);
	world.addObject(&sphere5);
	world.addObject(&sphere6);
	world.addObject(&sphere7);
	world.addObject(&sphere8);
	
	//for (auto tri : cube) world.addObject(tri);



	V3 camPos = V3(0, -8, 1);
	V3 camZ = camPos.NOZ();
	V3 camX = V3(0, 0, 1).cross(camZ).NOZ();
	V3 camY = camZ.cross(camX).NOZ();

	Camera cam = Camera(camPos, camX, camY, camZ);

	f32 filmDist = 1.0f;
	f32 filmW = 1;
	f32 filmH = 1;
	
	if (image.width > image.height) {

		filmH = filmW * image.height / image.width;
	}
	else if (image.width < image.height) {

		filmW = filmH * image.width / image.height;
	}
	f32 halfFilmH = filmH * 0.5f;
	f32 halfFimlW = filmW * 0.5f;
	V3 filmCenter = cam.pos - cam.camZAxis * filmDist;


	//Multi-thread
	u32 core_count = 8;
	u32 tile_width = (image.width + core_count + 1) / core_count;
	u32 tile_height = tile_width;

	tile_height = tile_width = 64;

	u32 tile_count_x = (image.width + tile_width - 1)/ tile_width;
	u32 tile_count_y = (image.height + tile_height - 1) / tile_height;

	//Set up work queue
	Work_Queue work_queue = {};
	u32 total_order_count = tile_count_x * tile_count_y;
	work_queue.orders = (Work_Order*)malloc(total_order_count * sizeof(Work_Order));
	work_queue.cam = &cam;
	work_queue.raysPerPixel = raysPerPixel;
	work_queue.filmCenter = filmCenter;
	work_queue.halfFilmH = halfFilmH;
	work_queue.halfFimlW = halfFimlW;



	for (size_t tile_y = 0; tile_y < tile_count_y; tile_y++)
	{

		u32 min_y = tile_y * tile_height;
		u32 max_y = min_y + tile_height;
		if (max_y > image.height) max_y = image.height;

		for (size_t tile_x = 0; tile_x < tile_count_x; tile_x++)
		{
			u32 min_x = tile_x * tile_width;
			u32 max_x = min_x + tile_width;
			if (max_x > image.width) max_x = image.width;

			//Add it to the work queue
			Work_Order* current_order = work_queue.orders + work_queue.work_order_count++;
			
			current_order->world = &world;
			current_order->image = image.pixels;
			current_order->image_height = image.height;
			current_order->image_width = image.width;
			current_order->min_x = min_x;
			current_order->min_y = min_y;
			current_order->max_x = max_x;
			current_order->max_y = max_y;
			
		}


	}

	//Fire up those threads

	for(int i = 1; i < core_count; i++) {

		create_thread(&work_queue);
	}


	while (work_queue.tile_retired_count < total_order_count)
	{
		render_tile(&work_queue);
	}
		
	writeImage(image, "test.bmp");


	return 0;
}


