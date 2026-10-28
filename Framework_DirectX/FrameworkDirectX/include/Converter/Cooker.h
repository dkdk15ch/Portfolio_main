#pragma once
#include <ostream>

class Bone;
class ModelNode;

namespace Cooker {
	void Bone(std::ostream& ofs, ::Bone* bone);
	void ModelNode(std::ostream& ofs, ::ModelNode* node);
};// namespace Cooker
