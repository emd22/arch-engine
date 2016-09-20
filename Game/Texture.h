#pragma once
#include <string>

class Texture
{
public:
	Texture(const std::string &path);
	~Texture();

	void Bind();
	void Unbind();

protected:
	unsigned int id;
	int width;
	int height;
};

