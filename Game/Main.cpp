#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <chrono>

#include "Mesh.h"
#include "Shader.h"
#include "GameObject.h"
#include "InputManager.h"
#include "Camera.h"
#include "Texture.h"
#include "Color.h"
#include "PointLight.h"

#include "ModelLoaders/ObjLoader.h"

#include "Math/matrix4.h"
#include "Math/matrix_util.h"

#include "PhysicsWorld.h"

// imgui
#include <imgui.h>
#include "imgui/imgui_impl_glfw_gl3.h"

// holds a list of all objects in the game
std::vector<std::shared_ptr<GameObject>> objects;

Shader *my_shader = nullptr;
InputManager *input_mgr = nullptr;
Camera *camera = nullptr;
PhysicsWorld *physics_world = nullptr;

Color ambience;

std::vector<PointLight> point_lights;

void Update(const double delta_time)
{
	// update physics
	physics_world->Update(delta_time);

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

	// apply scene ambience color
	my_shader->SetUniformVector4("u_ambientColor", ambience);

	// set point lights
	my_shader->SetUniformInt("u_numPointLights", point_lights.size());
	for (int i = 0; i < point_lights.size() && i < 4; i++) {
		my_shader->SetUniformVector3("u_pointLight[" + std::to_string(i) + "].position", point_lights[i].position);
		my_shader->SetUniformVector4("u_pointLight[" + std::to_string(i) + "].color", point_lights[i].color);
	}

	// set camera position
	my_shader->SetUniformVector3("u_cameraPosition", camera->GetPosition());

	// loop through all objects and draw each
	for (auto &&obj : objects) {
		if (obj->GetMesh() != nullptr) {
			my_shader->SetUniformMatrix("u_modelMatrix", obj->GetMatrix());

			// set material parameters
			my_shader->SetUniformFloat("u_roughness", obj->GetMesh()->GetMaterial().GetRoughness());
			my_shader->SetUniformFloat("u_shininess", obj->GetMesh()->GetMaterial().GetShininess());

			const bool has_diffuse_map = obj->GetMesh()->GetMaterial().GetDiffuseMap() != nullptr;

			if (has_diffuse_map) {
				// this model has a diffuse texture
				glActiveTexture(GL_TEXTURE0);
				obj->GetMesh()->GetMaterial().GetDiffuseMap()->Bind();
				my_shader->SetUniformInt("u_diffuseTexture", 0);
				my_shader->SetUniformInt("u_hasTexture", true);
			} else {
				my_shader->SetUniformInt("u_hasTexture", false);
			}

			obj->GetMesh()->Draw();

			if (has_diffuse_map) {
				obj->GetMesh()->GetMaterial().GetDiffuseMap()->Unbind();
			}
		}
	}

	my_shader->End();
}

