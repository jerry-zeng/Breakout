#include "TextRenderer.h"
#include "GL.h"
#include "Debug.h"
#include "../Game.h"

#include <ft2build.h>
#include FT_FREETYPE_H

TextRenderer::TextRenderer( Shader shader )
	:_shader(shader)
	,_fontSize(20)
	,_fontName("")
{
	Init();

	charSet = std::map<char, Character>();
}


TextRenderer::~TextRenderer()
{
	Clean();

	glDeleteVertexArrays( 1, &_vao );
	glDeleteBuffers( 1, &_vbo );
}

void TextRenderer::Clean()
{
	std::map<char, Character>::iterator iter;

	for( iter = charSet.begin(); iter != charSet.end(); ++iter )
	{
		glDeleteTextures( 1, &iter->second.texID );
	}
	charSet.clear();
}

void TextRenderer::Init()
{
	glGenVertexArrays( 1, &_vao );
	glBindVertexArray( _vao );

	glGenBuffers( 1, &_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, _vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof( GLfloat ) * 6 * 4, NULL, GL_DYNAMIC_DRAW );

	glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof( GLfloat ), (const void*)0 );
	glEnableVertexAttribArray( 0 );

	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	glBindVertexArray( 0 );
}

void TextRenderer::LoadFont( const char* fontName, unsigned int fontSize )
{
	if( fontName )
		_fontName = fontName;
	else{
		Debug::LogError("Null fontName!");
		return;
	}

	if( fontSize < 10 )
		fontSize = 10;
	_fontSize = fontSize;

	Clean();

	// init freetype
	FT_Library ft;
	if( FT_Init_FreeType( &ft ) )
		Debug::LogError( "can't init free type" );

	// load ttf
	FT_Face ttf;
	if( FT_New_Face( ft, _fontName.c_str(), 0, &ttf ) )
		Debug::LogError( "can't load ttf" );

	// set font size
	FT_Set_Pixel_Sizes( ttf, 0, _fontSize );

	// pre load char 0~127
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 ); //禁用字节对齐限制.

	for( GLubyte i = 0; i < 128; i++ )
	{
		if( FT_Load_Char( ttf, i, FT_LOAD_RENDER ) )
		{
			Debug::LogWarnning( ("can't load char " + Utils::ToString( (int)i )).c_str() );
			continue;
		}

		GLuint tex;
		glGenTextures( 1, &tex );
		glBindTexture( GL_TEXTURE_2D, tex );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RED,
			ttf->glyph->bitmap.width,
			ttf->glyph->bitmap.rows,
			0, GL_RED, GL_UNSIGNED_BYTE,
			ttf->glyph->bitmap.buffer
		);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		Character cha;
		cha.texID = tex;
		cha.size = glm::ivec2( ttf->glyph->bitmap.width, ttf->glyph->bitmap.rows );
		cha.bearing = glm::ivec2( ttf->glyph->bitmap_left, ttf->glyph->bitmap_top );
		cha.advance = ttf->glyph->advance.x;

		charSet.insert( std::pair<char, Character>( (char)i, cha ) );
	}
	glBindTexture( GL_TEXTURE_2D, 0 );

	// release font resource
	FT_Done_Face( ttf );
	FT_Done_FreeType( ft );

}

void TextRenderer::RenderText( std::string text, glm::vec2 pos, glm::vec3 color, float scale, float alpha )
{
	if( _fontName.empty() || text.empty() )
		return;

	glEnable( GL_CULL_FACE );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	
	_shader.use();
	_shader.setInt( "Texture0", 0 );
	_shader.setVec4( "textColor", glm::vec4(color, alpha) );
	_shader.setMat4( "projection", Game::getInstance()->getProjection() );

	glBindVertexArray( _vao );

	glActiveTexture( GL_TEXTURE0 );

	for( size_t i = 0; i < text.length(); i++ )
	{
		char c = text.at( i );
		Character cha = charSet.at( c );

		float posX = pos.x + cha.bearing.x * scale;
		float posY = pos.y - (cha.size.y - cha.bearing.y) * scale;

		float w = cha.size.x * scale;
		float h = cha.size.y * scale;
		// . .
		// . .
		GLfloat vertices[6][4] = {
			{posX,   posY + h, 0.0, 0.0}, //top-left
			{posX,   posY,   0.0, 1.0},  //bottom-left
			{posX + w, posY,   1.0, 1.0},  //bottom-right

			{posX,   posY + h, 0.0, 0.0},  //top-left
			{posX + w, posY,   1.0, 1.0},  //bottom-right
			{posX + w, posY + h, 1.0, 0.0},  //top-right
		};

		glBindTexture( GL_TEXTURE_2D, cha.texID );

		glBindBuffer( GL_ARRAY_BUFFER, _vbo );
		glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( vertices ), vertices );
		glBindBuffer( GL_ARRAY_BUFFER, 0 );

		glDrawArrays( GL_TRIANGLES, 0, 6 );

		pos.x += (cha.advance / 64) * scale; // 位偏移6个单位来获取单位为像素的值 (2^6 = 64).
	}

	glBindVertexArray( 0 );
	glBindTexture( GL_TEXTURE_2D, 0 );

	glDisable( GL_CULL_FACE );
}