#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/string_cast.hpp>
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
		m_QueueCurrentTime = 0.0;
		m_BlendFactor = 0.0f;
		m_Skeleton = skeleton;
		m_AnimationIndex = 0;
		m_CurrentAnimation = m_Skeleton->GetAnimation(m_AnimationIndex);
		m_QueueAnimationIndex = -1;
		m_QueueAnimation = nullptr;

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
		if (m_BlendFactor == 0.0f) 
		{
			UpdateCurrentAnimation(dt);
		}
		else 
		{
			UpdateBlendedAnimation(dt);
		}
		
	}

	void PlayAnimation(unsigned int index) // only use to completely restart animation if playing!
	{
		if (index == m_QueueAnimationIndex) {
			m_QueueAnimation = nullptr;
			m_QueueAnimationIndex = -1;
		}
		m_AnimationIndex = index;
		m_CurrentAnimation = m_Skeleton->GetAnimation(m_AnimationIndex);
		m_CurrentTime = 0.0f;
		m_QueueCurrentTime = 0.0f;
		m_BlendFactor = 0.0f;
	}

	void DequeueAnimation()
	{
		if (m_QueueAnimation)
		{
			m_CurrentTime = m_QueueCurrentTime;
			m_AnimationIndex = m_QueueAnimationIndex;
			m_CurrentAnimation = m_Skeleton->GetAnimation(m_AnimationIndex);
			m_QueueAnimation = nullptr;
			m_QueueCurrentTime = 0.0f;
			m_QueueAnimationIndex = -1;
			m_BlendFactor = 0.0f;
		}
	}

	void QueueAnimation(unsigned int index)
	{
		if (index == m_AnimationIndex) return;
		m_QueueAnimationIndex = index;
		m_QueueAnimation = m_Skeleton->GetAnimation(m_QueueAnimationIndex);
		m_QueueCurrentTime = 0.0f;
		m_BlendFactor = 0.0f;
	}

	void SetBlendFactor(float bf)
	{
		m_BlendFactor = bf;
	}

	std::vector<glm::mat4> GetFinalBoneMatrices()
	{
		return m_FinalBoneMatrices;
	}

