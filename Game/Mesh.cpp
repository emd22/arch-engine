#include "Mesh.h"
#include "Vertex.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices)
	: vertices(vertices), indices(indices)
{
	glGenBuffers(1, &vboID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0].x, GL_STATIC_DRAW);

	glGenBuffers(1, &iboID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
}

Mesh::Mesh(const Mesh &other)
{
	vertices = other.vertices;
	indices = other.indices;

	glGenBuffers(1, &vboID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0].x, GL_STATIC_DRAW);

	glGenBuffers(1, &iboID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
}

Mesh::~Mesh()
{
	glDeleteBuffers(1, &vboID);
	glDeleteBuffers(1, &iboID);
}

Material &Mesh::GetMaterial()
{
	return material;
}

void Mesh::Draw() 
{
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	
	glEnableVertexAttribArray(0); // positions
	glEnableVertexAttribArray(1); // normals
	glEnableVertexAttribArray(2); // texcoords

	// set pointer to vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), BUFFER_OFFSET(0));
	// set pointer to normals
	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), BUFFER_OFFSET(12));
	// set pointer to texcoords
	glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Vertex), BUFFER_OFFSET(24));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0));
}