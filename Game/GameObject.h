#pragma once
#include "Math/vector3.h"
#include "Math/quaternion.h"
#include "Mesh.h"
#include "BinaryModel.h"

#include <memory>
#include <string>

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

	// saves this game object to a binary file
	void Save(const std::string &filepath) const;
	// loads a game object from a binary file
	static std::shared_ptr<GameObject> Load(const std::string &filepath);

	Vector3 translation;
	Vector3 scale;
	Quaternion rotation;

private:
	Matrix4 matrix;
	std::shared_ptr<Mesh> mesh;
};

