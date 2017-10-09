#ifndef __Debug_H__
#define __Debug_H__

#include <string>

class Debug
{
//private:
//	static char* _buffer;

public:
	static void Log(const char* txt);
	static void Log( const std::string &txt );
	static void LogWarnning(const char* txt);
	static void LogWarnning( const std::string &txt );
	static void LogError( const char* txt );
	static void LogError( const std::string &txt );
};

#endif