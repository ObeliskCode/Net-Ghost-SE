#version 330 core

out vec4 FragColor;

uniform vec4 lightColor;

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

void main(){
	FragColor = texture(diffuse0, texCoord) * lightColor;
}