private:

	void UpdateCurrentAnimation(float dt)
	{
		if (m_Skeleton && m_CurrentAnimation)
		{
			m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
			m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
			CalculateBoneTransform(&m_Skeleton->GetRootNode(), glm::mat4(1.0f));
		}
	}

	void CalculateBoneTransform(const AssimpNodeData* node, const glm::mat4& parentTransform)
	{
		const std::string nodeName = node->name;
		glm::mat4 nodeTransform = node->transformation;

		Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

		if (Bone)
		{
			Bone->Update(m_CurrentTime);
			nodeTransform = Bone->GetLocalTransform();
		}

		glm::mat4 globalTransformation = parentTransform * nodeTransform;

		const auto& boneInfoMap = m_Skeleton->GetBoneIDMap();
		if (boneInfoMap.find(nodeName) != boneInfoMap.end())
		{
			const int index = boneInfoMap.at(nodeName).id;
			const glm::mat4 offset = boneInfoMap.at(nodeName).offset;
			m_FinalBoneMatrices[index] = globalTransformation * offset;
		}

		for (int i = 0; i < node->childrenCount; i++)
			CalculateBoneTransform(&node->children[i], globalTransformation);
	}

	void UpdateBlendedAnimation(float deltaTime)
	{
		if (!m_Skeleton || !m_CurrentAnimation || !m_QueueAnimation) return;

		Animation* pBaseAnimation = m_CurrentAnimation;
		Animation* pLayeredAnimation = m_QueueAnimation;

		// Speed multipliers to correctly transition from one animation to another
		float a = 1.0f;
		float b = pBaseAnimation->GetDuration() / pLayeredAnimation->GetDuration();
		const float animSpeedMultiplierUp = (1.0f - m_BlendFactor) * a + b * m_BlendFactor; // Lerp

		a = pLayeredAnimation->GetDuration() / pBaseAnimation->GetDuration();
		b = 1.0f;
		const float animSpeedMultiplierDown = (1.0f - m_BlendFactor) * a + b * m_BlendFactor; // Lerp

		// Current time of each animation, "scaled" by the above speed multiplier variables
		m_CurrentTime += pBaseAnimation->GetTicksPerSecond() * deltaTime * animSpeedMultiplierUp;
		m_CurrentTime = fmod(m_CurrentTime, pBaseAnimation->GetDuration());

		m_QueueCurrentTime += pLayeredAnimation->GetTicksPerSecond() * deltaTime * animSpeedMultiplierDown;
		m_QueueCurrentTime = fmod(m_QueueCurrentTime, pLayeredAnimation->GetDuration());

		CalculateBlendedBoneTransform(&m_Skeleton->GetRootNode(), glm::mat4(1.0f), m_BlendFactor);
	}

	void CalculateBlendedBoneTransform(const AssimpNodeData* node, const glm::mat4& parentTransform, const float blendFactor)
	{
		const std::string& nodeName = node->name;

		Animation* pAnimationBase = m_CurrentAnimation;
		Animation* pAnimationLayer = m_QueueAnimation;

		//his brain is the one with the thousand yard stare not him that's why he's confused
		glm::mat4 nodeTransform = node->transformation;
		Bone* pBone = pAnimationBase->FindBone(nodeName);
		if (pBone)
		{
			pBone->Update(m_CurrentTime);
			nodeTransform = pBone->GetLocalTransform();
		}

		glm::mat4 layeredNodeTransform = node->transformation;
		pBone = pAnimationLayer->FindBone(nodeName);
		if (pBone)
		{
			pBone->Update(m_QueueCurrentTime);
			layeredNodeTransform = pBone->GetLocalTransform();
		}

		glm::vec3 scale0;
		glm::quat rotation0;
		glm::vec3 translation0;
		glm::vec3 skew0;
		glm::vec4 perspective0;
		glm::decompose(nodeTransform, scale0, rotation0, translation0, skew0, perspective0);
		
		glm::vec3 scale1;
		glm::quat rotation1;
		glm::vec3 translation1;
		glm::vec3 skew1;
		glm::vec4 perspective1;
		glm::decompose(layeredNodeTransform, scale1, rotation1, translation1, skew1, perspective1);

		glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::mix(translation0, translation1, blendFactor));
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::mix(scale0, scale1, blendFactor));
		glm::mat4 rotation = glm::mat4_cast(glm::slerp(rotation0, rotation1, blendFactor));

		glm::mat4 blendedMat = translation * rotation * scale;

		
		//glm::mix() <-- lerp //glm::slerp() <-- slerp
		/* // Incomplete! not grabbing scale!
		const glm::quat rot0 = glm::quat_cast(nodeTransform);
		const glm::quat rot1 = glm::quat_cast(layeredNodeTransform);
		const glm::quat finalRot = glm::slerp(rot0, rot1, blendFactor);
		glm::mat4 blendedMat = glm::mat4_cast(finalRot);
		blendedMat[3] = (1.0f - blendFactor) * nodeTransform[3] + layeredNodeTransform[3] * blendFactor;
		*/

		glm::mat4 globalTransformation = parentTransform * blendedMat;

		const auto& boneInfoMap = m_Skeleton->GetBoneIDMap();
		if (boneInfoMap.find(nodeName) != boneInfoMap.end())
		{
			const int index = boneInfoMap.at(nodeName).id;
			const glm::mat4& offset = boneInfoMap.at(nodeName).offset;

			m_FinalBoneMatrices[index] = globalTransformation * offset;
		}

		for (size_t i = 0; i < node->children.size(); ++i)
			CalculateBlendedBoneTransform(&node->children[i], globalTransformation, blendFactor);
	}

	std::vector<glm::mat4> m_FinalBoneMatrices;
	Skeleton* m_Skeleton;
	Animation* m_CurrentAnimation;
	Animation* m_QueueAnimation;
	int m_AnimationIndex;
	int m_QueueAnimationIndex;
	float m_BlendFactor;
	float m_CurrentTime;
	float m_QueueCurrentTime;

};