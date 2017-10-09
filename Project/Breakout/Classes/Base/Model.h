#pragma once
#ifndef __MODEL_H__
#define __MODEL_H__

#include "Mesh.h"
#include "Shader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


class Model
{
public:
	bool EnableDebug;

	Model( const std::string &path, Shader shader, bool debug = false );
	~Model();

	void Draw();
	void Draw(Shader overrideShader);
	virtual void DrawInstanced( unsigned int count );

	void Print();

	inline Shader getShader() { return _shader; }
	inline void setShader( Shader value ) { _shader = value; }

	inline const std::vector<Mesh> getMeshList() { return _meshList; }

private:
	Shader _shader;

	std::string directory;
	std::vector<Mesh> _meshList;
	std::vector<Texture> _textures_loaded;

	void loadModel( const std::string &path );
	void processNode( aiNode *node, const aiScene *scene );
	Mesh processMesh( aiMesh *mesh, const aiScene *scene );
	std::vector<Texture> loadMaterialTextures( aiMaterial *mat, aiTextureType type, std::string typeName );

	unsigned int createTextureFromFile(const char* name, const std::string &dir);
};

#endif // !__MODEL_H__
