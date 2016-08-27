//Includes "SDL2,GL3W,IMGUI, etc
#include <GL\gl3w.h>

#include <SDL.h>
#include "imgui\imgui.h"
#include "IM_BINDING.h"
#include "Definitions.h"
#include "Cube.h"
#include "Lights.h"
#include <iostream>
#include "Camera.h"
#include "Model.h"

void handleEvents(Config* config, SDL_Event* event, bool* exit);

void handleKeyboard(Config * config, SDL_Event * event);

void gui(Config* config);

int main(int argc, char* argv[])
{

	//SDL Config
	Config config;
	SDL_GLContext glcontext;

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_DisplayMode current;
	SDL_GetCurrentDisplayMode(0, &current);

	config.window = SDL_CreateWindow("World Loader", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_X, WINDOW_Y, SDL_WINDOW_OPENGL);
	glcontext = SDL_GL_CreateContext(config.window);
	SDL_GL_SetSwapInterval(1);

	gl3wInit();

	if (config.window == NULL) {
		std::cout << "Could not create window: %s\n", SDL_GetError();
		return 1;
	}
	ImGui_ImplSdlGL3_Init(config.window); // Init IMGUI binding


	//Init Config

	config.sdl.keys = SDL_GetKeyboardState(&config.sdl.mSize);
	config.sdl.mKeys = new Uint8[config.sdl.mSize];
	config.gl.camera = Camera(glm::vec3(0.0, 5.0, 0.0));
	config.gl.projection = glm::perspective((float)glm::radians(90.0), (float)WINDOW_X / (float)WINDOW_Y, 0.1f, 100.0f);
	config.gl.clear = glm::vec4(0.5); config.gl.clear.a = 1.0;
	config.delta = 0.0; config.lastFrame = 0.0; config.firstMouse = true;
	config.mouseX = WINDOW_X / 2; config.mouseY = WINDOW_Y / 2;

	//GLConfig

	glViewport(0, 0, WINDOW_X, WINDOW_Y);
	glClearColor(config.gl.clear.r, config.gl.clear.g, config.gl.clear.b, config.gl.clear.a);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	Cube cube;

	CubeShader shader("shaders/cube.vert", "shaders/cube.frag");
	SimpleShader lshader("shaders/simple.vert", "shaders/simple.frag");

	TexturedCube tcube;
	tcube.loadTextures("textures/box.png", "textures/box_spec.png");
	TexturedCubeShader tshader("shaders/texturedcube.vert", "shaders/texturedcube.frag");

	ModelShader mshader("shaders/model.vert", "shaders/model.frag");
	Model model("model/nanosuit.obj");

	//END

	//Vars



	SpotLight flashlight(config.gl.camera.Position, config.gl.camera.Front, glm::vec3(0.2), glm::vec3(0.8), glm::vec3(0.4), glm::radians(12.5), glm::radians(17.5), 0.022, 0.0019);
	config.obj.spots.push_back(&flashlight);




	//Main Loop
	bool exit = false;
	config.flashlight = true;
	while (!exit)
	{

		SDL_Event event;


		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSdlGL3_ProcessEvent(&event);
			handleEvents(&config, &event, &exit);
			memcpy(config.sdl.mKeys, config.sdl.keys, config.sdl.mSize * sizeof Uint8);
		}

		gui(&config);

		handleKeyboard(&config, &event);

		flashlight.position = config.gl.camera.Position;
		flashlight.direction = config.gl.camera.Front;


		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (config.obj.iCubes = config.obj.mCubes.begin(); config.obj.iCubes != config.obj.mCubes.end(); config.obj.iCubes++)
		{
			shader.setup(&config, glm::vec3(0.5, 0.2, 0.0), 32, (*(*config.obj.iCubes)->getModel()));
			(*config.obj.iCubes)->render();
		}

		for (std::vector<mPointLight*>::iterator it = config.obj.points.begin(); it != config.obj.points.end(); it++)
		{
			mPointLight light = *(*it);

			lshader.setup(&config, glm::vec3(1.0), *light.cube.getModel());
			light.cube.render();
		}

		mshader.setup(&config, 32.0, glm::translate(glm::mat4(), glm::vec3(10.0)));
		model.Draw(mshader);
		ImGui::Render();

		SDL_GL_SwapWindow(config.window);
		config.currentFrame = SDL_GetTicks() / 500.0;
		config.delta = config.currentFrame - config.lastFrame;
		config.lastFrame = config.currentFrame;



		int x = glGetError();

		if (x != 0)
			std::cout << x << '\n';

	}

	deleteConfig(&config);
	ImGui_ImplSdlGL3_Shutdown();
	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(config.window);
	SDL_Quit();

	return 0;
}


