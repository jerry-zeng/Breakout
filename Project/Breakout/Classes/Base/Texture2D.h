#pragma once
#ifndef Texture2D_H
#define Texture2D_H

typedef unsigned int UINT;

class Texture2D
{
public:
	Texture2D();
	virtual ~Texture2D();

	void Generate( UINT w, UINT h, unsigned char* data );
	void Bind();

public:
	UINT ID;
	UINT width, height;
	UINT internal_format;
	UINT image_format;
	UINT wrap_s;
	UINT wrap_t;
	UINT filter_min;
	UINT filter_mag;
};

#endif // !Texture2D_H