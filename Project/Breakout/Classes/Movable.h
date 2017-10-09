#pragma once
#ifndef Player_H
#define Player_H

#include "Base/Types.h"
#include "GameObject.h"

class Movable : public GameObject
{
public:
	Movable();
	Movable( glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color = glm::vec3( 1.0f ) );

	virtual void Update(float dt);
	virtual void MoveTowards(Direction dir, float dt );

	virtual void Reset( glm::vec2 pos, glm::vec2 vel );

public:
	glm::vec2 velocity;
};

#endif // !Player_H