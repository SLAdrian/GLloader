#include "Definitions.h"

float Config::IMGUI::xyz[3];
float Config::IMGUI::xyz1[3];



void deleteConfig(Config* config)
{
	delete config->sdl.mKeys;

	for (config->obj.iCubes = config->obj.mCubes.begin(); config->obj.iCubes != config->obj.mCubes.end(); config->obj.iCubes++)
		delete (*config->obj.iCubes);

	for (std::vector<mPointLight*>::iterator it = config->obj.points.begin(); it != config->obj.points.end(); it++)
		delete (*it);

}


