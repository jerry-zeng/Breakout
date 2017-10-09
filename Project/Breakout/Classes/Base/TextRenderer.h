#pragma once
#ifndef TextRenderer_H
#define TextRenderer_H

#include "Shader.h"
#include <glm/glm.hpp>
#include <map>
#include <string>

struct Character
{
	unsigned int texID;
	glm::ivec2 size;
	glm::ivec2 bearing;
	unsigned int advance;
};

class TextRenderer
{
public:
	TextRenderer(Shader shader );
	virtual ~TextRenderer();

	void LoadFont( const char* fontName, unsigned int fontSize = 20 );
	void RenderText( std::string text, glm::vec2 pos, glm::vec3 color = glm::vec3(1.0), float scale = 1.0, float alpha = 1.0 );

	inline std::string& getFontName() { return _fontName; }
	inline unsigned int getFontSize() { return _fontSize; }

private:
	void Init();
	void Clean();

private:
	Shader _shader;
	unsigned int _vao, _vbo;

	std::string _fontName;
	unsigned int _fontSize;
	std::map<char, Character> charSet;
};

#endif // !TextRenderer_H