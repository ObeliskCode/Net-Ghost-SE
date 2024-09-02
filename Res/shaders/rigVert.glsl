#version 330 core

// Coordinates
layout (location = 0) in vec3 aPos;

// Normals
layout (location = 1) in vec3 aNormal;

// Texture Coordinates
layout (location = 2) in vec2 aTex;


out vec3 crntPos;
out vec3 Normal;
out vec2 texCoord;
out vec4 fragPosLight;

uniform mat4 camMatrix;
uniform mat4 model;

uniform mat4 transform;
uniform mat4 ntransform;

uniform mat4 lightSpaceMatrix;

void main(){
	// calculates physical position within world
	vec4 trunc = transform * model * vec4(aPos, 1.0f);
	crntPos =  trunc.xyz / trunc.w;
	//crntPos = vec3(trunc);

	// camera/screen space coordinates
	gl_Position = camMatrix * vec4(crntPos, 1.0);

	fragPosLight = lightSpaceMatrix * vec4(crntPos, 1.0f);

	// Normals need to be transformed aswell (will i need to apply model, rotations and non-uniform scales here too?)
	// normals should be translation and uniform scaling safe.
	Normal = normalize(vec3(ntransform * model * vec4(aNormal, 0.0)));

	texCoord = aTex;
}