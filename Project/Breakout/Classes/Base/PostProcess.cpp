#include "PostProcess.h"
#include "GL.h"
#include "Debug.h"

PostProcess::PostProcess( Shader shader, int width, int height )
	:_shader(shader)
	,_width(width)
	,_height(height)
	,_texture(Texture2D())
	,_useChaos(false)
	,_useConfuse(false)
	,_useShake(false)
	,_totalElaspeTime(0.0)
	,_shakeRemainTime(0.0)
{
	// init msfbo & rbo
	glGenFramebuffers( 1, &_msfbo );
	glGenRenderbuffers( 1, &_rbo );
	
	glBindFramebuffer( GL_FRAMEBUFFER, _msfbo );
	glBindRenderbuffer( GL_RENDERBUFFER, _rbo );
	glRenderbufferStorageMultisample( GL_RENDERBUFFER, 8, GL_RGB, _width, _height );
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _rbo );
	if( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
		Debug::LogError( "ERROR::POSTPROCESSOR: Failed to initialize MSFBO" );

	// init fbo
	glGenFramebuffers( 1, &_fbo );
	glBindFramebuffer( GL_FRAMEBUFFER, _fbo );
	_texture.Generate( _width, _height, NULL );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture.ID, 0 );
	if( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
		Debug::LogError( "ERROR::POSTPROCESSOR: Failed to initialize FBO" );
	
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	// init vao & vbo
	Init();

	// init shader
	_shader.use();
	_shader.setInt("scene", 0);

	float offset = 1.0 / 300.0;
	float offsets[9][2] = 
	{
		{-offset,  offset},  // top-left
		{0.0f,    offset},  // top-center
		{offset,  offset},  // top-right
		{-offset,  0.0f},  // center-left
		{0.0f,    0.0f},  // center-center
		{offset,  0.0f},  // center - right
		{-offset, -offset},  // bottom-left
		{0.0f,   -offset},  // bottom-center
		{offset, -offset}   // bottom-right
	};
	glUniform2fv( _shader.getUniformLocation( "offset" ), 9, (GLfloat*)offsets );

	int edge_kernel[9] = 
	{
		-1, -1, -1,
		-1,  8, -1,
		-1, -1, -1
	};
	glUniform1iv( _shader.getUniformLocation( "edge_kernel" ), 9, (GLint*)edge_kernel );

	float blur_kernel[9] = 
	{
		1.0 / 16, 2.0 / 16, 1.0 / 16,
		2.0 / 16, 4.0 / 16, 2.0 / 16,
		1.0 / 16, 2.0 / 16, 1.0 / 16
	};
	glUniform1fv( _shader.getUniformLocation( "blur_kernel" ), 9, (GLfloat*)blur_kernel );

}

PostProcess::~PostProcess()
{
	glDeleteVertexArrays( 1, &_vao );
	glDeleteBuffers( 1, &_vbo );

	glDeleteFramebuffers( 1, &_msfbo );
	glDeleteRenderbuffers( 1, &_rbo );

	glDeleteFramebuffers( 1, &_fbo );
}

void PostProcess::Init()
{
	GLfloat vertices[] = {
		// Pos        // Tex
		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f,  1.0f, 1.0f, 1.0f,
		-1.0f,  1.0f, 0.0f, 1.0f,

		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		1.0f,  1.0f, 1.0f, 1.0f
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

void PostProcess::BeginRender()
{
	glBindFramebuffer( GL_FRAMEBUFFER, _msfbo );
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );
}


void PostProcess::Render()
{
	_shader.use();
	_shader.setFloat("time", _totalElaspeTime );
	_shader.setBool("confuse", _useConfuse);
	_shader.setBool( "chaos", _useChaos );
	_shader.setBool( "shake", _useShake );

	glActiveTexture( GL_TEXTURE0 );
	_texture.Bind();

	glBindVertexArray( _vao );
	glDrawArrays( GL_TRIANGLES, 0, 6 );
	glBindVertexArray( 0 );
}

void PostProcess::EndRender()
{
	glBindFramebuffer( GL_READ_FRAMEBUFFER, _msfbo );
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, _fbo );
	glBlitFramebuffer( 0, 0, _width, _height,
						0, 0, _width, _height,
						GL_COLOR_BUFFER_BIT, GL_NEAREST );

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void PostProcess::Reset()
{
	_totalElaspeTime = 0.0;
	_shakeRemainTime = 0.0;
	_useShake = false;
	_useConfuse = false;
	_useChaos = false;
}

void PostProcess::Update( float dt )
{
	_totalElaspeTime += dt;

	if( _useShake )
	{
		_shakeRemainTime -= dt;
		if( _shakeRemainTime <= 0.0 )
			_useShake = false;		
	}
}

void PostProcess::DoShake( float duration )
{
	_shakeRemainTime = duration;
	_useShake = true;
}