#pragma once

#include <vector>
#include <unordered_map> 
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

struct Animation {
	float m_Duration;
	int m_TicksPerSecond;
	std::string m_Name;
	std::vector<Bone> m_Bones;

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
};

class Skeleton
{
public:
	Skeleton(const std::string animationPath, SkeletalModel* model)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate | aiProcess_LimitBoneWeights);
		assert(scene && scene->mRootNode);

		aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
		globalTransformation = globalTransformation.Inverse();
		ReadHierarchyData(m_RootNode, scene->mRootNode);
		for (int i = 0; i < scene->mNumAnimations; i++) {
			Animation newAnim;
			auto animation = scene->mAnimations[i];
			newAnim.m_Duration = animation->mDuration;
			newAnim.m_TicksPerSecond = animation->mTicksPerSecond;
			ReadBones(newAnim, animation, *model);
			if (animation->mNumMeshChannels != 0) std::cerr << "UNSUPPORTED ANIMATION KEYS! mNumMeshChannels: " << animation->mNumMeshChannels << std::endl;
			if (animation->mNumMorphMeshChannels != 0) std::cerr << "UNSUPPORTED ANIMATION KEYS! mNumMorphMeshChannels: " << animation->mNumMorphMeshChannels << std::endl;
		}
	}

	~Skeleton()
	{

	}

	void addAnimation(const std::string animationPath, SkeletalModel* model) 
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate | aiProcess_LimitBoneWeights);
		assert(scene && scene->mRootNode);

		// ON faith we do not import skeleton!
		for (int i = 0; i < scene->mNumAnimations; i++) {
			Animation newAnim;
			auto animation = scene->mAnimations[i];
			newAnim.m_Duration = animation->mDuration;
			newAnim.m_TicksPerSecond = animation->mTicksPerSecond;
			ReadBones(newAnim, animation, *model);
			if (animation->mNumMeshChannels != 0) std::cerr << "UNSUPPORTED ANIMATION KEYS! mNumMeshChannels: " << animation->mNumMeshChannels << std::endl;
			if (animation->mNumMorphMeshChannels != 0) std::cerr << "UNSUPPORTED ANIMATION KEYS! mNumMorphMeshChannels: " << animation->mNumMorphMeshChannels << std::endl;
		}
	}

	Animation* GetAnimation(unsigned int index)
	{
		if (index >= m_Animations.size()) return nullptr;
		return &m_Animations[index];
	}

	inline const AssimpNodeData& GetRootNode() { return m_RootNode; }
	inline const std::unordered_map<std::string, BoneInfo>& GetBoneIDMap()
	{
		return m_BoneInfoMap;
	}

private:
	void ReadBones(Animation newAnim, const aiAnimation* animation, SkeletalModel& model)
	{
		int size = animation->mNumChannels;

		// we should do this only once otherwise get up-to-date private member infoMap instead
		auto& boneInfoMap = model.GetBoneInfoMap();//getting m_BoneInfoMap from Model class 
		int& boneCount = model.GetBoneCount(); //getting the m_BoneCounter from Model class

		//reading channels(bones engaged in an animation and their keyframes)
		for (int i = 0; i < size; i++)
		{
			auto channel = animation->mChannels[i];
			std::string boneName = channel->mNodeName.data;

			// literally useless bone we make here
			if (boneInfoMap.find(boneName) == boneInfoMap.end())
			{
				// impossible to index m_FMB[id] since no vertex maps to this id.
				boneInfoMap[boneName].id = boneCount;
				// if not derived from m_bone then will not affect any vertices? is m_FBM[id] ever used?
				boneInfoMap[boneName].offset = glm::mat4(1.0f);
				std::cerr <<  "useless bone, Name: " << boneName << std::endl;
				boneCount++;
			}
			newAnim.m_Bones.push_back(Bone(channel->mNodeName.data,
				boneInfoMap[channel->mNodeName.data].id, channel));
		}

		m_Animations.push_back(newAnim);
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


	glm::mat4 aiMat4toGLM(const aiMatrix4x4t<float>& matrix) {

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

	std::vector<Animation> m_Animations;
	AssimpNodeData m_RootNode;
	std::unordered_map<std::string, BoneInfo> m_BoneInfoMap;
};