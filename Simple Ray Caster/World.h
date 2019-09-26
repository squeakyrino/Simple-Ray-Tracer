#pragma once
#include <stdlib.h>
#include <list>
#include "GeometricObject.h"
#include "Material.h"

class World
{
public:

	std::list<GeometricObject*> objects;
	Material bgMaterial;

	World();
	~World();
	void addObject(GeometricObject *object);
};

