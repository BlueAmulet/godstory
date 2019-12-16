#ifndef STR_LEXER_H
#define STR_LEXER_H

#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

#define STR_LEXER_TOKEN_SIZE	128

enum eStrTokenType
{
	STR_LEXER_TOKEN_TYPE_END,					// 结束
	STR_LEXER_TOKEN_TYPE_UNKNOWN,				// 未知或非法
	STR_LEXER_TOKEN_TYPE_BLANK,					// 空格
	STR_LEXER_TOKEN_TYPE_SYMBOL,				// 符号
	STR_LEXER_TOKEN_TYPE_RETURN,				// 换行
	STR_LEXER_TOKEN_TYPE_NUM,					// 数字
	STR_LEXER_TOKEN_TYPE_ALPHABET,				// 字母
};

struct stStrLexerToken
{
	int nTokenType;
	char szBuffer[STR_LEXER_TOKEN_SIZE];
	int nSize;
public:
	stStrLexerToken()
	{
		memset( this, 0, sizeof( stStrLexerToken ) );
	}

	void clear()
	{
		memset( this, 0, sizeof( stStrLexerToken ) );
	}

	char operator * ()
	{
		return szBuffer[0];
	}

	operator int()
	{
		return atoi( szBuffer );
	}

	operator unsigned char ()
	{
		return (unsigned char)atoi( szBuffer );
	}

	operator char()
	{
		return (char)atoi( szBuffer );
	}

	operator const char* ()
	{
		return szBuffer;
	}

	operator char* ()
	{
		return szBuffer;
	}

	operator float ()
	{
		return (float)atof( szBuffer );
	}

	operator double ()
	{
		return atof( szBuffer );
	}
};

extern char g_symbol_list[];

class CStrLexer
{
public:
	CStrLexer( const char* pStrBuffer );
	virtual ~CStrLexer();

	int GetNext( stStrLexerToken& strToken );
	void getTokenUntil( stStrLexerToken& token, char cEnd );
	int GetNextType();
	int PreGetNext( stStrLexerToken& strToken );

private:

	int GetType( char c );
	bool IsNumberic( char c );
	bool IsAlphabet( char c );
	bool IsSymbol( char c );
	bool IsReturn( char c );
	bool IsBlank( char c );
	bool IsEnd( char c );

	int m_nTokenType;
	char* m_pPos;
	const char* m_pBuffer;

};


#endif