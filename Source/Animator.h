#pragma once

#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <Skeleton.h>
#include <Bone.h>

class Animator
{
public:
	Animator(Skeleton* skeleton)
	{
		m_CurrentTime = 0.0;
		m_DeltaTime = 0.0;
		m_Skeleton = skeleton;
		m_AnimationIndex = 0;
		m_CurrentAnimation = m_Skeleton->GetAnimation(m_AnimationIndex);

		#define MAX_BONES 200

		m_FinalBoneMatrices.reserve(MAX_BONES);

		for (int i = 0; i < MAX_BONES; i++)
			m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
	}

	~Animator() {
		delete m_Skeleton;
	}

	void UpdateAnimation(float dt)
	{
		m_DeltaTime = dt;
		if (m_Skeleton)
		{
			m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
			m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
			CalculateBoneTransform(&m_Skeleton->GetRootNode(), glm::mat4(1.0f));
		}
	}

	void PlayAnimation(unsigned int index)
	{
		m_AnimationIndex = index;
		m_CurrentAnimation = m_Skeleton->GetAnimation(m_AnimationIndex);
		m_CurrentTime = 0.0f;
	}

	void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
	{
		std::string nodeName = node->name;
		glm::mat4 nodeTransform = node->transformation;

		Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

		if (Bone)
		{
			Bone->Update(m_CurrentTime);
			nodeTransform = Bone->GetLocalTransform();
		}

		glm::mat4 globalTransformation = parentTransform * nodeTransform;

		auto boneInfoMap = m_Skeleton->GetBoneIDMap();
		if (boneInfoMap.find(nodeName) != boneInfoMap.end())
		{
			int index = boneInfoMap[nodeName].id;
			glm::mat4 offset = boneInfoMap[nodeName].offset;
			m_FinalBoneMatrices[index] = globalTransformation * offset;
		}

		for (int i = 0; i < node->childrenCount; i++)
			CalculateBoneTransform(&node->children[i], globalTransformation);
	}

	std::vector<glm::mat4> GetFinalBoneMatrices()
	{
		return m_FinalBoneMatrices;
	}

private:
	std::vector<glm::mat4> m_FinalBoneMatrices;
	Skeleton* m_Skeleton;
	Animation* m_CurrentAnimation;
	unsigned int m_AnimationIndex;
	float m_CurrentTime;
	float m_DeltaTime;

};