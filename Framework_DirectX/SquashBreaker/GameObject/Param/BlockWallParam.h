#pragma once
#include "GameObject/ObjectParam.h"

class BlockWall;

class BlockWallParam : public ObjectParam
{
	friend BlockWall;
private:
	void moveFromJson(const std::string& actorKey, const std::string& sceneKey) override final;

	int row;
	int column;
	int depth;

	int totalBlock()const {
		return row * column * depth;
	}
};
