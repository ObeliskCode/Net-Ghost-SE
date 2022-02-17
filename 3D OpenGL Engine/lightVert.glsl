#version 330 core

// Coordinates
layout (location = 0) in vec3 aPos;

// Normals
layout (location = 1) in vec3 aNormal;

// Colors
layout (location = 2) in vec3 aColor;

// Texture Coordinates
layout (location = 3) in vec2 aTex;


out vec3 crntPos;
out vec3 Normal;
out vec3 color;
out vec2 texCoord;

uniform mat4 lightModel;
uniform mat4 camMatrix;
uniform mat4 transform;

uniform mat4 model;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;

void main(){
	// calculates current position
	// lightmodel is a translation so it must go after scaling and rotation;
	crntPos = vec3(model * translation * lightModel * rotation * scale * transform * vec4(aPos, 1.0f));
	
	// Outputs the positions/coordinates of all vertices
	gl_Position = camMatrix * vec4(crntPos, 1.0);

	// Passing inputs to fragment shader
	Normal = aNormal;
	color = aColor;
	texCoord = aTex;
}