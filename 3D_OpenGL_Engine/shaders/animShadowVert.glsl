#version 330 core
layout (location = 0) in vec3 aPos;

layout(location = 3) in ivec4 boneIds;

layout(location = 4) in vec4 weights;

uniform mat4 model;

uniform mat4 transform;

uniform mat4 lightSpaceMatrix;

const int MAX_BONES = 200;
const int MAX_BONE_INFLUENCE = 4; // change this to 8? // maybe uniform int boneIds[] like finalBonesMatrices[]
uniform mat4 finalBonesMatrices[MAX_BONES];

void main(){
	vec4 totalPosition = vec4(0.0f);
    if(boneIds[0] == -1)
    {
        totalPosition = model * vec4(aPos,1.0f);
    }
    else
    {
        for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
        {
            if(boneIds[i] == -1) 
                continue;
            if(boneIds[i] >=MAX_BONES) 
            {
                totalPosition = vec4(aPos,1.0f);
                break;
            }
            vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(aPos,1.0f);
            totalPosition += localPosition * weights[i];
        } 
    }

    gl_Position =  lightSpaceMatrix * transform * totalPosition;
}