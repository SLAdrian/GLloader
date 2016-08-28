#pragma once

#include <glm\glm.hpp>
#include <SDL.h>
#include "Camera.h"
#include "Cube.h"
#include "Lights.h"
#define WINDOW_X 1280
#define WINDOW_Y 720

struct mPointLight {
	Cube cube;
	PointLight data;
	bool active;
};

struct mSpotLight {
	SpotLight data;
	bool active;
};

struct Config {
	struct GL {
		Camera camera;
		glm::vec4 clear;
		glm::mat4 projection;

	};

	struct SDL {
		const Uint8* keys;
		Uint8* mKeys; //Keys last frame
		int mSize; //Key size
	};

	struct OBJ {
		std::vector<mPointLight*> points;
		std::vector<mSpotLight*> spots;
		std::vector<Cube*> mCubes;
		std::vector<Cube*>::iterator iCubes;
	};

	struct IMGUI {
		static float xyz[3];
		static float xyz1[3];
		static float lincst[2];
	};

	SDL_Window* window;
	GL gl;
	SDL sdl;
	OBJ obj;
	IMGUI gui;
	int mouseX;
	int mouseY;
	bool firstMouse;
	float delta, lastFrame, currentFrame;
	bool flashlight;

};


void deleteConfig(Config* config);
