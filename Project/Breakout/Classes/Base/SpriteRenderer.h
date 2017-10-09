#pragma once
#ifndef SpriteRenderer_H
#define SpriteRenderer_H

#include "Shader.h"
#include "Texture2D.h"

class SpriteRenderer
{
public:
	SpriteRenderer(Shader &shader);
	~SpriteRenderer();

	void DrawSprite( Texture2D &texture, 
		glm::vec2 position,
		glm::vec2 size, 
		float rotate = 0.0f,
		glm::vec3 color = glm::vec3( 1.0f ),
		float alpha = 1.0);

protected:
	Shader _shader;
	unsigned int _vao, _vbo;

	void Init();
};

#endif // SpriteRenderer_H
