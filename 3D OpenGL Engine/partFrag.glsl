#version 330 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D sprite;

uniform float life;

void main()
{
    vec4 texColor = texture(sprite, TexCoords);
    float half = 1.0f/2.0f;
    float halfN = pow(half,3.0);
    texColor.a = texColor.a * ( -halfN*sin( half*life ) + halfN );

    float gamma = 1.05f;
	vec4 sample = texColor;
    sample.rgb = pow(sample.rgb, vec3(1.0f/gamma));
    color = sample;
}