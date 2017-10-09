#pragma once
#ifndef Ball_H
#define Ball_H

#include "Movable.h"

class Ball : public Movable
{
public:
	Ball();
	Ball( glm::vec2 pos, float radius, Texture2D sprite, glm::vec3 color = glm::vec3( 1.0f ) );

	virtual void Update( float dt );
	virtual void Reset( glm::vec2 pos, glm::vec2 vel );

public:
	bool isStuck;
	float radius;

	bool sticky;
	bool passThrough;
};

#endif // !Ball_H