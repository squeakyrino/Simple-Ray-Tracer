#include "World.h"



World::World()
{
	objects = std::list<GeometricObject*>();
	bgMaterial = Material(V3(0.3f, 0.4f, 0.5f), V3(0, 0, 0));
}


World::~World()
{
}

void World::addObject(GeometricObject * object)
{
	objects.push_back(object);
}
