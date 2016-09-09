#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

#include "Mesh.h"
#include "Shader.h"
#include "GameObject.h"
#include "InputManager.h"
#include "Camera.h"

#include "ModelLoaders/ObjLoader.h"

#include "Math/matrix4.h"
#include "Math/matrix_util.h"

// holds a list of all objects in the game
std::vector<std::shared_ptr<GameObject>> objects;

Shader *my_shader = nullptr;
InputManager *input_mgr = nullptr;
Camera *camera = nullptr;

void Update(const double delta_time)
{
	// update the camera
	camera->UpdateMouse(input_mgr, delta_time);
	camera->UpdateMovement(input_mgr, delta_time);
	camera->UpdateMatrices();
}

void Render()
{
	my_shader->Begin();
	// set camera parameters
	my_shader->SetUniformMatrix("u_viewMatrix", camera->GetViewMatrix());
	my_shader->SetUniformMatrix("u_projMatrix", camera->GetProjectionMatrix());
	my_shader->SetUniformFloat("red", 0.3f);
	// loop through all objects and draw each
	for (auto &&obj : objects) {
		if (obj->GetMesh() != nullptr) {
			my_shader->SetUniformMatrix("u_modelMatrix", obj->GetMatrix());
			obj->GetMesh()->Draw();
		}
	}

	my_shader->End();
}

std::shared_ptr<Mesh> CreateTestMesh()
{
	std::vector<Vertex> vertices;
	vertices.resize(3);

	//VERTEX 0
	vertices[0].x = 0.0;
	vertices[0].y = 0.0;
	vertices[0].z = 0.0;
	vertices[0].nx = 0.0;
	vertices[0].ny = 0.0;
	vertices[0].nz = 1.0;
	vertices[0].s = 0.0;
	vertices[0].t = 0.0;
	//VERTEX 1
	vertices[1].x = 1.0;
	vertices[1].y = 0.0;
	vertices[1].z = 0.0;
	vertices[1].nx = 0.0;
	vertices[1].ny = 0.0;
	vertices[1].nz = 1.0;
	vertices[1].s = 1.0;
	vertices[1].t = 0.0;
	//VERTEX 2
	vertices[2].x = 0.0;
	vertices[2].y = 1.0;
	vertices[2].z = 0.0;
	vertices[2].nx = 0.0;
	vertices[2].ny = 0.0;
	vertices[2].nz = 1.0;
	vertices[2].s = 0.0;
	vertices[2].t = 1.0;

	std::vector<unsigned int> indices;
	indices.resize(3); 
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	return std::make_shared<Mesh>(vertices, indices);
}

Shader *NewShader()
{
	// holds the actual source code
	std::string vertex_src, fragment_src;

	std::ifstream vertex_file;
	vertex_file.open("shaders\\shader.vert");
	std::stringstream vertex_ss;
	vertex_ss << vertex_file.rdbuf();
	vertex_src = vertex_ss.str();

	std::ifstream fragment_file;
	fragment_file.open("shaders\\shader.frag");
	std::stringstream fragment_ss;
	fragment_ss << fragment_file.rdbuf();
	fragment_src = fragment_ss.str();

	return new Shader(vertex_src, fragment_src);
}

static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	} else if (action == GLFW_PRESS) {
		input_mgr->keys[key] = true;
	} else if (action == GLFW_RELEASE) {
		input_mgr->keys[key] = false;
	}
}

static void ErrorCallback(int error, const char *description)
{
	std::cout << "Error: " << description << "\n";
}

bool Run()
{
	glfwSetErrorCallback(ErrorCallback);

	if (!glfwInit()) {
		return false;
	}

	auto *window = glfwCreateWindow(1080, 720,
		"My game", NULL, NULL);

	if (!window) {
		glfwTerminate();
		return false;
	}

	glfwSetKeyCallback(window, KeyCallback);
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		throw std::exception("error initializing glew");
	}

	input_mgr = new InputManager();
	input_mgr->window = window;

	camera = new Camera(1080, 720);

	my_shader = NewShader();


	// test load file
	ObjLoader loader;
	auto cow_mesh = loader.LoadMesh("models/Mountain Bike.obj");
	auto cow_object = std::make_shared<GameObject>();
	cow_object->SetMesh(cow_mesh);
	cow_object->scale = Vector3(0.02f);
	cow_object->UpdateMatrix();
	objects.push_back(cow_object);

	/*auto triangle = std::make_shared<GameObject>();
	triangle->SetMesh(CreateTestMesh());
	triangle->translation = Vector3(0, 0, 8);
	triangle->UpdateMatrix();
	objects.push_back(triangle);*/


	glfwSwapInterval(1);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.5f, 0.4f, 1.0f);

	float rot = 0.0f;

	double last_time = 0;
	while (!glfwWindowShouldClose(window)) {
		double current_time = glfwGetTime();
		double delta_time = current_time - last_time;

		rot += delta_time;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//camera_dir.Rotate(Vector3::UnitY(), delta_time);
		int width, height;
		glfwGetWindowSize(window, &width, &height);

		glfwGetCursorPos(window, &input_mgr->mx, &input_mgr->my);
		input_mgr->mx = MathUtil::Clamp<double>(input_mgr->mx, 0, width);
		input_mgr->my = MathUtil::Clamp<double>(input_mgr->my, 0, height);

		//MatrixUtil::ToLookAt(view_matrix, camera_pos, camera_pos + camera_dir, camera_up);
		//MatrixUtil::ToPerspective(proj_matrix, 45, 1080, 720, 1.0f, 50.0f);

		Update(delta_time);
		Render();

		glfwSwapBuffers(window);
		glfwPollEvents();

		last_time = current_time;
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	if (my_shader != nullptr) {
		delete my_shader;
		my_shader = nullptr;
	}

	delete input_mgr;
	delete camera;

	return true;
}

int main()
{
	if (!Run()) {
		std::cout << "Could not initialize game\n";
		system("pause");
		return 1;
	}
	return 0;
}