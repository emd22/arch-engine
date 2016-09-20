#pragma once
#include "Texture.h"
#include <memory>

class Material {
public:
	Material();
	Material(const Material &other);

	std::shared_ptr<Texture> GetDiffuseMap() const;
	void SetDiffuseMap(std::shared_ptr<Texture> ptr);

	inline float GetRoughness() const { return roughness; }
	inline void SetRoughness(float new_roughness) { roughness = new_roughness; }
	inline float GetShininess() const { return shininess; }
	inline void SetShininess(float new_shininess) { shininess = new_shininess; }

private:
	std::shared_ptr<Texture> diffuse_map;

	float roughness = 0.4;
	float shininess = 0.2;
};