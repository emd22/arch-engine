#include "Material.h"

Material::Material()
{
}

Material::Material(const Material &other)
	: diffuse_map(other.diffuse_map)
{
}

std::shared_ptr<Texture> Material::GetDiffuseMap() const
{
	return diffuse_map;
}

void Material::SetDiffuseMap(std::shared_ptr<Texture> ptr)
{
	diffuse_map = ptr;
}
