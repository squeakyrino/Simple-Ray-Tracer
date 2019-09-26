#include "Material.h"



Material::Material()
{
}

Material::Material(V3 emitColor, V3 reflectColor) : emitColor(emitColor), reflectColor(reflectColor), specular(0)
{
}

Material::Material(V3 emitColor, V3 reflectColor, f32 specular) : emitColor(emitColor), reflectColor(reflectColor), specular(specular)
{
}


Material::~Material()
{
}
