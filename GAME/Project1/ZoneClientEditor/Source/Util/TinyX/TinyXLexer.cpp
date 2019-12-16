#include "TinyXLexer.h"


char CTinyXLexer::mSymbolSet[] = { ' ', '~', '`', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '-', '_', '+', '=', '{', '}', '[', ']', ':', ';', '"', '\'', '\\', '|', '<', '>', ',', '.', '/', '?' };


CTinyXLexer::CTinyXLexer( const char* text )
{
	mText = ( char* )text;
	mLastCharType = _getType( *text );
}

CTinyXLexer::CTinyXLexer( const CTinyXLexer& lexer )
{
	mText = lexer.mText;
	mLastCharType = lexer.mLastCharType;
}

CTinyXLexer::~CTinyXLexer()
{
	// do nothing
}

inline bool CTinyXLexer::_isAlphabet( const char c ) const 
{
	return ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) || c < 0;
}


inline bool CTinyXLexer::_isNumber( const char c ) const 
{
	return c <= '9' && c >= '0';
}


inline bool CTinyXLexer::_isSymbol( const char c ) const 
{
	for( int i=0; i < SYMBOL_SET_COUNT * sizeof( char ); i++ )
	{
		if( c == *( mSymbolSet + i ) )
			return true;
	}

	return false;
}

TinyXCharType CTinyXLexer::_getType( const char c ) const
{
	if( c ==  0 )
		return CHAR_TYPE_EOS;

	if( _isAlphabet( c ) )
		return CHAR_TYPE_ALPHABET;

	if( _isNumber( c ) )
		return CHAR_TYPE_NUMBER;

	if( _isSymbol( c ) ) 
		return CHAR_TYPE_SYMBOL;

	return CHAR_TYPE_UNKNOWN;
}

bool CTinyXLexer::next( TinyXToken& token )
{
	// when eos
	if( *mText == 0 )
		return false;

	int nCount = 0;
	char* pDest = ( char* )token.buf;
	TinyXCharType ct = CHAR_TYPE_UNKNOWN;
	while( nCount++ < TinyXToken::MAX_TOKEN_SIZE && *mText != 0 )
	{
		ct = _getType( *mText );
		if( ct != mLastCharType )
			break;

		*pDest++ = *mText++;
	}

	*pDest = 0;
	mLastCharType = ct;
	return true;
}


char CTinyXLexer::nextChar()
{
	mLastCharType = _getType( *mText );
	return *mText++;
}

int CTinyXLexer::nextUntil( TinyXToken& token, const char c )
{
	// when eos
	if( *mText == 0 )
	{
		token.buf[0] = 0;
		return -1;
	}

	if( *mText == c )
	{
		token.buf[0] = 0;
		return 0;
	}

	int nCount = 0;
	char* pDest = ( char* )token.buf;
	TinyXCharType ct = CHAR_TYPE_UNKNOWN;
	while( nCount++ < TinyXToken::MAX_TOKEN_SIZE )
	{
		if( *mText == 0 && nCount > 0)
			return 1;

		ct = _getType( *mText );
		if( *mText == c )
			break;

		*pDest++ = *mText++;
	}

	*pDest = 0;
	mLastCharType = ct;
	return 1;
}

#include <stack>

bool CTinyXLexer::nextString( TinyXToken& token )
{
	char c = nextChar();
	if( c == 0 || ( c != '"' && c != '\'' ) )
		return false;

	std::stack<char> charStack;

	char* p = token.buf;
	
	charStack.push( c );
	while( 1 )
	{
		c = nextChar();

		if( c == '"' || c == '\'' )
		{
			if( c == charStack.top() )
			{
				charStack.pop();

				if( charStack.size() == 0 )
					break;
			}
			else
			{
				charStack.push( c );
			}


		}

		*p++ = c;
	}
/*
	*p = 0;

	if( !nextUntil( token, c ) )
		return false;

	skipChar();*/


	*p = 0;
	return true;
}


void CTinyXLexer::skipWhiteSpaces()
{
	while( *mText == ' ' && *mText != 0 )
		mText++;

	mLastCharType = CHAR_TYPE_SYMBOL;
}

inline const char* CTinyXLexer::getPointer()
{
	return mText;
}

void CTinyXLexer::skipChar()
{
	mText++;
}

bool CTinyXLexer::nextLabel( TinyXToken& token )
{
	char c = *mText++;

	if( _getType( c ) != CHAR_TYPE_ALPHABET && c != '_' ) // 允许下划线起始标识
	{
		token.buf[0] = c;
		return false;
	}

	char* pDest= token.buf;
	int nCount = 0;
	TinyXCharType type;
	*pDest++ = c;
	c = *mText;
	while( c != 0  && nCount++ < TinyXToken::MAX_TOKEN_SIZE )
	{
		c = *mText;
		type = _getType( c );
		if( type != CHAR_TYPE_ALPHABET && type != CHAR_TYPE_NUMBER && c != '_' ) // 允许从第二个字母开始使用数字
			break;

		*mText++;
		*pDest++ = c;
	}
	*pDest = 0;
	return true;
}


