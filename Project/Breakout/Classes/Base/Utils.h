#pragma once
#ifndef __Utils_H__
#define __Utils_H__

#include <string>

namespace Utils
{
	std::string ToString( unsigned int value );
	std::string ToString(int value);
	std::string ToString( int value1, int value2 );
	std::string ToString(float value);
	std::string ToString( float value1, float value2 );
	std::string ToString( float value1, float value2, float value3 );
	std::string ToString( float value1, float value2, float value3, float value4 );
	std::string ToString( double value );
	int ToInt( std::string value );
}

#endif