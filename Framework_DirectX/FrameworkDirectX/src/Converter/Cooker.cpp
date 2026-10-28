#include "Converter/Cooker.h"
#include "Animation/Bone.h"

#include <cstring>


void Cooker::Bone(std::ostream& ofs, ::Bone* bone)
{
	BoneBlob blob{};
	const std::string& nameStr = bone->name();
	assert(nameStr.size() < sizeof(blob.name) && "name is too long");
	strcpy_s(blob.name, sizeof(blob.name), nameStr.c_str());
	const std::string& nodeNameStr = bone->nodeName();
	assert(nodeNameStr.size() < sizeof(blob.nodeName) && "nodeName is too long");
	strcpy_s(blob.nodeName, sizeof(blob.nodeName), nodeNameStr.c_str());
	blob.index = bone->index();
	blob.nodeIndex = bone->nodeIndex();
	blob.worldMat = bone->worldMatrix();
	blob.invMat = bone->inverseMatrix();
	ofs.write(reinterpret_cast<const char*>(&blob), sizeof(BoneBlob));
}

void Cooker::ModelNode(std::ostream& ofs, ::ModelNode* node)
{
	//ModelNodeBlob blob{};
	//const std::string& nameStr = node->name();
	//assert(nameStr.size() < sizeof(blob.name) && "name is too long");
	//std::strncpy(blob.name, nameStr.c_str(), sizeof(blob.name));
	//blob.localPosition = node->localPosition();
	//blob.localRotation = node->localRotation();
	//blob.localScale = node->localScale();
	//blob.localMatrix = node->localMatrix();
	//blob.globalInvertTPose = node->globlaInvertTPose();
	//blob.parent = node->parent();
	//uint32_t count = static_cast<uint32_t>(node.size());
	//ofs.write(reinterpret_cast<const char*>(&blob), sizeof(count));
	
}

