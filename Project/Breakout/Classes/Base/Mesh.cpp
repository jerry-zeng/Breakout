#include "Mesh.h"

#include "GL.h"
#include "Debug.h"
#include "Utils.h"

using namespace std;


Mesh::Mesh( const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices, const std::vector<Texture> &textures )
{
	this->_vertices = vertices;
	this->_indices = indices;
	this->_textures = textures;

	this->Setup();
}

void Mesh::PrepareShader( Shader shader )
{
	shader.use();

	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;

	shader.setFloat( "material.shininess", 32.0f );

	std::string format;

	for( int i = 0; i < _textures.size(); i++ )
	{
		glActiveTexture( GL_TEXTURE0 + i );

		unsigned int number;
		if( _textures[i].type == "texture_diffuse" )
			number = diffuseNr++;
		else if( _textures[i].type == "texture_specular" )
			number = specularNr++;
		else if( _textures[i].type == "texture_normal" )
			number = normalNr++;
		else if( _textures[i].type == "texture_height" )
			number = heightNr++;

		format = ("material." + _textures[i].type + Utils::ToString( number ));
		shader.setInt( format, i );

		//Debug::Log(format.c_str());

		// no material prefix
		format = (_textures[i].type + Utils::ToString( number ));
		shader.setInt( format, i );

		glBindTexture( GL_TEXTURE_2D, _textures[i].id );
	}
}

void Mesh::Draw( Shader shader )
{
	PrepareShader(shader);

	glBindVertexArray( _vao );
	glDrawElements( GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0 );
	glBindVertexArray( 0 );

	glActiveTexture( GL_TEXTURE0 );
}

void Mesh::DrawInstanced( Shader shader, unsigned int count )
{
	PrepareShader( shader );

	glBindVertexArray( _vao );
	glDrawElementsInstanced( GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0, count );
	glBindVertexArray( 0 );

	glActiveTexture( GL_TEXTURE0 );
}

void Mesh::Setup()
{
	glGenVertexArrays( 1, &_vao );
	glBindVertexArray( _vao );

	glGenBuffers( 1, &_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, _vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof(Vertex)*_vertices.size(), &_vertices[0], GL_STATIC_DRAW );

	glGenBuffers( 1, &_ebo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _ebo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( unsigned int )*_indices.size(), &_indices[0], GL_STATIC_DRAW );

	// location of 'position'
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (GLvoid*)0 );
	glEnableVertexAttribArray( 0 );

	// location of 'normal'
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (GLvoid*)(offsetof(Vertex, normal)) );
	glEnableVertexAttribArray( 1 );

	// location of 'texCood'
	glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (GLvoid*)(offsetof( Vertex, texCood )) );
	glEnableVertexAttribArray( 2 );

	// localtion of 'tangents'
	glVertexAttribPointer( 3, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (GLvoid*)(offsetof( Vertex, tangents )) );
	glEnableVertexAttribArray( 3 );

	// localtion of 'bitangents'
	glVertexAttribPointer( 4, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (GLvoid*)(offsetof( Vertex, bitangents )) );
	glEnableVertexAttribArray( 4 );

	// unbind vbo
	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	// unbind vao
	glBindVertexArray( 0 );
}
