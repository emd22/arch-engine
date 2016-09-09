#include "GameObject.h"
#include "Math/matrix_util.h"

GameObject::GameObject()
{
	mesh = nullptr;
	translation = Vector3::Zero();
	scale = Vector3::One();
	rotation = Quaternion::Identity();
}

GameObject::GameObject(const GameObject &other)
{
	mesh = other.mesh;
	translation = other.translation;
	scale = other.scale;
	rotation = other.rotation;
	matrix = other.matrix;
}

GameObject::~GameObject()
{
}

void GameObject::UpdateMatrix()
{
	// scale, rotation, translation
	Matrix4 S, R, T;

	MatrixUtil::ToRotation(R, rotation);
	MatrixUtil::ToTranslation(T, translation);
	MatrixUtil::ToScaling(S, scale);

	matrix = S * R * T;
}

Matrix4 GameObject::GetMatrix()
{
	return matrix;
}

std::shared_ptr<Mesh> GameObject::GetMesh()
{
	return mesh;
}

void GameObject::SetMesh(std::shared_ptr<Mesh> m)
{
	mesh = m;
}