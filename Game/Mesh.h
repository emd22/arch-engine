#pragma once
#include "Vertex.h"
#include <vector>

#define BUFFER_OFFSET(i) ((void*)(i))

class Mesh
{
public:
	Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices);
	Mesh(const Mesh &other);
	~Mesh();

	void Draw();

private:
	unsigned int vboID, iboID;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};

