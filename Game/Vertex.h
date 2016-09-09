#pragma once
struct Vertex
{
	float x, y, z;        //Vertex
	float nx, ny, nz;     //Normal
	float s, t;         //Texcoord0

	Vertex() : x(0.0f), y(0.0f), z(0.0f),
		nx(0.0f), ny(0.0f), nz(0.0f),
		s(0.0f), t(0.0f)
	{
	}

	Vertex(const Vertex &other)
		: x(other.x), y(other.y), z(other.z),
		nx(other.nx), ny(other.ny), nz(other.nz),
		s(other.s), t(other.t)
	{
	}
};

