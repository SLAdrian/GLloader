#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GL\gl3w.h>
#include "Definitions.h"
#include "Lights.h"
#include <vector>

class Shader
{
public:
	GLuint program;

	Shader(const GLchar * vertexPath, const GLchar * fragmentPath);
	void use();
	virtual void setup();
};


class CubeShader : public Shader
{
public:
	CubeShader(const GLchar * vertexPath, const GLchar * fragmentPath);
	void setup(Config* config, glm::vec3 color, float shine, glm::mat4 model);
};

class SimpleShader : public Shader
{
public:
	SimpleShader(const GLchar * vertexPath, const GLchar * fragmentPath);
	void setup(Config* config, glm::vec3 color, glm::mat4 model);
};

class TexturedCubeShader : public Shader 
{
public:
	TexturedCubeShader(const GLchar * vertexPath, const GLchar * fragmentPath);
	void setup(Config* config, float shine, glm::mat4 model, GLuint tex, GLuint spec);
};

class ModelShader : public Shader
{
public:
	ModelShader(const GLchar * vertexPath, const GLchar * fragmentPath);
	void setup(Config* config, float shine, glm::mat4 model);
};




