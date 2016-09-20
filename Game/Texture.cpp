#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <GL/glew.h>

Texture::Texture(const std::string &path)
{
	int comp;
	unsigned char *image = stbi_load(path.c_str(), &width, &height, &comp, STBI_rgb);

	if (image == nullptr) {
		throw(std::string("Failed to load texture"));
	}

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	if (comp == 3) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	} else if (comp == 4) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	}

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(image);
}

Texture::~Texture()
{
	glDeleteTextures(1, &id);
}

void Texture::Bind()
{
	glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}