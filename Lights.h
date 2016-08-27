#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class PointLight {
public:
	glm::vec3 position;
	glm::vec3 ambient, diffuse, specular;
	float linear, quadratic;
	PointLight();
	PointLight(glm::vec3 pos, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float lin, float quad);
};

class SpotLight {
public:
	glm::vec3 position;
	glm::vec3 direction;
	float innerCutoff, outerCutoff;
	glm::vec3 ambient, diffuse, specular;
	float linear, quadratic;
	SpotLight();
	SpotLight(glm::vec3 pos, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float inner, float outer, float lin, float quad);
};