std::shared_ptr<GameObject> LoadLandscape()
{
	auto landscape = GameObject::Load("landscape.m5m");
	auto grass = std::make_shared<Texture>("textures/grass.jpg");
	landscape->GetMesh()->GetMaterial().SetDiffuseMap(grass);
	return landscape;
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
		"Mach5 Engine", NULL, NULL);

	if (!window) {
		glfwTerminate();
		return false;
	}

	glfwSetKeyCallback(window, KeyCallback);
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		throw std::exception("error initializing glew");
	}

	// Setup ImGui binding
	ImGui_ImplGlfwGL3_Init(window, false);


	physics_world = new PhysicsWorld();

	// initialize global input manager
	input_mgr = new InputManager();
	input_mgr->window = window;
	// initialize main camera
	camera = new Camera(1080, 720);
	// initialize main shader
	my_shader = NewShader();
	// initialize test texture
	// set the scene's ambience color
	ambience = Color(0.1, 0.25, 0.4, 1.0);

	// default OBJ loader
	ObjLoader loader;

	{
		auto start = std::chrono::high_resolution_clock::now();

		// load a test obj model
		auto box_mesh = loader.LoadMesh("models/pokestan.obj");
		// apply a gravel texture on the mesh
		/*auto gravel = std::make_shared<Texture>("textures/gravel.jpg");
		box_mesh->GetMaterial().SetDiffuseMap(gravel);
		box_mesh->GetMaterial().SetShininess(0.6);
		box_mesh->GetMaterial().SetRoughness(0.4);*/

		auto box = std::make_shared<GameObject>();
		box->SetMesh(box_mesh);
		box->scale = Vector3(0.2);
		box->translation = Vector3(0, 30, 0);
		box->UpdateMatrix();
		//box->Save("duce.m5m");
		objects.push_back(box);
		physics_world->RegisterObject(box, 1.0);

		auto end = std::chrono::high_resolution_clock::now();
		auto duration = end - start;
		std::cout << std::chrono::duration<double, std::milli>(duration).count() << "ms to load obj box.\n";
	}

	{
		auto start = std::chrono::high_resolution_clock::now();

		auto box2 = GameObject::Load("duce.m5m");
		box2->translation += Vector3(0, 10, 0);
		box2->UpdateMatrix();
		objects.push_back(box2);
		physics_world->RegisterObject(box2, 1.0);

		auto end = std::chrono::high_resolution_clock::now();
		auto duration = end - start;
		std::cout << std::chrono::duration<double, std::milli>(duration).count() << "ms to load m5m box.\n";
	}
	{
		auto start = std::chrono::high_resolution_clock::now();

		auto monkey_mesh = loader.LoadMesh("models/monkow.obj");
		monkey_mesh->GetMaterial().SetShininess(0.25);
		monkey_mesh->GetMaterial().SetRoughness(0.25);
		auto monkey = std::make_shared<GameObject>();
		monkey->SetMesh(monkey_mesh);
		monkey->translation = Vector3(-6, 30, 0);
		monkey->scale = Vector3(0.25f);
		monkey->UpdateMatrix();
		objects.push_back(monkey);
		physics_world->RegisterObject(monkey, 1.0);
		//monkey->Save("monkey.m5m");
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = end - start;
		std::cout << std::chrono::duration<double, std::milli>(duration).count() << "ms to load obj monkey.\n";
	}

	{
		auto start = std::chrono::high_resolution_clock::now();

		auto monkey = GameObject::Load("monkey.m5m");
		monkey->translation += Vector3(0, 10, 0);
		monkey->UpdateMatrix();
		objects.push_back(monkey);
		physics_world->RegisterObject(monkey, 1.0);

		auto end = std::chrono::high_resolution_clock::now();
		auto duration = end - start;
		std::cout << std::chrono::duration<double, std::milli>(duration).count() << "ms to load m5m monkey.\n";
	}

	auto landscape = LoadLandscape();
	objects.push_back(landscape);
	physics_world->RegisterObject(landscape, 0.0);

	glfwSwapInterval(1);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glClearColor(ambience.x * 2, ambience.y * 2, ambience.z * 2, ambience.w);

	bool show_test_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImColor(114, 144, 154);

	double last_time = 0;
	while (!glfwWindowShouldClose(window)) {

		ImGui_ImplGlfwGL3_NewFrame();

		double current_time = glfwGetTime();
		double delta_time = current_time - last_time;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//camera_dir.Rotate(Vector3::UnitY(), delta_time);
		int width, height;
		glfwGetWindowSize(window, &width, &height);

		glfwGetCursorPos(window, &input_mgr->mx, &input_mgr->my);
		input_mgr->mx = MathUtil::Clamp<double>(input_mgr->mx, 0, width);
		input_mgr->my = MathUtil::Clamp<double>(input_mgr->my, 0, height);

		Update(delta_time);
		Render();

		{
			static float f = 0.0f;
			ImGui::Text("Hello, world!");
			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
			ImGui::ColorEdit3("clear color", (float*)&clear_color);
			if (ImGui::Button("Test Window")) show_test_window ^= 1;
			if (ImGui::Button("Another Window")) show_another_window ^= 1;
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		}

		// 2. Show another simple window, this time using an explicit Begin/End pair
		if (show_another_window) {
			ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
			ImGui::Begin("Another Window", &show_another_window);
			ImGui::Text("Hello");
			ImGui::End();
		}

		// 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
		if (show_test_window) {
			ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
			ImGui::ShowTestWindow(&show_test_window);
		}
		ImGui::Render();

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
	delete physics_world;

	// Cleanup
	ImGui_ImplGlfwGL3_Shutdown();

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