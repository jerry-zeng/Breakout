#include "Shader.h"
#include "GL.h"
#include "Debug.h"

#include <string>
#include <iostream>

Shader::Shader()
{
	program = glCreateProgram();
}

void Shader::initWithString( const char* vertexCode, const char* fragmentCode, const char* geometryCode )
{
	// 2. compile shaders
	int success;
	char infoLog[512];

	// vertex shader
	GLuint vertex;
	vertex = glCreateShader( GL_VERTEX_SHADER );
	glShaderSource( vertex, 1, &vertexCode, NULL );
	glCompileShader( vertex );
	checkCompileErrors( vertex, "VERTEX" );

	// fragment Shader
	GLuint fragment;
	fragment = glCreateShader( GL_FRAGMENT_SHADER );
	glShaderSource( fragment, 1, &fragmentCode, NULL );
	glCompileShader( fragment );
	checkCompileErrors( fragment, "FRAGMENT" );

	// if geometry shader is given, compile geometry shader
	GLuint geometry;
	if( geometryCode != nullptr )
	{
		geometry = glCreateShader( GL_GEOMETRY_SHADER );
		glShaderSource( geometry, 1, &geometryCode, NULL );
		glCompileShader( geometry );
		checkCompileErrors( geometry, "GEOMETRY" );
	}

	// shader Program
	glAttachShader( program, vertex );
	glAttachShader( program, fragment );
	if( geometryCode != nullptr )
		glAttachShader( program, geometry );

	glLinkProgram( program );
	checkCompileErrors( program, "PROGRAM" );

	// delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader( vertex );
	glDeleteShader( fragment );
	if( geometryCode != nullptr )
		glDeleteShader( geometry );
}

void Shader::use()
{
	glUseProgram( program );
}

int Shader::getUniformLocation( const std::string &name ) const
{
	return (int)glGetUniformLocation(program, name.c_str());
}

void Shader::setBool( const std::string &name, bool value ) const
{
	GLint location = getUniformLocation( name );
	if(location >= 0)
	glUniform1i( location, (int)value );
}
// ------------------------------------------------------------------------
void Shader::setInt( const std::string &name, int value ) const
{
	GLint location = getUniformLocation( name );
	if( location >= 0 )
	glUniform1i( location, value );
}
// ------------------------------------------------------------------------
void Shader::setFloat( const std::string &name, float value ) const
{
	GLint location = getUniformLocation( name );
	if( location >= 0 )
	glUniform1f( location, value );
}
// ------------------------------------------------------------------------
void Shader::setVec2( const std::string &name, const glm::vec2 &value ) const
{
	GLint location = getUniformLocation( name );
	if( location >= 0 )
	glUniform2fv( location, 1, &value[0] );
}
void Shader::setVec2( const std::string &name, float x, float y ) const
{
	GLint location = getUniformLocation( name );
	if( location >= 0 )
	glUniform2f( location, x, y );
}
// ------------------------------------------------------------------------
void Shader::setVec3( const std::string &name, const glm::vec3 &value ) const
{
	GLint location = getUniformLocation( name );
	if( location >= 0 )
	glUniform3fv( location, 1, &value[0] );
}
void Shader::setVec3( const std::string &name, float x, float y, float z ) const
{
	GLint location = getUniformLocation( name );
	if( location >= 0 )
	glUniform3f( location, x, y, z );
}
// ------------------------------------------------------------------------
void Shader::setVec4( const std::string &name, const glm::vec4 &value ) const
{
	GLint location = getUniformLocation( name );
	if( location >= 0 )
	glUniform4fv( location, 1, &value[0] );
}
void Shader::setVec4( const std::string &name, float x, float y, float z, float w ) const
{
	GLint location = getUniformLocation( name );
	if( location >= 0 )
	glUniform4f( location, x, y, z, w );
}
// ------------------------------------------------------------------------
void Shader::setMat2( const std::string &name, const glm::mat2 &mat ) const
{
	GLint location = getUniformLocation( name );
	if( location >= 0 )
	glUniformMatrix2fv( location, 1, GL_FALSE, &mat[0][0] );
}
// ------------------------------------------------------------------------
void Shader::setMat3( const std::string &name, const glm::mat3 &mat ) const
{
	GLint location = getUniformLocation( name );
	if( location >= 0 )
	glUniformMatrix3fv( location, 1, GL_FALSE, &mat[0][0] );
}
// ------------------------------------------------------------------------
void Shader::setMat4( const std::string &name, const glm::mat4 &mat ) const
{
	GLint location = getUniformLocation( name );
	if( location >= 0 )
	glUniformMatrix4fv( location, 1, GL_FALSE, &mat[0][0] );
}

void Shader::checkCompileErrors( unsigned int shader, std::string type )
{
	GLint success;
	GLchar infoLog[1024];
	if( type != "PROGRAM" )
	{
		glGetShaderiv( shader, GL_COMPILE_STATUS, &success );
		if( !success )
		{
			glGetShaderInfoLog( shader, 1024, NULL, infoLog );
			Debug::LogError( "ERROR::SHADER_COMPILATION_ERROR of type: " + type + "\n" + infoLog + "\n" );
		}
	}
	else
	{
		glGetProgramiv( shader, GL_LINK_STATUS, &success );
		if( !success )
		{
			glGetProgramInfoLog( shader, 1024, NULL, infoLog );
			Debug::LogError( "ERROR::PROGRAM_LINKING_ERROR of type: " + type + "\n" + infoLog + "\n" );
		}
	}
}