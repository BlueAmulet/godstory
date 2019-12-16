#pragma once

#include <string>
#include <string.h>

class CTinyXString : public std::string
{
public:
	CTinyXString()
	{

	}

	CTinyXString( const char* other )
	{
		__super::operator = ( other );
	}

	operator const char* ()
	{
		return __super::c_str();
	}

	
};