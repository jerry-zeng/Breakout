#pragma once
#ifndef GameObject_H
#define GameObject_H

#include <glm/glm.hpp>
#include "Base/Texture2D.h"
#include "Base/SpriteRenderer.h"

enum class Shape
{
	AABB,
	Circle
};

class GameObject
{
public:
	GameObject();
	virtual ~GameObject();
	GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color = glm::vec3( 1.0f ) );

	virtual void Draw(SpriteRenderer* renderer);

	glm::vec2& getCenter();

public:
	glm::vec2 position;
	glm::vec2 size;
	float rotation;
	glm::vec3 color;
	bool isSolid;
	bool isDestroyed;
	Shape shape;

	Texture2D sprite;
};

#endif // !GameObject_H