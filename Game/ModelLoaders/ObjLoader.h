#pragma once
#include "../Mesh.h"

#include <string>
#include <memory>

class ObjLoader {
public:
	std::shared_ptr<Mesh> LoadMesh(const std::string &path);
};