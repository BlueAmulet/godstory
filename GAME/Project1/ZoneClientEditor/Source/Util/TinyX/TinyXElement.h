#pragma once

#include "TinyXAttribute.h"

class CTinyXElement : public CTinyXNode
{
public:
	CTinyXElement();
	virtual ~CTinyXElement();

	void setTagName( const char* name );
	const char* getTagName();

	void setContent( const char* content );
	const char* getContent();

	const TinyXAttributeMap* getAttributes() const;

	void setAttribute( const char* name, const char* value );
	const char* getAttribute( const char* name );

	void insertAttribute( CTinyXAttribute* pAttribute );
	void removeAttribute( const char* name );

private:
	char* mTagName;
	char* mContent;

	TinyXAttributeMap mAttributes;

	static const int MAX_TAG_NAME_LENGTH = 32;
	static const int MAX_CONTENT_LENGTH =  10240;
} ;

#define TinyXElementList std::list< CTinyXElement* >
#define TinyXElementMap stdext::hash_map< const char*, CTinyXElement* >

