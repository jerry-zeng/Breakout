
#include "Debug.h"

#include <iostream>


void Debug::Log(const char* txt)
{
	std::cout << "[I]" << txt << std::endl;
}
void Debug::Log( const std::string &txt )
{
	Log( txt.c_str() );
}

void Debug::LogWarnning( const char* txt )
{
	std::cout << "[W]" << txt << std::endl;
}
void Debug::LogWarnning( const std::string &txt )
{
	LogWarnning( txt.c_str() );
}

void Debug::LogError( const char* txt )
{
	std::cout << "[E]" << txt << std::endl;
}
void Debug::LogError( const std::string &txt )
{
	LogError( txt.c_str() );
}
