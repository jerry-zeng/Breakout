#include "Utils.h"
#include <io.h>
#include <strstream>
#include <sstream>
#include <ostream>
#include <istream>

using namespace std;

namespace Utils 
{
	std::string ToString( unsigned int value )
	{
		std::stringstream ss;
		ss << value;
		return ss.str();
	}

	std::string ToString( int value )
	{
		std::stringstream ss;
		ss << value;
		return ss.str();
	}

	std::string ToString( int value1, int value2 )
	{
		std::stringstream ss;
		ss << "(" << value1 << "," << value2 << ")";
		return ss.str();
	}

	std::string ToString( float value )
	{
		std::stringstream ss;
		ss << value;
		return ss.str();
	}

	std::string ToString( float value1, float value2 )
	{
		std::stringstream ss;
		ss << "(" << value1 << ", " << value2 << ")";
		return ss.str();
	}

	std::string ToString( float value1, float value2, float value3 )
	{
		std::stringstream ss;
		ss << "(" << value1 << ", " << value2 << ", " << value3 << ")";
		return ss.str();
	}

	std::string ToString( float value1, float value2, float value3, float value4 )
	{
		std::stringstream ss;
		ss << "(" << value1 << ", " << value2 << ", " << value3 << ", " << value4 << ")";
		return ss.str();
	}

	std::string ToString( double value )
	{
		std::stringstream ss;
		ss << value;
		return ss.str();
	}

	int ToInt( std::string value )
	{
		std::stringstream ss;
		ss << value;

		int out = 0;
		ss >> out;
		return out;
	}
}
