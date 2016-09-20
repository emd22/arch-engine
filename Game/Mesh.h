#pragma once
#include "Vertex.h"
#include "Material.h"
#include <vector>
#define BUFFER_OFFSET(i) ((void*)(i))

class Mesh
{
public:
	Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices);
	Mesh(const Mesh &other);
	~Mesh();

	inline const std::vector<Vertex> &GetVertices() const { return vertices; }
	inline const std::vector<unsigned int> &GetIndices() const { return indices; }

	Material &GetMaterial();

	void Draw();

private:
	unsigned int vboID, iboID;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	Material material;
};

