#pragma once
#ifndef GameLevel_H
#define GameLevel_H

#include <vector>

#include "GameObject.h"
#include "Base/SpriteRenderer.h"

class GameLevel
{
public:
	GameLevel();
	virtual ~GameLevel();

	bool Load(const char* file, int width, int height, glm::vec2 posOffset);
	virtual void Draw( SpriteRenderer* renderer );

	bool IsCompleted();
	virtual void Reset();

private:
	bool Init(std::vector<std::vector<int>> data, int width, int height, glm::vec2 posOffset );

public:
	std::vector<GameObject> bricks;
};

#endif // !GameLevel_H
