#include "ParticleSystem.h"
#include "GL.h"
#include "../Game.h"

const int DefaultCount = 300;

ParticleSystem::ParticleSystem( Shader &shader, Texture2D &tex, float emitPerSecond )
	:_texture(tex)
	,_shader(shader)
	,_elaspedTime(0.0)
{
	if( emitPerSecond <= 0.0 )
		emitPerSecond = 10.0;
	emitInterval = 1.0 / emitPerSecond;

	unusedParticles = std::vector<Particle>( DefaultCount );
	for( int i = 0; i < DefaultCount; i++ )
		unusedParticles.push_back( Particle() );
	
	_usedParticles = std::vector<Particle>();

	Init();
}

ParticleSystem::~ParticleSystem()
{
	_usedParticles.clear();
	unusedParticles.clear();

	glDeleteVertexArrays( 1, &_vao );
	glDeleteBuffers( 1, &_vbo );
}

void ParticleSystem::Init()
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
	glBindBuffer( GL_ARRAY_BUFFER, _vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );

	glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof( GL_FLOAT ), (const void*)0 );
	glEnableVertexAttribArray( 0 );

	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	glBindVertexArray( 0 );
}

void ParticleSystem::RespawnParticle( Particle &particle, Movable& go  )
{
	float random = ((rand() % 100) - 50) / 10.0f; //-5~5
	glm::vec2 offset = glm::vec2( go.size.x/4, go.size.y/4 );
	particle.position = glm::vec2( go.position.x + random + offset.x, go.position.y + random + offset.y);
	particle.velocity = go.velocity * 0.1f;

	float rColor = 0.5 + ((rand() % 100) / 200.0f); //0.5~1.5
	particle.color = glm::vec4( rColor, rColor, rColor, 1.0f );

	particle.lifeTime = 1.0f;
	particle.scale = 10.0f;
}

void ParticleSystem::Update( float dt, Movable& go )
{
	_elaspedTime += dt;

	if( _elaspedTime >= emitInterval )
	{
		int count = (int)floorf( _elaspedTime/emitInterval );
		if( count > 0 && unusedParticles.size() > 0 )
		{
			for( int i = 0; i < count; i++ )
			{
				if( unusedParticles.size() <= 0 )
					continue;

				Particle &p = unusedParticles.at( unusedParticles.size() - 1 );
				unusedParticles.pop_back();
				RespawnParticle( p, go );

				_usedParticles.push_back( p );
			}			
		}

		_elaspedTime -= emitInterval * count;
	}

	// update
	for( iter = _usedParticles.begin(); iter != _usedParticles.end(); )
	{
		Particle &p = *iter;
		if( p.lifeTime > 0.0 )
		{
			p.lifeTime -= dt;

			if( p.lifeTime > 0.0 ){
				p.position.x -= p.velocity.x * dt;
				p.position.y -= p.velocity.y * dt;
				p.color.a -= dt * 2.5;

				++iter;
			}
			else
			{
				unusedParticles.push_back(p);
				iter = _usedParticles.erase( iter );
			}
		}
		else
		{
			unusedParticles.push_back( p );
			iter = _usedParticles.erase(iter);
		}
	}
}

void ParticleSystem::Draw()
{
	glBlendFunc( GL_SRC_ALPHA, GL_ONE );

	_shader.use();
	_shader.setInt("Texture0", 0);
	_shader.setMat4("projection", Game::getInstance()->getProjection());

	for( Particle &particle : _usedParticles )
	{
		if( particle.lifeTime > 0.0 ){
			_shader.setVec4( "color", particle.color );
			_shader.setVec2( "offset", particle.position );
			_shader.setFloat( "scale", particle.scale);

			_texture.Bind();

			glBindVertexArray( _vao );
			glDrawArrays( GL_TRIANGLES, 0, 6 );
			glBindVertexArray( 0 );
		}
	}

	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}