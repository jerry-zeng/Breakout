#include "GameObject.h"



GameObject::GameObject()
	:position( glm::vec2( 0.0 ) )
	, size( glm::vec2( 2, 2 ) )
	, sprite( Texture2D() )
	, color( glm::vec3( 1.0 ) )
	, rotation( 0.0 )
	, isSolid( false )
	, isDestroyed( false )
	, shape( Shape::AABB )
{

}

GameObject::GameObject( glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color )
	:position(pos)
	,size(size)
	,sprite(sprite)
	,color(color)
	,rotation(0.0)
	,isSolid(false)
	,isDestroyed(false)
	,shape(Shape::AABB)
{
	
}

GameObject::~GameObject()
{
	
}

glm::vec2& GameObject::getCenter()
{
	return glm::vec2(position.x + size.x/2, position.y + size.y/2);
}

void GameObject::Draw( SpriteRenderer* renderer )
{
	renderer->DrawSprite( sprite, position, size, rotation, color );
}