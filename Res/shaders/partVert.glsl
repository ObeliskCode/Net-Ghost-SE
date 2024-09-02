#version 330 core

// Coordinates
layout (location = 0) in vec3 aPos;

layout (location = 1) in vec2 aTex;

layout (location = 2) in vec4 M0;

layout (location = 3) in vec4 M1;

layout (location = 4) in vec4 M2;

layout (location = 5) in vec4 M3;

layout (location = 6) in float life;

out vec2 TexCoords;

out float lf;

uniform mat4 camMatrix;

void main()
{
    mat4 transform = mat4(M0,M1,M2,M3);

    lf = life;

    TexCoords = aTex;
    gl_Position = camMatrix * transform * vec4(aPos,1.0f);
}