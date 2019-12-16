#pragma once

#include "TinyXToken.h"

class CTinyXLexer
{
public:
	CTinyXLexer( const char* text );
	CTinyXLexer( const CTinyXLexer& lexer );

	virtual ~CTinyXLexer();

	bool next( TinyXToken& token );
	char nextChar();
	int nextUntil( TinyXToken& token, const char c );

	bool nextLabel( TinyXToken& token );
	bool nextString( TinyXToken& token );

	void skipWhiteSpaces();
	void skipChar();

	const char* getPointer();

private:
	TinyXCharType _getType( const char c ) const;

	bool _isAlphabet( const char c ) const;
	bool _isNumber( const char c ) const;
	bool _isSymbol( const char c ) const;

private:
	static char mSymbolSet[];
	static const int SYMBOL_SET_COUNT = 33;

	char* mText;

	TinyXCharType mLastCharType;
} ;
