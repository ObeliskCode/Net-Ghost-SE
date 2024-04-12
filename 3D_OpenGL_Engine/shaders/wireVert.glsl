#version 330 core

// Coordinates
layout (location = 0) in vec3 aPos;

uniform mat4 camMatrix;

uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;

void main(){
	gl_Position = camMatrix * translation * rotation * scale * vec4(aPos, 1.0);
}