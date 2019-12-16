#ifndef STR_PARSER_H
#define STR_PARSER_H

#pragma once

#include "BuildPrefix.h"
#include "platform/platformStrings.h"
#include <string>
#include <sstream>
#include "StrLexer.h"
#include <hash_map>

const int STR_PARSER_TOKEN_TYPE_UNKNOWN = -1;

class CString
{
public:
	CString( int nSize = 255 );
	virtual ~CString();

	int size() const;
	void clear();

	void operator += ( const char* string );
	operator const char*();
	void operator = ( const char* string );
	void operator = ( const CString& string );

private:
	int mSize;
	int mUsedSize;

	char* mBuffer;
};

struct stStrParserToken
{
	int mType;
	CString mContent;

	stStrParserToken()
	{
		mType = 0;
	}
};


class CStrParser
{
public:
	CStrParser( CStrLexer& lexer );
	virtual ~CStrParser();

	// the number of type must above 1
	void addPrefix( char cPrefix, int nType );

	bool nextToken( stStrParserToken& token );

	void setStartPrefix( char cPrefix );
private:
	CStrLexer* m_pLexer;

	stdext::hash_map< char, int > m_PrefixMap;

	char mStartPrefix;
} ;

#endif