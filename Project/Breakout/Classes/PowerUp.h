#pragma once
#ifndef PowerUp_H
#define PowerUp_H

#include "Movable.h"

enum class PowerUpType
{
	Sticky = 1,
	PassThrough,
	SpeedUp,
	PadSizeUp,
	//Bullet,
	//Separate,
	//LifeUp
};

class PowerUp : public Movable
{
public:
	PowerUp();
	PowerUp( PowerUpType type, float duration, glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color = glm::vec3( 1.0f ) );

	virtual void Update( float dt );
	virtual void Reset( glm::vec2 &pos, glm::vec2 &vel );

	inline PowerUpType getType() { return _type; }

public:
	bool _isActivated;
	float _duration;

protected:
	PowerUpType _type;
};

#endif // !PowerUp_H