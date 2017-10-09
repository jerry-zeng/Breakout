#include "SpriteRenderer.h"
#include "GL.h"
#include "../Game.h"


SpriteRenderer::SpriteRenderer( Shader &shader )
{
	_shader = shader;
	Init();
}

SpriteRenderer::~SpriteRenderer()
{
	glDeleteVertexArrays( 1, &_vao );
	glDeleteBuffers( 1, &_vbo );
}

void SpriteRenderer::Init()
{
	GLfloat vertices[] = {
		// Î»ÖÃ     // ÎÆÀí
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};

	glGenVertexArrays( 1, &_vao );
	glBindVertexArray( _vao );

	glGenBuffers( 1, &_vbo );
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData( GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW );

	glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof( GL_FLOAT ), (const void*)0 );
	glEnableVertexAttribArray( 0 );

	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	glBindVertexArray( 0 );
}

void SpriteRenderer::DrawSprite( Texture2D &texture, glm::vec2 position, glm::vec2 size,
	float rotation, glm::vec3 color, float alpha )
{
	Game* game = Game::getInstance();
	
	_shader.use();
	_shader.setMat4( "projection", game->getProjection() );
	_shader.setMat4( "view", game->getView() );

	glm::mat4 model;
	model = glm::translate( model, glm::vec3(position.x, position.y, 0.0f) );

	model = glm::translate( model, glm::vec3( 0.5f * size.x, 0.5f * size.y, 0.0f ) );
	model = glm::rotate( model, rotation, glm::vec3( 0.0f, 0.0f, 1.0f ) );
	model = glm::translate( model, glm::vec3( -0.5f * size.x, -0.5f * size.y, 0.0f ) );

	model = glm::scale( model, glm::vec3( size, 1.0f ) );
	_shader.setMat4( "model", model );

	_shader.setInt("Texture0", 0);
	_shader.setVec4("color", glm::vec4(color, alpha));

	glActiveTexture( GL_TEXTURE0 );
	texture.Bind();

	glBindVertexArray( _vao );
	glDrawArrays( GL_TRIANGLES, 0, 6 );
	glBindVertexArray( 0 );
}
