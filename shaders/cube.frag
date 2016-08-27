#version 330 core

uniform float shine;
uniform vec3 color;

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

in vec3 FragPos;  
in vec3 Normal;  
  
out vec4 fragcolor;
  
uniform vec3 viewPos;
uniform PointLight light;
uniform PointLight[10] pLights;
uniform SpotLight[10] sLights;
uniform int sCount;
uniform int pCount;

vec3 calcPoint(PointLight light, vec3 normal, vec3 viewdir, vec3 fragpos);
vec3 calcSpot(SpotLight light, vec3 normal, vec3 viewdir, vec3 fragpos);

void main()
{
	
	vec3 norm = normalize(Normal);
	vec3 viewdir = normalize(viewPos - FragPos);
	vec3 result;

	for(int x = 0; x < pCount; x++)
		result += calcPoint(pLights[x], norm, viewdir, FragPos);

	for(int x = 0; x < sCount; x++)
		result += calcSpot(sLights[x], norm, viewdir, FragPos);
	
	 fragcolor = vec4(result, 1.0);
} 


vec3 calcPoint(PointLight light, vec3 normal, vec3 viewdir, vec3 fragpos)
{
	vec3 lightdir = normalize(light.position - fragpos);
	//Dif
	float diff = max(dot(normal, lightdir), 0.0);

	//Spec
	vec3 reflectdir = reflect(-lightdir, normal);
	float spec = pow(max(dot(viewdir, reflectdir),0.0), shine);

	//Atten
	float distance = length(light.position - fragpos);
    float attenuation = 1.0f / (1.0 + light.linear * distance + light.quadratic * (distance * distance));  

	//Comb
	vec3 ambient = light.ambient * color;
	vec3 diffuse = light.diffuse * diff * color;
	vec3 specular = light.specular * spec;
	
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);
}

vec3 calcSpot(SpotLight light, vec3 normal, vec3 viewdir, vec3 fragpos)
{
	//amb
	vec3 ambient = light.ambient * color;

	//Dif
	vec3 lightDir = normalize(light.position - fragpos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * color;

	//Spec
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewdir, reflectDir), 0.0), shine);
	vec3 specular = light.specular * spec;

	//Spotlight
	float theta = dot(lightDir, normalize(-light.direction));
	float epison = (light.innerCutoff - light.outerCutoff);
	float intensity = clamp((theta - light.outerCutoff) / epison, 0.0, 1.0);
	diffuse *= intensity;
	specular *= intensity;

	//Atten
	float distance = length(light.position - fragpos);
	float attenuation = 1.0f / (1.0 + light.linear * distance + light.quadratic * (distance * distance));    
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	
	return (ambient + diffuse + specular);
}