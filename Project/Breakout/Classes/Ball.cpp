#include "Ball.h"
#include "Game.h"

Ball::Ball()
	:Movable()
	,isStuck(true)
	,radius(0.0)
	,sticky(false)
	,passThrough(false)
{
}

Ball::Ball( glm::vec2 pos, float radius, Texture2D sprite, glm::vec3 color )
	:Movable(pos, glm::vec2( radius*2, radius*2), sprite, color)
	,isStuck( true )
	,radius( radius )
	,sticky( false )
	,passThrough( false )
{

}

void Ball::Update( float dt )
{
	if(isStuck == false)
	{
		position.x += velocity.x * dt;
		position.y += velocity.y * dt;

		glm::vec2 stageSize = Game::getInstance()->getStageSize();

		if( position.x <= 0 ){
			position.x = 0;
			velocity.x = -velocity.x;
		}
		else if( position.x >= stageSize.x - size.x ){
			position.x = stageSize.x - size.x;
			velocity.x = -velocity.x;
		}

		if( position.y > stageSize.y - size.y ){
			position.y = stageSize.y - size.y;
			velocity.y = -velocity.y;
		}
		else if( position.y <= -size.y ){ // die
			velocity.x = 0;
			velocity.y = 0;

			isDestroyed = true;
		}
	}
}

void Ball::Reset( glm::vec2 pos, glm::vec2 vel )
{
	Movable::Reset(pos, vel);

	isStuck = true;
	sticky = false;
	passThrough = false;
}