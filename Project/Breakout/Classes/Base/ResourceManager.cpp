#include "ResourceManager.h"
#include "GL.h"
#include "stb_image.h"
#include "Debug.h"

#include <iostream>
#include <sstream>
#include <fstream>


ResourceManager* ResourceManager::_instance = NULL;

ResourceManager* ResourceManager::getInstance()
{
	if( _instance == nullptr )
		_instance = new ResourceManager();
	return _instance;
}

void ResourceManager::destroyInstance()
{
	if( _instance ){
		_instance->clean();
		delete _instance;
	}
}

ResourceManager::ResourceManager()
{
	_texMap = std::map<std::string, Texture2D>();
	_shaderMap = std::map<std::string, Shader>();
}

ResourceManager::~ResourceManager()
{
	clean();
}

void ResourceManager::clean()
{
	// delete all shaders
	for( auto iter : _shaderMap )
	{
		glDeleteProgram( iter.second.program );
	}
	_shaderMap.clear();

	// delete all textures
	for( auto iter : _texMap )
	{
		glDeleteTextures( 1, &iter.second.ID );
	}
	_texMap.clear();
}

Texture2D ResourceManager::loadTexture( const char* path, const std::string &key )
{
	Texture2D tex;

	stbi_set_flip_vertically_on_load( true );

	int t_width, t_heigth, nrChannels;
	unsigned char* imageData = stbi_load( path, &t_width, &t_heigth, &nrChannels, 0 );
	if( imageData )
	{
		GLenum texFormat;
		if( nrChannels == 1 )
			texFormat = GL_RED;
		else if( nrChannels == 3 )
			texFormat = GL_RGB;
		else if( nrChannels == 4 )
			texFormat = GL_RGBA;

		tex.internal_format = texFormat;
		tex.image_format = texFormat;
		tex.Generate( t_width, t_heigth, imageData );
	}
	else
	{
		Debug::Log( "can't load the texture" );
	}
	stbi_image_free( imageData );

	if( !key.empty() )
		_texMap[key] = tex;

	return tex;
}

Texture2D ResourceManager::getTexture( const std::string &key )
{
	return _texMap[key];
}

Shader ResourceManager::loadShader( const char *vShaderFile, const char *fShaderFile, const char *gShaderFile, const std::string &key )
{
	// 1. Retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	try
	{
		// Open files
		std::ifstream vertexShaderFile( vShaderFile );
		std::ifstream fragmentShaderFile( fShaderFile );
		std::stringstream vShaderStream, fShaderStream;
		// Read file's buffer contents into streams
		vShaderStream << vertexShaderFile.rdbuf();
		fShaderStream << fragmentShaderFile.rdbuf();
		// close file handlers
		vertexShaderFile.close();
		fragmentShaderFile.close();
		// Convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		// If geometry shader path is present, also load a geometry shader
		if( gShaderFile != nullptr )
		{
			std::ifstream geometryShaderFile( gShaderFile );
			std::stringstream gShaderStream;
			gShaderStream << geometryShaderFile.rdbuf();
			geometryShaderFile.close();
			geometryCode = gShaderStream.str();
		}
	}
	catch( std::exception e )
	{
		Debug::LogError( "ERROR::SHADER: Failed to read shader files" );
	}
	const GLchar *vShaderCode = vertexCode.c_str();
	const GLchar *fShaderCode = fragmentCode.c_str();
	const GLchar *gShaderCode = geometryCode.c_str();

	// 2. Now create shader object from source code
	Shader shader;
	shader.initWithString( vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr );
	
	if( !key.empty() )
		_shaderMap[key] = shader;

	return shader;
}

Shader ResourceManager::getShader( const std::string &key )
{
	return _shaderMap[key];
}
