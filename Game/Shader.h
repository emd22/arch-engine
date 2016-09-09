#pragma once
#include <string>
#include "Math/matrix4.h"

class Shader
{
public:
	Shader(const std::string &vertexSource, const std::string &fragmentSource);
	~Shader();

	void Begin();
	void End();

	void SetUniformInt(const std::string &name, int value);
	void SetUniformFloat(const std::string &name, float value);
	void SetUniformMatrix(const std::string &name, const Matrix4 &value);

private:
	unsigned int programID, vertexShaderID, fragmentShaderID;
	bool created = false;
};