void handleEvents(Config* config, SDL_Event* event, bool* exit)
{
	int xpos, ypos;
	SDL_GetMouseState(&xpos, &ypos);
	if (event->type == SDL_QUIT || config->sdl.keys[SDL_SCANCODE_ESCAPE]) *exit = true;

	if (event->type == SDL_MOUSEMOTION && !config->sdl.keys[SDL_SCANCODE_L])
	{
		int xoffset = event->motion.xrel;
		int yoffset = -event->motion.yrel;

		config->gl.camera.ProcessMouseMovement(xoffset, yoffset);
	}

	if (config->sdl.keys[SDL_SCANCODE_L] && !config->sdl.mKeys[SDL_SCANCODE_L])
		SDL_SetRelativeMouseMode(SDL_FALSE);

	else if (!config->sdl.keys[SDL_SCANCODE_L] && config->sdl.mKeys[SDL_SCANCODE_L])
		SDL_SetRelativeMouseMode(SDL_TRUE);


}

void handleKeyboard(Config* config, SDL_Event* event)
{
	if (config->sdl.keys[SDL_SCANCODE_LSHIFT])
		config->gl.camera.MovementSpeed *= 4.0;

	if (config->sdl.keys[SDL_SCANCODE_W])
		config->gl.camera.ProcessKeyboard(FORWARD, config->delta);

	if (config->sdl.keys[SDL_SCANCODE_S])
		config->gl.camera.ProcessKeyboard(BACKWARD, config->delta);

	if (config->sdl.keys[SDL_SCANCODE_A])
		config->gl.camera.ProcessKeyboard(LEFT, config->delta);

	if (config->sdl.keys[SDL_SCANCODE_D])
		config->gl.camera.ProcessKeyboard(RIGHT, config->delta);

	if (config->sdl.keys[SDL_SCANCODE_SPACE])
		config->gl.camera.Position.y += 1 * config->delta;

	if (config->sdl.keys[SDL_SCANCODE_LCTRL])
		config->gl.camera.Position.y -= 1 * config->delta;

	config->gl.camera.MovementSpeed = SPEED;
}

void gui(Config * config)
{
	ImGui_ImplSdlGL3_NewFrame(config->window);
	{
		ImGui::SetNextWindowPosCenter();
		ImGui::SetNextWindowSize(ImVec2(320, 500));
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::Begin("Loader");

		if (ImGui::CollapsingHeader("Settings"))
		{
			if (ImGui::Button("Toggle Flashlight"))
			{

			}
		}

		if (ImGui::CollapsingHeader("Drawing"))
		{
			ImGui::InputFloat3("X Y Z", config->gui.xyz);

			if (ImGui::TreeNode("Cubes")) 
			{
				if (ImGui::Button("Add cube"))
				{
					Cube *c = new Cube;
					*c->getModel() = glm::translate(glm::mat4(), glm::vec3(config->gui.xyz[0], config->gui.xyz[1], config->gui.xyz[2]));
					config->obj.mCubes.push_back(c);
				}
				ImGui::TreePop();

			}
		}

		if (ImGui::CollapsingHeader("Lighting"))
		{
			ImGui::InputFloat3("XlllYZ", config->gui.xyz1);

			if (ImGui::TreeNode("Pointlight")) 
			{
				if (ImGui::Button("Add light"))
				{
					mPointLight *light = new mPointLight;
					(*light->cube.getModel()) = glm::scale(glm::mat4(), glm::vec3(0.2));

					(*light->cube.getModel()) = glm::translate((*light->cube.getModel()), glm::vec3(config->gui.xyz1[0], config->gui.xyz1[1], config->gui.xyz1[2]));

					light->data = PointLight(glm::vec3(config->gui.xyz[0], config->gui.xyz[1], config->gui.xyz[2]), glm::vec3(0.2), glm::vec3(0.4), glm::vec3(0.8), 0.9, .032);
					config->obj.points.push_back(light);
				}
				ImGui::TreePop();
			}
		}

		ImGui::End();
	}
}
