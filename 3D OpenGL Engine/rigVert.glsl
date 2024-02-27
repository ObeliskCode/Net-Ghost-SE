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

uniform mat4 camMatrix;
uniform mat4 transform;

uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;

void main(){
	// calculates physical position within world
	crntPos = vec3(translation * rotation * scale * transform * vec4(aPos, 1.0f)); // is it ok to truncate w?
	
	// camera/screen space coordinates
	gl_Position = camMatrix * vec4(crntPos, 1.0);

	// Normals need to be transformed aswell (will i need to apply model, rotations and non-uniform scales here too?)
	// normals should be translation and uniform scaling safe.
	Normal = normalize(vec3(rotation * scale * transform * vec4(aNormal, 0.0)));

	color = aColor;
	texCoord = aTex;
}