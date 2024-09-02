#version 330 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D sprite;

void main()
{
    vec4 texColor = texture(sprite, TexCoords);
    color = texColor;
}