#include "Texture2D.h"
#include "GL.h"

typedef UINT GLuint;

Texture2D::Texture2D()
	:width(0)
	,height(0)
	,internal_format( GL_RGB )
	,image_format( GL_RGB )
	,wrap_s( GL_REPEAT )
	,wrap_t( GL_REPEAT )
	,filter_min( GL_LINEAR )
	,filter_mag( GL_LINEAR )
{
	glGenTextures( 1, &ID );
}

Texture2D::~Texture2D()
{

}

void Texture2D::Generate( GLuint w, GLuint h, unsigned char* data )
{
	width = w;
	height = h;

	glBindTexture( GL_TEXTURE_2D, ID );
	glTexImage2D( GL_TEXTURE_2D, 0, internal_format, width, height, 0, image_format, GL_UNSIGNED_BYTE, data );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_min );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_mag );
	glBindTexture( GL_TEXTURE_2D, 0 );
}

void Texture2D::Bind()
{
	glBindTexture( GL_TEXTURE_2D, ID );
}