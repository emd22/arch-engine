#include "GameObject.h"
#include "Math/matrix_util.h"
#include <fstream>

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

void GameObject::Save(const std::string &filepath) const
{
	std::ofstream file;
	file.open(filepath, std::ios::out | std::ios::binary);

	// write model translation
	{
		// write marker
		int marker = BinaryModelFlags::MODEL_TRANSLATION;
		file.write((char*)&marker, sizeof(marker));
		// write Vector3
		file.write((char*)&translation.x, sizeof(translation.x));
		file.write((char*)&translation.y, sizeof(translation.y));
		file.write((char*)&translation.z, sizeof(translation.z));
	}
	// write model rotation
	{
		// write marker
		int marker = BinaryModelFlags::MODEL_ROTATION;
		file.write((char*)&marker, sizeof(marker));
		// write Quaternion
		file.write((char*)&rotation.x, sizeof(rotation.x));
		file.write((char*)&rotation.y, sizeof(rotation.y));
		file.write((char*)&rotation.z, sizeof(rotation.z));
		file.write((char*)&rotation.w, sizeof(rotation.w));
	}
	// write model scale
	{
		// write marker
		int marker = BinaryModelFlags::MODEL_SCALE;
		file.write((char*)&marker, sizeof(marker));
		// write Vector3
		file.write((char*)&scale.x, sizeof(scale.x));
		file.write((char*)&scale.y, sizeof(scale.y));
		file.write((char*)&scale.z, sizeof(scale.z));
	}

	if (mesh != nullptr) {
		// save the mesh
		
		// save vertices. we have to write the MODEL_VERTICES flag along with the number of vertices.
		{
			int marker = BinaryModelFlags::MODEL_VERTICES;
			file.write((char*)&marker, sizeof(marker));

			int num_vertices = mesh->GetVertices().size();
			file.write((char*)&num_vertices, sizeof(num_vertices));

			auto &vertices = mesh->GetVertices();

			// now, we write the actual vertex data.
			for (int i = 0; i < num_vertices; i++) {
				auto &vertex = vertices[i];
				// vertex position
				file.write((char*)&vertex.x, sizeof(vertex.x));
				file.write((char*)&vertex.y, sizeof(vertex.y));
				file.write((char*)&vertex.z, sizeof(vertex.z));
				// vertex normal
				file.write((char*)&vertex.nx, sizeof(vertex.nx));
				file.write((char*)&vertex.ny, sizeof(vertex.ny));
				file.write((char*)&vertex.nz, sizeof(vertex.nz));
				// vertex texture coordinates
				file.write((char*)&vertex.s, sizeof(vertex.s));
				file.write((char*)&vertex.t, sizeof(vertex.t));
			}
		}

		// save indices. write MODEL_INDICES flag along with number of indices
		{
			int marker = BinaryModelFlags::MODEL_INDICES;
			file.write((char*)&marker, sizeof(marker));

			int num_indices = mesh->GetIndices().size();
			file.write((char*)&num_indices, sizeof(num_indices));

			auto &indices = mesh->GetIndices();

			// now, we write index to the file.
			for (int i = 0; i < num_indices; i++) {
				file.write((char*)&indices[i], sizeof(indices[i]));
			}
		}
	}

	// write end of model flag
	int marker = BinaryModelFlags::MODEL_END_FILE;
	file.write((char*)&marker, sizeof(marker));

	// close output file
	file.close();
}

std::shared_ptr<GameObject> GameObject::Load(const std::string &filepath)
{
	std::ifstream file;
	file.open(filepath, std::ios::in | std::ios::binary);
	if (!file.is_open()) {
		return nullptr;
	}

	auto object = std::make_shared<GameObject>();

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	int marker = 0;
	do {
		file.read((char*)&marker, sizeof(marker));

		if (marker == BinaryModelFlags::MODEL_TRANSLATION) {
			// read the Vector3
			file.read((char*)&object->translation.x, sizeof(translation.x));
			file.read((char*)&object->translation.y, sizeof(translation.y));
			file.read((char*)&object->translation.z, sizeof(translation.z));
		} else if (marker == BinaryModelFlags::MODEL_ROTATION) {
			// read the Quaternion
			file.read((char*)&object->rotation.x, sizeof(rotation.x));
			file.read((char*)&object->rotation.y, sizeof(rotation.y));
			file.read((char*)&object->rotation.z, sizeof(rotation.z));
			file.read((char*)&object->rotation.w, sizeof(rotation.w));
		} else if (marker == BinaryModelFlags::MODEL_SCALE) {
			// read the Vector3
			file.read((char*)&object->scale.x, sizeof(scale.x));
			file.read((char*)&object->scale.y, sizeof(scale.y));
			file.read((char*)&object->scale.z, sizeof(scale.z));
		} else if (marker == BinaryModelFlags::MODEL_VERTICES) {
			// read the number of vertices to read
			int num_vertices;
			file.read((char*)&num_vertices, sizeof(num_vertices));

			// allocate number of vertices we will read
			vertices.resize(num_vertices);

			for (int i = 0; i < num_vertices; i++) {
				auto &vertex = vertices[i];
				// vertex position
				file.read((char*)&vertex.x, sizeof(vertex.x));
				file.read((char*)&vertex.y, sizeof(vertex.y));
				file.read((char*)&vertex.z, sizeof(vertex.z));
				// vertex normal
				file.read((char*)&vertex.nx, sizeof(vertex.nx));
				file.read((char*)&vertex.ny, sizeof(vertex.ny));
				file.read((char*)&vertex.nz, sizeof(vertex.nz));
				// vertex texture coordinates
				file.read((char*)&vertex.s, sizeof(vertex.s));
				file.read((char*)&vertex.t, sizeof(vertex.t));
			}
		} else if (marker == BinaryModelFlags::MODEL_INDICES) {
			// read the number of indices to read
			int num_indices;
			file.read((char*)&num_indices, sizeof(num_indices));

			// allocate number of indices we will read
			indices.resize(num_indices);

			for (int i = 0; i < num_indices; i++) {
				auto &index = indices[i];
				file.read((char*)&index, sizeof(index));
			}
		}

	} while (marker != BinaryModelFlags::MODEL_END_FILE);

	file.close();

	if (!indices.empty()) {
		// vertices are not empty, add a mesh
		object->SetMesh(std::make_shared<Mesh>(vertices, indices));
	}

	// update loaded model's matrix
	object->UpdateMatrix();

	return object;
}