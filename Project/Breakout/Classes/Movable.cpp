#include "Movable.h"
#include "Game.h"

Movable::Movable()
	:GameObject()
{
	velocity = glm::vec2( 0.0 );
}

Movable::Movable( glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color )
	: GameObject( pos, size, sprite, color )
{
	velocity = glm::vec2(0.0);
}

void Movable::Update( float dt )
{
	position.x += velocity.x * dt;
	position.y += velocity.y * dt;
}

void Movable::MoveTowards( Direction dir, float dt )
{
	if(dir == Direction::Left)
		position.x -= velocity.x * dt;
	else if( dir == Direction::Right )
		position.x += velocity.x * dt;
	else if( dir == Direction::Up )
		position.y += velocity.y * dt;
	else if( dir == Direction::Down )
		position.y -= velocity.y * dt;

	Game::getInstance()->ClampPosition( *this );
}

void Movable::Reset( glm::vec2 pos, glm::vec2 vel )
{
	position = pos;
	velocity = vel;
}