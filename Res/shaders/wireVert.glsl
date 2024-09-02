#version 330 core

// Coordinates
layout (location = 0) in vec3 aPos;

uniform mat4 camMatrix;

uniform mat4 transform;

void main(){
	gl_Position = camMatrix * transform * vec4(aPos, 1.0);
}