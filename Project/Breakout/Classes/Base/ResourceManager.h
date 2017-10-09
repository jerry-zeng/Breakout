#pragma once
#ifndef ResourceManager_H
#define ResourceManager_H

#include "Texture2D.h"
#include "Shader.h"
#include <string>
#include <map>

class ResourceManager
{
public:
	static ResourceManager* getInstance();
	static void destroyInstance();

	Texture2D loadTexture( const char* path, const std::string &key );
	Texture2D getTexture( const std::string &key );

	Shader loadShader( const char *vShaderFile, const char *fShaderFile, const char *gShaderFile, const std::string &key );
	Shader getShader( const std::string &key );

	void clean();

private:
	ResourceManager();
	~ResourceManager();

	static ResourceManager* _instance;

	std::map<std::string, Texture2D> _texMap;
	std::map<std::string, Shader> _shaderMap;
};

#endif // ResourceManager_H