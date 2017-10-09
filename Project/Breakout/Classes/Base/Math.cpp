#include "Math.h"

// 向量与Direction的顺序对应.
const static glm::vec2 Compass[] = 
{
	glm::vec2( 0.0f, 1.0f ),  // 上	
	glm::vec2( 0.0f, -1.0f ), // 下
	glm::vec2( -1.0f, 0.0f ),  // 左
	glm::vec2( 1.0f, 0.0f ),  // 右
};

Direction Math::CheckDirection( glm::vec2 vector )
{
	float max = 0.0;
	int bestMatch = -1;

	for( int i = 0; i < 4; i++ )
	{
		float dot = glm::dot( glm::normalize( vector ), Compass[i] );
		if( dot > max )
			bestMatch = i;
	}
	return (Direction)bestMatch;
}

/** .----.
 *  |    | h
 *  P----.
 *    w
 *  AABB: p is position, w&h is size
 */

bool Math::AABBIntersectAABB( const glm::vec2 &posA, const glm::vec2 &sizeA, const glm::vec2 &posB, const glm::vec2 &sizeB )
{
	bool collisionX = (posA.x + sizeA.x >= posB.x) && (posA.x <= posB.x + sizeB.x);
	bool collisionY = (posA.y + sizeA.y >= posB.y) && (posA.y <= posB.y + sizeB.y);
	return collisionX && collisionY;
}

bool Math::CircleIntersectAABB( const glm::vec2 &centerA, float radiusA, const glm::vec2 &posB, const glm::vec2 &sizeB, CollisionInfo &info )
{
	glm::vec2 halfSizeB = glm::vec2( sizeB.x*0.5, sizeB.y*0.5 );
	glm::vec2 centerB = posB + halfSizeB;

	glm::vec2 dirBA = centerA - centerB;
	dirBA = glm::clamp( dirBA, -halfSizeB, halfSizeB );

	glm::vec2 p = centerB + dirBA; //the nearest point to circle in AABB
	glm::vec2 line = centerA - p;

	if( glm::dot( line, line ) <= radiusA * radiusA ){
		info.isCollided = true;
		info.difference = line;
		info.direction = CheckDirection(line);
	}
	else{
		info.isCollided = false;
		info.direction = Direction::Up;
		info.difference.x = 0.0;
		info.difference.y = 0.0;
	}

	return info.isCollided;
}

bool Math::CircleIntersectCircle( const glm::vec2 &centerA, float radiusA, const glm::vec2 &centerB, float radiusB )
{
	// the distance between centerA and centerB <= radiusA + radiusB
	glm::vec2 line = centerA - centerB;

	return glm::dot( line, line ) <= (radiusA + radiusB)*(radiusA + radiusB);
}