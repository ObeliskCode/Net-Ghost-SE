#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{    
    float gamma = 1.05f;
	vec4 sample = texture(skybox, TexCoords);
    sample.rgb = pow(sample.rgb, vec3(1.0f/gamma));
    FragColor = sample;
}