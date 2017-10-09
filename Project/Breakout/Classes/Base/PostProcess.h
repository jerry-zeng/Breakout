#pragma once
#ifndef PostProcess_H
#define PostProcess_H

#include "Shader.h"
#include "Texture2D.h"

class PostProcess
{
public:
	PostProcess(Shader shader, int width, int height);
	virtual ~PostProcess();

	virtual void Update(float dt);

	virtual void BeginRender();
	virtual void Render();
	virtual void EndRender();
	virtual void Reset();

	void DoShake(float duration);

protected:
	virtual void Init();

	// incomplete
	inline void SetChaos( bool value ) { _useChaos = value; }
	inline void SetConfuse( bool value ) { _useConfuse = value; }

protected:
	Shader _shader;
	Texture2D _texture;
	unsigned int _vao;
	unsigned int _vbo;
	unsigned int _msfbo, _fbo;
	unsigned int _rbo;

	int _width, _height;
	bool _useShake;
	float _shakeRemainTime;
	bool _useChaos;
	bool _useConfuse;

	float _totalElaspeTime;
};

#endif // !PostProcess_H
