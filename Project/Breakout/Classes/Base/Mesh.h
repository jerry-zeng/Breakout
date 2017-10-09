#pragma once
#ifndef __Mesh_H__
#define __Mesh_H__

#include <glm/glm.hpp>
#include "Shader.h"
#include <vector>
#include <assimp/types.h>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCood;
	glm::vec3 tangents;
	glm::vec3 bitangents;
};
struct Texture
{
	unsigned int id;
	std::string type;
	aiString path;
};

class Mesh
{
public:
	std::vector<Vertex> _vertices;
	std::vector<unsigned int> _indices;
	std::vector<Texture> _textures;

	Mesh( const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices, const std::vector<Texture> &textures );
	void Draw(Shader shader);

	virtual void DrawInstanced(Shader shader, unsigned int count);

	inline unsigned int getVao() { return _vao; }
	inline unsigned int getVbo() { return _vbo; }
	inline unsigned int getEbo() { return _ebo; }

protected:
	virtual void PrepareShader( Shader shader );

	unsigned int _vao, _vbo, _ebo;

	// initialize vao,vbo,ebo
	void Setup();
};

#endif