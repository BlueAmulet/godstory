#pragma once

#include "TinyXLexer.h"
#include "TinyXElement.h"
#include "TinyXDocument.h"

#include <deque>

class CTinyXParser
{
	typedef std::deque< CTinyXElement* > ElementStack;

public:
	CTinyXParser( const  char* mText );
	virtual ~CTinyXParser();

	bool parseContent( CTinyXDocument& document, bool clearDocument = true );
private:
	CTinyXLexer* mLexer;

	ElementStack mElementStack;
} ;

