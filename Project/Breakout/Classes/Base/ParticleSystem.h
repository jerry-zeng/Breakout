#pragma once
#ifndef ParticleSystem_H
#define ParticleSystem_H

#include <vector>
#include <glm/glm.hpp>
#include "Texture2D.h"
#include "Shader.h"
#include "../Movable.h"

struct Particle
{
	glm::vec2 position;
	glm::vec2 velocity;
	glm::vec4 color;
	float lifeTime;
	float scale;

	Particle()
		:position( glm::vec2() )
		, velocity( glm::vec2() )
		, color( glm::vec4( 1.0 ) )
		, lifeTime( 0.0 )
		, scale( 1.0 )
	{
	}
};

class ParticleSystem
{
public:
	ParticleSystem(Shader &shader, Texture2D &tex, float emitPerSecond);
	~ParticleSystem();

	virtual void Update(float dt, Movable& go);
	virtual void Draw();

	static void RespawnParticle( Particle &particle, Movable& go );

protected:
	void Init();

protected:
	float emitInterval;
	float _elaspedTime;

	std::vector<Particle>::iterator iter;
	std::vector<Particle> unusedParticles;

	std::vector<Particle> _usedParticles;
	Texture2D _texture; //不能用引用&
	Shader _shader;     //不能用引用&
	unsigned int _vao;
	unsigned int _vbo;
};

#endif // !ParticleSystem_H