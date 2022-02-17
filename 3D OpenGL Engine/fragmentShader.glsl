#version 330 core

// Final color of pixel
out vec4 FragmentColor;


// Imports the current position we calculated
in vec3 crntPos;

// Imports the normal from the Vertex Shader
in vec3 Normal;

// Imports the color from the Vertex Shader
in vec3 color;

// Imports the texture coordinates from the Vertex Shader
in vec2 texCoord;

uniform sampler2D diffuse0;
uniform sampler2D specular0;

uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;

void main()
{
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightPos - crntPos);

	float ambient = 0.3f;

	float diffuse = max(dot(normal, lightDirection), 0.0f);

	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 8);
	float specular = specAmount * specularLight;

	FragmentColor = (texture(diffuse0, texCoord) * (diffuse + ambient) + texture(specular0, texCoord).r * specular) * lightColor;
	
}