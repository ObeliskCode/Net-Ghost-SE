#pragma once

#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <Bone.h>
#include <functional>
#include <SkeletalModel.h>

struct AssimpNodeData
{
	glm::mat4 transformation;
	std::string name;
	int childrenCount;
	std::vector<AssimpNodeData> children;
};

class Animation
{
public:
	Animation() = default;

	Animation(const std::string animationPath, SkeletalModel* model)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
		assert(scene && scene->mRootNode);
		auto animation = scene->mAnimations[0];
		m_Duration = animation->mDuration;
		m_TicksPerSecond = animation->mTicksPerSecond;
		aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
		globalTransformation = globalTransformation.Inverse();
		ReadHierarchyData(m_RootNode, scene->mRootNode);
		ReadMissingBones(animation, *model);
	}

	~Animation()
	{
	}

	Bone* FindBone(const std::string& name)
	{
		auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
			[&](const Bone& Bone)
			{
				return Bone.GetBoneName() == name;
			}
		);
		if (iter == m_Bones.end()) return nullptr;
		else return &(*iter);
	}


	inline float GetTicksPerSecond() { return m_TicksPerSecond; }
	inline float GetDuration() { return m_Duration; }
	inline const AssimpNodeData& GetRootNode() { return m_RootNode; }
	inline const std::map<std::string, BoneInfo>& GetBoneIDMap()
	{
		return m_BoneInfoMap;
	}

private:
	void ReadMissingBones(const aiAnimation* animation, SkeletalModel& model)
	{
		int size = animation->mNumChannels;

		auto& boneInfoMap = model.GetBoneInfoMap();//getting m_BoneInfoMap from Model class
		int& boneCount = model.GetBoneCount(); //getting the m_BoneCounter from Model class

		//reading channels(bones engaged in an animation and their keyframes)
		for (int i = 0; i < size; i++)
		{
			auto channel = animation->mChannels[i];
			std::string boneName = channel->mNodeName.data;

			if (boneInfoMap.find(boneName) == boneInfoMap.end())
			{
				boneInfoMap[boneName].id = boneCount;
				boneCount++;
			}
			m_Bones.push_back(Bone(channel->mNodeName.data,
				boneInfoMap[channel->mNodeName.data].id, channel));
		}

		m_BoneInfoMap = boneInfoMap;
	}

	void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src)
	{
		assert(src);

		dest.name = src->mName.data;
		dest.transformation = aiMat4toGLM(src->mTransformation);
		dest.childrenCount = src->mNumChildren;

		for (int i = 0; i < src->mNumChildren; i++)
		{
			AssimpNodeData newData;
			ReadHierarchyData(newData, src->mChildren[i]);
			dest.children.push_back(newData);
		}
	}


	glm::mat4 aiMat4toGLM(aiMatrix4x4t<float> matrix) {

		glm::mat4 m = glm::mat4(0.0f);
		m[0][0] = matrix.a1;
		m[0][1] = matrix.b1;
		m[0][2] = matrix.c1;
		m[0][3] = matrix.d1;

		m[1][0] = matrix.a2;
		m[1][1] = matrix.b2;
		m[1][2] = matrix.c2;
		m[1][3] = matrix.d2;

		m[2][0] = matrix.a3;
		m[2][1] = matrix.b3;
		m[2][2] = matrix.c3;
		m[2][3] = matrix.d3;

		m[3][0] = matrix.a4;
		m[3][1] = matrix.b4;
		m[3][2] = matrix.c4;
		m[3][3] = matrix.d4;

		return m;
	}

	float m_Duration;
	int m_TicksPerSecond;
	std::vector<Bone> m_Bones;
	AssimpNodeData m_RootNode;
	std::map<std::string, BoneInfo> m_BoneInfoMap;
};

