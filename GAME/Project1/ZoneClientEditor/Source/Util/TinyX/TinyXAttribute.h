#pragma once

#include <hash_map>

#include "TinyXNode.h"
#include "TinyXString.h"

class CTinyXAttribute : public CTinyXNode
{
public:
	void setName( const CTinyXString name ) { mName = name; }
	CTinyXString& getName() { return mName; }

	void setValue( const CTinyXString value ) { mValue = value; }
	CTinyXString& getValue() { return mValue; }

private:
	CTinyXString mName;
	CTinyXString mValue;
} ;

static size_t hash_value( const CTinyXString& _Str )
{	// hash string to size_t value
	const char *_Ptr = _Str.c_str();

	return (stdext::_Hash_value(_Ptr, _Ptr + _Str.size()));
}

#define TinyXAttributeMap stdext::hash_map< CTinyXString, CTinyXAttribute* >