#include "PowerUp.h"



PowerUp::PowerUp()
	:Movable()
	,_type(PowerUpType::Sticky)
	,_isActivated(false)
	,_duration(0.0)
{
}

PowerUp::PowerUp( PowerUpType type, float duration, glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color )
	: Movable(pos, size, sprite, color)
	, _type( type )	
	, _duration( duration )
	, _isActivated( false )
{

}

void PowerUp::Update( float dt )
{
	Movable::Update(dt);

	if( position.y <= 0 - size.y )
		isDestroyed = true;
}

void PowerUp::Reset( glm::vec2 &pos, glm::vec2 &vel )
{
	Movable::Reset(pos, vel);

	_isActivated = false;
}