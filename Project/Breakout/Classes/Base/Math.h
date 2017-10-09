#pragma once
#ifndef Math_H
#define Math_H

#include <glm/glm.hpp>
#include "Collision.h"

class Math
{
public:
	static bool AABBIntersectAABB(const glm::vec2 &posA, const glm::vec2 &sizeA, const glm::vec2 &posB, const glm::vec2 &sizeB);
	static bool CircleIntersectAABB( const glm::vec2 &centerA, float radiusA, const glm::vec2 &posB, const glm::vec2 &sizeB, CollisionInfo &info );
	static bool CircleIntersectCircle( const glm::vec2 &centerA, float radiusA, const glm::vec2 &centerB, float radiusB );

	static Direction CheckDirection(glm::vec2 vector);
};

#endif // !Math_H