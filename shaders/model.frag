#version 330 core

out vec4 color;

in vec2 Texcoords;
in vec3 Normal;
in vec3 FragPos;

struct Material {
	sampler2D texture_diffuse1;
	sampler2D texture_diffuse2;
	sampler2D texture_diffuse3;
	sampler2D texture_diffuse4;

	sampler2D texture_specular1;
	sampler2D texture_specular2;
	sampler2D texture_specular3;
	sampler2D texture_specular4;
	float shine;
};

struct PointLight {
	vec3 position;

	vec3 diffuse;
	vec3 ambient;
	vec3 specular;
	float linear;
	float quadratic;
};

struct SpotLight {
    vec3 position;
	vec3 direction;

	vec3 diffuse;
	vec3 ambient;
	vec3 specular;

	float innerCutoff;
	float outerCutoff;
	float linear;
	float quadratic;
};

uniform vec3 viewPos;
uniform Material material;

uniform PointLight pLights[10];
uniform int pCount;

float constant = 1.0;

uniform SpotLight sLights[10];
uniform int sCount;

vec3 getPointLight(PointLight light, vec3 normal, vec3 viewdir, vec3 fragpos);
vec3 getSpotLight(SpotLight light, vec3 normal, vec3 viewdir, vec3 fragpos);


void main()
{
	vec3 result;
	vec3 Viewdir = normalize(viewPos - FragPos);
	for (int i = 0; i < pCount; i++)
		result += getPointLight(pLights[i], Normal, Viewdir, FragPos);

	for (int i = 0; i < sCount; i++)
		result += getSpotLight(sLights[i], Normal, Viewdir, FragPos);

	color = vec4(result, 1.0);
}

vec3 getPointLight(PointLight light, vec3 normal, vec3 viewdir, vec3 fragpos)
{
	
	vec3 lightdir = normalize(light.position - fragpos);
	//Dif
	float diff = max(dot(normal, lightdir), 0.0);

	//Spec
	vec3 reflectdir = reflect(-lightdir, normal);
	float spec = pow(max(dot(viewdir, reflectdir),0.0), material.shine);

	//Atten
	float distance = length(light.position - fragpos);
    float attenuation = 1.0f / ( 1.0 + light.linear * distance + light.quadratic * (distance * distance));  

	//Comb
	vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, Texcoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, Texcoords));
	vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, Texcoords));
	
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}

vec3 getSpotLight(SpotLight light, vec3 normal, vec3 viewdir, vec3 fragpos)
{
	//amb
	vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1,Texcoords));

	//Dif
	vec3 lightDir = normalize(light.position - fragpos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, Texcoords));

	//Spec
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewdir, reflectDir), 0.0), material.shine);
	vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, Texcoords));

	//Spotlight
	float theta = dot(lightDir, normalize(-light.direction));
	float epison = (light.innerCutoff - light.outerCutoff);
	float intensity = clamp((theta - light.outerCutoff) / epison, 0.0, 1.0);
	diffuse *= intensity;
	specular *= intensity;

	//Atten
	float distance = length(light.position - fragpos);
	float attenuation = 1.0f / ( 1.0 + light.linear * distance + light.quadratic * (distance * distance));    
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	
	return (ambient + diffuse + specular);
}