#version 330 core

// Coordinates
layout (location = 0) in vec3 aPos;

// Normals
layout (location = 1) in vec3 aNormal;

// Texture Coordinates
layout (location = 2) in vec2 aTex;

layout(location = 3) in ivec4 boneIds;

layout(location = 4) in vec4 weights;

out vec3 crntPos;
out vec3 Normal;
out vec2 texCoord;
out vec4 fragPosLight;

uniform mat4 camMatrix;
uniform mat4 model;

uniform mat4 transform;
uniform mat4 ntransform;

uniform mat4 lightSpaceMatrix;

const int MAX_BONES = 200;
const int MAX_BONE_INFLUENCE = 4; // change this to 8? // maybe uniform int boneIds[] like finalBonesMatrices[]
uniform mat4 finalBonesMatrices[MAX_BONES];

void main(){
	vec4 totalPosition = vec4(0.0f);
    vec3 totalNormal = vec3(0.0f);
    if(boneIds[0] == -1)
    {
        totalPosition = model * vec4(aPos,1.0f);
        totalNormal = vec3(model * vec4(aNormal,0.0f));
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
            vec3 localNormal = mat3(finalBonesMatrices[boneIds[i]]) * aNormal;
            totalNormal += localNormal;
        } 
    }

    // fix later, is this correct?
    Normal = vec3(ntransform * vec4(totalNormal,0.0f));

    vec4 worldPos = transform * totalPosition;
     
    // transform not used since baked into bone transforms? w truncated!
    crntPos = worldPos.xyz / worldPos.w;
    //crntPos = vec3(worldPos);

	fragPosLight = lightSpaceMatrix * vec4(crntPos, 1.0f);

    // TAKING OFF W COMPONENT MESSES WITH ACCURACY BY A LOT! e.g. vec3(vec4Object)

    gl_Position =  camMatrix * worldPos;

    texCoord = aTex;
}