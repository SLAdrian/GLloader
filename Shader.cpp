#include "Shader.h"



Shader::Shader(const GLchar * vertexPath, const GLchar * fragmentPath)
{
	// 1. Retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	// ensures ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::badbit);
	try
	{
		// Open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		// Read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// Convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	const GLchar* vShaderCode = vertexCode.c_str();
	const GLchar * fShaderCode = fragmentCode.c_str();
	// 2. Compile shaders
	GLuint vertex, fragment;
	GLint success;
	GLchar infoLog[512];
	// Vertex Shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	// Print compile errors if any
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	// Print compile errors if any
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Shader Program
	this->program = glCreateProgram();
	glAttachShader(this->program, vertex);
	glAttachShader(this->program, fragment);
	glLinkProgram(this->program);
	// Print linking errors if any
	glGetProgramiv(this->program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(this->program, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	// Delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(vertex);
	glDeleteShader(fragment);

}

void Shader::use()
{
	glUseProgram(this->program);
	return;
}

void Shader::setup()
{
	return;
}

CubeShader::CubeShader(const GLchar * vertexPath, const GLchar * fragmentPath) : Shader(vertexPath, fragmentPath)
{
	return;
}

void CubeShader::setup(Config* config, glm::vec3 color, float shine, glm::mat4 model)
{
	this->use();

	std::vector<mPointLight*>* pointVector = &config->obj.points;
	std::vector<mSpotLight*>* spotVector = &config->obj.spots;

	//Matrices
	glUniformMatrix4fv(glGetUniformLocation(this->program, "model"), 1, false, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(this->program, "view"), 1, false, glm::value_ptr(config->gl.camera.GetViewMatrix()));
	glUniformMatrix4fv(glGetUniformLocation(this->program, "projection"), 1, false, glm::value_ptr(config->gl.projection));

	//Properties
	glUniform3f(glGetUniformLocation(this->program, "color"), color.r, color.g, color.b);
	glUniform1f(glGetUniformLocation(this->program, "shine"), shine);
	glUniform3f(glGetUniformLocation(this->program, "viewPos"), config->gl.camera.Position.x, config->gl.camera.Position.y, config->gl.camera.Position.z);


	//Lighting
	//Directional lights

	//PointLights
	int count = pointVector->size();
	glUniform1i(glGetUniformLocation(this->program, "pCount"), count);
	for (std::vector<mPointLight*>::iterator it = pointVector->begin(); it != pointVector->end(); it++)
	{
		int i = it - pointVector->begin();
		PointLight light = (*it)->data; //Double pointer -> data is POINTLIGHT

		glUniform3f(glGetUniformLocation(this->program, ("pLights[" + std::to_string(i) + "].position").c_str()), light.position.x, light.position.y, light.position.z);
		glUniform3f(glGetUniformLocation(this->program, ("pLights[" + std::to_string(i) + "].ambient").c_str()), light.ambient.r, light.ambient.g, light.ambient.b);
		glUniform3f(glGetUniformLocation(this->program, ("pLights[" + std::to_string(i) + "].diffuse").c_str()), light.diffuse.r, light.diffuse.g, light.diffuse.b);
		glUniform3f(glGetUniformLocation(this->program, ("pLights[" + std::to_string(i) + "].specular").c_str()), light.specular.r, light.specular.g, light.specular.b);

		glUniform1f(glGetUniformLocation(this->program, ("pLights[" + std::to_string(i) + "].linear").c_str()), light.linear);
		glUniform1f(glGetUniformLocation(this->program, ("pLights[" + std::to_string(i) + "].quadratic").c_str()), light.quadratic);
	}

	//SpotLights
	count = spotVector->size();
	glUniform1i(glGetUniformLocation(this->program, "sCount"), count);
	for (std::vector<mSpotLight*>::iterator it = spotVector->begin(); it != spotVector->end(); it++)
	{
		int i = it - spotVector->begin();
		SpotLight light = (*it)->data;

		glUniform3f(glGetUniformLocation(this->program, ("sLights[" + std::to_string(i) + "].position").c_str()), light.position.x, light.position.y, light.position.z);
		glUniform3f(glGetUniformLocation(this->program, ("sLights[" + std::to_string(i) + "].direction").c_str()), light.direction.x, light.direction.y, light.direction.z);
		glUniform3f(glGetUniformLocation(this->program, ("sLights[" + std::to_string(i) + "].ambient").c_str()), light.ambient.r, light.ambient.g, light.ambient.b);
		glUniform3f(glGetUniformLocation(this->program, ("sLights[" + std::to_string(i) + "].diffuse").c_str()), light.diffuse.r, light.diffuse.g, light.diffuse.b);
		glUniform3f(glGetUniformLocation(this->program, ("sLights[" + std::to_string(i) + "].specular").c_str()), light.specular.r, light.specular.g, light.specular.b);

		glUniform1f(glGetUniformLocation(this->program, ("sLights[" + std::to_string(i) + "].linear").c_str()), light.linear);
		glUniform1f(glGetUniformLocation(this->program, ("sLights[" + std::to_string(i) + "].quadratic").c_str()), light.quadratic);
		glUniform1f(glGetUniformLocation(this->program, ("sLights[" + std::to_string(i) + "].innerCutoff").c_str()), light.innerCutoff);
		glUniform1f(glGetUniformLocation(this->program, ("sLights[" + std::to_string(i) + "].outerCutoff").c_str()), light.outerCutoff);
	}

}

SimpleShader::SimpleShader(const GLchar * vertexPath, const GLchar * fragmentPath) : Shader(vertexPath, fragmentPath)
{
	return;
}

void SimpleShader::setup(Config * config, glm::vec3 color, glm::mat4 model)
{
	this->use();


	//Matrices
	glUniformMatrix4fv(glGetUniformLocation(this->program, "model"), 1, false, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(this->program, "view"), 1, false, glm::value_ptr(config->gl.camera.GetViewMatrix()));
	glUniformMatrix4fv(glGetUniformLocation(this->program, "projection"), 1, false, glm::value_ptr(config->gl.projection));

	//Properties
	glUniform3f(glGetUniformLocation(this->program, "color"), color.r, color.g, color.b);
}

TexturedCubeShader::TexturedCubeShader(const GLchar * vertexPath, const GLchar * fragmentPath) : Shader(vertexPath, fragmentPath)
{
	return;
}

void TexturedCubeShader::setup(Config* config, float shine, glm::mat4 model, GLuint tex,GLuint spec = 0)
{
	this->use();
	std::vector<mPointLight*>* pointVector = &config->obj.points;
	std::vector<mSpotLight*>* spotVector = &config->obj.spots;

	//Matrices
	glUniformMatrix4fv(glGetUniformLocation(this->program, "model"), 1, false, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(this->program, "view"), 1, false, glm::value_ptr(config->gl.camera.GetViewMatrix()));
	glUniformMatrix4fv(glGetUniformLocation(this->program, "projection"), 1, false, glm::value_ptr(config->gl.projection));

	//Properties
	glUniform1f(glGetUniformLocation(this->program, "shine"), shine);
	glUniform3f(glGetUniformLocation(this->program, "viewPos"), config->gl.camera.Position.x, config->gl.camera.Position.y, config->gl.camera.Position.z);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glUniform1i(glGetUniformLocation(this->program, "Texture"), 0);

	if (spec != 0)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, spec);
		glUniform1i(glGetUniformLocation(this->program, "Specular"), 1);
	}



	//Lighting
	//Directional lights

	//PointLights
	int count = pointVector->size();
	glUniform1i(glGetUniformLocation(this->program, "pCount"), count);
	for (std::vector<mPointLight*>::iterator it = pointVector->begin(); it != pointVector->end(); it++)
	{
		int i = it - pointVector->begin();
		PointLight light = (*it)->data;

		glUniform3f(glGetUniformLocation(this->program, ("pLights[" + std::to_string(i) + "].position").c_str()), light.position.x, light.position.y, light.position.z);
		glUniform3f(glGetUniformLocation(this->program, ("pLights[" + std::to_string(i) + "].ambient").c_str()), light.ambient.r, light.ambient.g, light.ambient.b);
		glUniform3f(glGetUniformLocation(this->program, ("pLights[" + std::to_string(i) + "].diffuse").c_str()), light.diffuse.r, light.diffuse.g, light.diffuse.b);
		glUniform3f(glGetUniformLocation(this->program, ("pLights[" + std::to_string(i) + "].specular").c_str()), light.specular.r, light.specular.g, light.specular.b);

		glUniform1f(glGetUniformLocation(this->program, ("pLights[" + std::to_string(i) + "].linear").c_str()), light.linear);
		glUniform1f(glGetUniformLocation(this->program, ("pLights[" + std::to_string(i) + "].quadratic").c_str()), light.quadratic);
	}

	//SpotLights
	count = spotVector->size();
	glUniform1i(glGetUniformLocation(this->program, "sCount"), count);
	for (std::vector<mSpotLight*>::iterator it = spotVector->begin(); it != spotVector->end(); it++)
	{
		int i = it - spotVector->begin();
		SpotLight light = (*it)->data;

		glUniform3f(glGetUniformLocation(this->program, ("sLights[" + std::to_string(i) + "].position").c_str()), light.position.x, light.position.y, light.position.z);
		glUniform3f(glGetUniformLocation(this->program, ("sLights[" + std::to_string(i) + "].direction").c_str()), light.direction.x, light.direction.y, light.direction.z);
		glUniform3f(glGetUniformLocation(this->program, ("sLights[" + std::to_string(i) + "].ambient").c_str()), light.ambient.r, light.ambient.g, light.ambient.b);
		glUniform3f(glGetUniformLocation(this->program, ("sLights[" + std::to_string(i) + "].diffuse").c_str()), light.diffuse.r, light.diffuse.g, light.diffuse.b);
		glUniform3f(glGetUniformLocation(this->program, ("sLights[" + std::to_string(i) + "].specular").c_str()), light.specular.r, light.specular.g, light.specular.b);

		glUniform1f(glGetUniformLocation(this->program, ("sLights[" + std::to_string(i) + "].linear").c_str()), light.linear);
		glUniform1f(glGetUniformLocation(this->program, ("sLights[" + std::to_string(i) + "].quadratic").c_str()), light.quadratic);
		glUniform1f(glGetUniformLocation(this->program, ("sLights[" + std::to_string(i) + "].innerCutoff").c_str()), light.innerCutoff);
		glUniform1f(glGetUniformLocation(this->program, ("sLights[" + std::to_string(i) + "].outerCutoff").c_str()), light.outerCutoff);
	}
}

ModelShader::ModelShader(const GLchar * vertexPath, const GLchar * fragmentPath) : Shader(vertexPath, fragmentPath)
{
	return;
}

void ModelShader::setup(Config * config, float shine, glm::mat4 model)
{
	this->use();
	std::vector<mPointLight*>* pointVector = &config->obj.points;
	std::vector<mSpotLight*>* spotVector = &config->obj.spots;

	//Matrices
	glUniformMatrix4fv(glGetUniformLocation(this->program, "model"), 1, false, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(this->program, "view"), 1, false, glm::value_ptr(config->gl.camera.GetViewMatrix()));
	glUniformMatrix4fv(glGetUniformLocation(this->program, "projection"), 1, false, glm::value_ptr(config->gl.projection));

	//Properties
	glUniform1f(glGetUniformLocation(this->program, "material.shine"), shine);
	glUniform3f(glGetUniformLocation(this->program, "viewPos"), config->gl.camera.Position.x, config->gl.camera.Position.y, config->gl.camera.Position.z);
	

	//Lighting
	//Directional lights

	//PointLights
	int count = pointVector->size();
	glUniform1i(glGetUniformLocation(this->program, "pCount"), count);
	for (std::vector<mPointLight*>::iterator it = pointVector->begin(); it != pointVector->end(); it++)
	{
		int i = it - pointVector->begin();
		PointLight light = (*it)->data;

		glUniform3f(glGetUniformLocation(this->program, ("pLights[" + std::to_string(i) + "].position").c_str()), light.position.x, light.position.y, light.position.z);
		glUniform3f(glGetUniformLocation(this->program, ("pLights[" + std::to_string(i) + "].ambient").c_str()), light.ambient.r, light.ambient.g, light.ambient.b);
		glUniform3f(glGetUniformLocation(this->program, ("pLights[" + std::to_string(i) + "].diffuse").c_str()), light.diffuse.r, light.diffuse.g, light.diffuse.b);
		glUniform3f(glGetUniformLocation(this->program, ("pLights[" + std::to_string(i) + "].specular").c_str()), light.specular.r, light.specular.g, light.specular.b);

		glUniform1f(glGetUniformLocation(this->program, ("pLights[" + std::to_string(i) + "].linear").c_str()), light.linear);
		glUniform1f(glGetUniformLocation(this->program, ("pLights[" + std::to_string(i) + "].quadratic").c_str()), light.quadratic);
	}

	//SpotLights
	count = spotVector->size();
	glUniform1i(glGetUniformLocation(this->program, "sCount"), count);

	for (std::vector<mSpotLight*>::iterator it = spotVector->begin(); it != spotVector->end(); it++)
	{
		int i = it - spotVector->begin();
		SpotLight light = (*it)->data;

		glUniform3f(glGetUniformLocation(this->program, ("sLights[" + std::to_string(i) + "].position").c_str()), light.position.x, light.position.y, light.position.z);
		glUniform3f(glGetUniformLocation(this->program, ("sLights[" + std::to_string(i) + "].direction").c_str()), light.direction.x, light.direction.y, light.direction.z);
		glUniform3f(glGetUniformLocation(this->program, ("sLights[" + std::to_string(i) + "].ambient").c_str()), light.ambient.r, light.ambient.g, light.ambient.b);
		glUniform3f(glGetUniformLocation(this->program, ("sLights[" + std::to_string(i) + "].diffuse").c_str()), light.diffuse.r, light.diffuse.g, light.diffuse.b);
		glUniform3f(glGetUniformLocation(this->program, ("sLights[" + std::to_string(i) + "].specular").c_str()), light.specular.r, light.specular.g, light.specular.b);

		glUniform1f(glGetUniformLocation(this->program, ("sLights[" + std::to_string(i) + "].linear").c_str()), light.linear);
		glUniform1f(glGetUniformLocation(this->program, ("sLights[" + std::to_string(i) + "].quadratic").c_str()), light.quadratic);
		glUniform1f(glGetUniformLocation(this->program, ("sLights[" + std::to_string(i) + "].innerCutoff").c_str()), light.innerCutoff);
		glUniform1f(glGetUniformLocation(this->program, ("sLights[" + std::to_string(i) + "].outerCutoff").c_str()), light.outerCutoff);
	}
}
