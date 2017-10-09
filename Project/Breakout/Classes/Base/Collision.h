#pragma once
#ifndef Collision_H
#define Collision_H

#include <glm/glm.hpp>
#include "Types.h"

struct CollisionInfo
{
	bool isCollided;
	Direction direction;
	glm::vec2 difference;
};

#endif // !Collision_H
