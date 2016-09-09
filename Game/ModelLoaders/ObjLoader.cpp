#include "ObjLoader.h"
#include "../Math/vector2.h"
#include "../Math/vector3.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

void Split(const std::string &s, char delim, std::vector<std::string> &elems) 
{
	std::stringstream ss;
	ss.str(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
}

std::vector<std::string> Split(const std::string &s, char delim) 
{
	std::vector<std::string> elems;
	Split(s, delim, elems);
	return elems;
}

static inline std::string &LeftTrim(std::string &s) 
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(),
		std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

static inline std::string &RightTrim(std::string &s) 
{
	s.erase(std::find_if(s.rbegin(), s.rend(),
		std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

static inline std::string &Trim(std::string &s) 
{
	return LeftTrim(RightTrim(s));
}

static std::vector<std::string> RemoveEmpty(const std::vector<std::string> &strings)
{
	std::vector<std::string> res;
	for (auto &&str : strings) {
		if (!str.empty()) {
			res.push_back(str);
		}
	}
	return res;
}

struct ObjFace {
	unsigned int vertex;
	unsigned int normal;
	unsigned int texcoord;
};

ObjFace ParseObjIndex(const std::string &token)
{
	auto tokens = Split(token, '/');

	ObjFace res;
	if (!tokens.empty()) {
		res.vertex = std::stoi(tokens[0]) - 1;
		if (tokens.size() > 1) {
			if (!tokens[1].empty()) {
				res.texcoord = std::stoi(tokens[1]) - 1;
			}
			if (tokens.size() > 2) {
				if (!tokens[2].empty()) {
					res.normal = std::stoi(tokens[2]) - 1;
				}
			}
		}
	}

	return res;
}

std::shared_ptr<Mesh> ObjLoader::LoadMesh(const std::string &path)
{
	std::vector<Vector3> positions;
	std::vector<Vector3> normals;
	std::vector<Vector2> texcoords;

	std::vector<ObjFace> obj_faces;

	std::ifstream file;
	file.open(path);

	if (!file.is_open()) {
		std::cout << "Invalid file: " << path << ".\n";
		return nullptr;
	}

	std::string line;
	while (std::getline(file, line)) {
		// check to make sure it is not a comment
		if (line[0] != '#') {
			// tokens are separated by spaces
			auto tokens = Split(line, ' ');
			if (!tokens.empty()) {
				tokens = RemoveEmpty(tokens);

				if (tokens[0] == "v") { // vertex position
					Vector3 pos;
					// read x
					pos.x = std::stof(tokens[1]);
					// read y
					pos.y = std::stof(tokens[2]);
					// read z
					pos.z = std::stof(tokens[3]);
					// add to list of positions
					positions.push_back(pos);
				} else if (tokens[0] == "vn") { // vertex normal
					Vector3 norm;
					// read x
					norm.x = std::stof(tokens[1]);
					// read y
					norm.y = std::stof(tokens[2]);
					// read z
					norm.z = std::stof(tokens[3]);
					// add to list of normals
					normals.push_back(norm);
				} else if (tokens[0] == "vt") { // vertex texture coordinate
					Vector2 texcoord;
					// read s
					texcoord.x = std::stof(tokens[1]);
					// read t
					texcoord.y = std::stof(tokens[2]);
					// add to list of texture coordinates
					texcoords.push_back(texcoord);
				} else if (tokens[0] == "f") {
					for (int i = 0; i < tokens.size() - 3; i++) {
						obj_faces.push_back(ParseObjIndex(tokens[1]));
						obj_faces.push_back(ParseObjIndex(tokens[2 + i]));
						obj_faces.push_back(ParseObjIndex(tokens[3 + i]));
					}
				}
			}
		}
	}

	const bool has_normals = !normals.empty();
	const bool has_texcoords = !texcoords.empty();

	std::vector<Vertex> final_vertices;
	for (auto face : obj_faces) {
		Vertex vertex;

		// get the position
		Vector3 pos = positions[face.vertex];
		vertex.x = pos.x;
		vertex.y = pos.y;
		vertex.z = pos.z;

		// get the texcoord (if there is one)
		if (has_texcoords) {
			Vector2 texcoord = texcoords[face.texcoord];
			vertex.s = texcoord.x;
			vertex.t = texcoord.y;
		}

		// get the normal (if there is one)
		if (has_normals) {
			Vector3 norm = normals[face.normal];
			vertex.nx = norm.x;
			vertex.ny = norm.y;
			vertex.nz = norm.z;
		}

		final_vertices.push_back(vertex);
	}

	std::vector<unsigned int> final_faces;
	for (size_t i = 0; i < final_vertices.size(); i++) {
		final_faces.push_back(i);
	}

	return std::make_shared<Mesh>(final_vertices, final_faces);
}