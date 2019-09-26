#pragma once
#include "V3.h"

class Material
{
public:

	V3 emitColor; //0-1f
	V3 reflectColor;
	f32 specular; //0 is pure difuse, 1 is pure specular ("mirror")

	Material();
	Material(V3 emitColor, V3 reflectColor);
	Material(V3 emitColor, V3 reflectColor, f32 specular);
	~Material();
};

