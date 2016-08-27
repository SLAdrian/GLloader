#include "Lights.h"

PointLight::PointLight()
{
}

PointLight::PointLight(glm::vec3 pos, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float lin, float quad)
{
	position = pos;
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	linear = lin;
	quadratic = quad;
}

SpotLight::SpotLight()
{
}

SpotLight::SpotLight(glm::vec3 pos, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float inner, float outer, float lin, float quad)
{
	position = pos;
	this->direction = direction;
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	outerCutoff = glm::cos(outer);
	innerCutoff = glm::cos(inner);
	linear = lin;
	quadratic = quad;
}
