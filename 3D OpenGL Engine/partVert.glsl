#version 330 core

// Coordinates
layout (location = 0) in vec3 aPos;

layout (location = 1) in vec2 aTex;

out vec2 TexCoords;

uniform mat4 camMatrix;

uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;

void main()
{
    TexCoords = aTex;
    gl_Position = camMatrix * translation * rotation * scale * vec4(aPos,1.0f);
}