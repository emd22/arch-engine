#pragma once
#include "Math/vector3.h"
#include "Math/quaternion.h"
#include "Mesh.h"

#include <memory>

class GameObject
{
public:
	GameObject();
	GameObject(const GameObject &other);
	~GameObject();

	void UpdateMatrix();
	Matrix4 GetMatrix();

	std::shared_ptr<Mesh> GetMesh();
	void SetMesh(std::shared_ptr<Mesh> mesh);

	Vector3 translation;
	Vector3 scale;
	Quaternion rotation;

private:
	Matrix4 matrix;
	std::shared_ptr<Mesh> mesh;
};

