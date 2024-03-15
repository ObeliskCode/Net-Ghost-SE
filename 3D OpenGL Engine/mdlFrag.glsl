#version 330 core

// Final color of pixel
out vec4 FragmentColor;

// Imports the current position we calculated
in vec3 crntPos;

// Imports the normal from the Vertex Shader
in vec3 Normal;

// Imports the texture coordinates from the Vertex Shader
in vec2 texCoord;

// light uniforms
uniform vec4 lightColor;
uniform vec3 lightPos;

// Mesh uniform
uniform vec3 camPos;

// Textures
uniform sampler2D diffuse0;
uniform sampler2D specular0;

vec4 pointLight()
{
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightPos - crntPos);

	float diffuseLight = 0.8f;
	float diffuseAmount = max(dot(normal, lightDirection), 0.0f);
	float diffuse = diffuseAmount * diffuseLight;


	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	return (texture(diffuse0, texCoord) * (diffuse) + texture(specular0, texCoord).r * specular) * lightColor;
}

vec4 direcLight()
{
	// ambient lighting
	float ambient = 0.4f;

	// diffuse lighting
	float diffuseLight = 0.35f;
	vec3 normal = normalize(Normal);
	vec3 lightDirection = -normalize(vec3(-1.0f, -10.0f, 10.0f));
	float diffuseAmount = max(dot(normal, lightDirection), 0.0f);
	float diffuse = diffuseAmount * diffuseLight;

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	return (texture(diffuse0, texCoord) * (diffuse + ambient) + texture(specular0, texCoord).r * specular) * lightColor;
}


void main()
{
	FragmentColor = pointLight() + direcLight();
}