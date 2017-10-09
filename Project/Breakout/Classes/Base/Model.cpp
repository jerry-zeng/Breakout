#include "Model.h"

#include "GL.h"
#include "stb_image.h"
#include "Debug.h"
#include "Utils.h"

using namespace Assimp;
using namespace std;


Model::Model( const std::string &path, Shader shader, bool debug )
	:EnableDebug(debug)
	,_shader(shader)
{
	loadModel(path);

	if(EnableDebug)
		Print();
}


Model::~Model()
{
	_meshList.clear();
	_textures_loaded.clear();
}

void Model::Draw()
{
	for( int i = 0; i < _meshList.size(); i++ )
		_meshList[i].Draw(_shader);
}

void Model::Draw( Shader overrideShader )
{
	for( int i = 0; i < _meshList.size(); i++ )
		_meshList[i].Draw( overrideShader );
}

void Model::DrawInstanced( unsigned int count )
{
	for( int i = 0; i < _meshList.size(); i++ )
		_meshList[i].DrawInstanced( _shader, count );
}

void Model::Print()
{
	Debug::Log(("in directory: " + directory).c_str());
	for( int i = 0; i < _meshList.size(); i++ )
	{
		Mesh mesh = _meshList[i];

		for( int j = 0; j < mesh._textures.size(); j++ )
			Debug::Log(mesh._textures[j].path.C_Str());
	}
}

void Model::loadModel( const std::string &path )
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace );

	if( scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE == 1 || !scene->mRootNode ){
		Debug::Log("Load model failed.");
		return;
	}

	directory = path.substr( 0, path.find_last_of( '/' ) );

	this->processNode( scene->mRootNode, scene );
}

void Model::processNode( aiNode *node, const aiScene *scene )
{
	for(unsigned int i = 0; i < node->mNumMeshes; i++ )
	{
		auto mesh = scene->mMeshes[node->mMeshes[i]];
		_meshList.push_back( processMesh(mesh, scene) );
	}

	for(unsigned int i = 0; i < node->mNumChildren; i++ ){
		processNode( node->mChildren[i], scene);
	}
}


Mesh Model::processMesh( aiMesh *mesh, const aiScene *scene )
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	// vertex
	for( int i = 0; i < mesh->mNumVertices; i++ )
	{
		Vertex vertex;
		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;

		vertex.normal.x = mesh->mNormals[i].x;
		vertex.normal.y = mesh->mNormals[i].y;
		vertex.normal.z = mesh->mNormals[i].z;

		if( mesh->mTextureCoords[0] ){
			vertex.texCood.x = mesh->mTextureCoords[0][i].x;
			vertex.texCood.y = mesh->mTextureCoords[0][i].y;
		}
		else{
			vertex.texCood = glm::vec2(0.0f);
		}

		vertex.tangents.x = mesh->mTangents[i].x;
		vertex.tangents.y = mesh->mTangents[i].y;
		vertex.tangents.z = mesh->mTangents[i].z;

		vertex.bitangents.x = mesh->mBitangents[i].x;
		vertex.bitangents.y = mesh->mBitangents[i].y;
		vertex.bitangents.z = mesh->mBitangents[i].z;

		vertices.push_back(vertex);
	}
	
	// index
	for( int i = 0; i < mesh->mNumFaces; i++ )
	{
		auto face = mesh->mFaces[i];
		for( int j = 0; j < face.mNumIndices; j++ )
			indices.push_back( face.mIndices[j] );
	}

	// textures
	if( mesh->mMaterialIndex >= 0 )
	{
		auto mat = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture> diffuseMap = loadMaterialTextures( mat, aiTextureType::aiTextureType_DIFFUSE, "texture_diffuse" );
		if( diffuseMap.size() > 0 )
			textures.insert( textures.end(), diffuseMap.begin(), diffuseMap.end() );

		std::vector<Texture> specularMap = loadMaterialTextures( mat, aiTextureType::aiTextureType_SPECULAR, "texture_specular" );
		if( specularMap.size() > 0 )
			textures.insert( textures.end(), specularMap.begin(), specularMap.end() );

		std::vector<Texture> normalMap = loadMaterialTextures( mat, aiTextureType::aiTextureType_HEIGHT, "texture_normal" );
		if( normalMap.size() > 0 )
			textures.insert( textures.end(), normalMap.begin(), normalMap.end() );

		std::vector<Texture> heightMap = loadMaterialTextures( mat, aiTextureType::aiTextureType_AMBIENT, "texture_height" );
		if( heightMap.size() > 0 )
			textures.insert( textures.end(), heightMap.begin(), heightMap.end() );

		if( EnableDebug ){
			std::string msg( "" );
			msg.append( "material " );
			msg.append( Utils::ToString( mesh->mMaterialIndex ) );
			msg.append( " has diffuse map " );
			msg.append( Utils::ToString( mat->GetTextureCount( aiTextureType_DIFFUSE ) ) );
			msg.append( ", specular map " );
			msg.append( Utils::ToString( mat->GetTextureCount( aiTextureType_SPECULAR ) ) );
			msg.append( ", normal map " );
			msg.append( Utils::ToString( mat->GetTextureCount( aiTextureType_HEIGHT ) ) );
			msg.append( ", height map " );
			msg.append( Utils::ToString( mat->GetTextureCount( aiTextureType_AMBIENT ) ) );
			Debug::Log( msg.c_str() );
		}
	}

	return Mesh( vertices, indices, textures);
}


std::vector<Texture> Model::loadMaterialTextures( aiMaterial *mat, aiTextureType type, std::string typeName )
{
	std::vector<Texture> textures;
	for( unsigned int i = 0; i < mat->GetTextureCount( type ); i++ )
	{
		aiString str;
		mat->GetTexture( type, i, &str );

		bool hasFound = false;
		for( int j = 0; j < _textures_loaded.size(); j++ )
		{
			if( std::strcmp(_textures_loaded[i].path.C_Str(), str.C_Str()) == 0 )
			{
				textures.push_back( _textures_loaded[i] );
				hasFound = true;
				break;
			}
		}

		if( !hasFound )
		{
			Texture texture;
			texture.id = createTextureFromFile( str.C_Str(), this->directory );
			texture.type = typeName;
			texture.path = str;

			textures.push_back( texture );

			_textures_loaded.push_back(texture);
		}
	}
	return textures;
}

unsigned int Model::createTextureFromFile( const char* name, const std::string &dir )
{
	std::string path( dir + "/" + name );

	GLuint textureID;
	glGenTextures( 1, &textureID );

	int t_width, t_heigth, nrChannels;
	unsigned char* imageData = stbi_load( path.c_str(), &t_width, &t_heigth, &nrChannels, 0 );
	if( imageData )
	{
		GLenum format;
		if( nrChannels == 1 )
			format = GL_RED;
		else if( nrChannels == 3 )
			format = GL_RGB;
		else if( nrChannels == 4 )
			format = GL_RGBA;

		glBindTexture( GL_TEXTURE_2D, textureID );
		glTexImage2D( GL_TEXTURE_2D, 0, format, t_width, t_heigth, 0, format, GL_UNSIGNED_BYTE, imageData );
		glGenerateMipmap( GL_TEXTURE_2D );

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	}
	else{
		Debug::Log( ("failed to load the texture: " + path).c_str() );
	}

	stbi_image_free( imageData );

	return textureID;
}
