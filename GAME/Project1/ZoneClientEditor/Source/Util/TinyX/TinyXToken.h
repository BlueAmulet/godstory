#pragma once

#include <memory.h>
#include <string.h>

struct TinyXToken
{
	enum token_type
	{
		TOKEN_TYPE_LEXER,
		TOKEN_TYPE_PARSER,
	} ;

	const static int MAX_TOKEN_SIZE = 2048;

	char buf[MAX_TOKEN_SIZE];

	TinyXToken();

	void toLower();
} ;

enum TinyXCharType
{
	CHAR_TYPE_EOS = 0,
	CHAR_TYPE_SYMBOL,
	CHAR_TYPE_NUMBER,
	CHAR_TYPE_ALPHABET,
	CHAR_TYPE_UNKNOWN,
};
