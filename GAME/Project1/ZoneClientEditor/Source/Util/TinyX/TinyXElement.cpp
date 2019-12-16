#include "TinyXElement.h"


CTinyXElement::CTinyXElement()
{
	mTagName = new char[MAX_TAG_NAME_LENGTH + 1];
	mContent = new char[MAX_CONTENT_LENGTH + 1];
	mTagName[0] = 0;
	mContent[0] = 0;
}


CTinyXElement::~CTinyXElement()
{
	delete[] mTagName;
	delete[] mContent;

	TinyXAttributeMap::iterator it;
	for( it = mAttributes.begin(); it != mAttributes.end(); it++ )
	{
		delete it->second;
	}

}


void CTinyXElement::setTagName( const char* name )
{
	strncpy_s( mTagName, MAX_TAG_NAME_LENGTH + 1, name, MAX_TAG_NAME_LENGTH );
}


void CTinyXElement::setContent( const char* content )
{
	strncpy_s( mContent, MAX_CONTENT_LENGTH + 1, content, MAX_CONTENT_LENGTH );
}

const char* CTinyXElement::getTagName()
{
	return mTagName;
}

const char* CTinyXElement::getContent()
{
	return mContent;
}

const TinyXAttributeMap* CTinyXElement::getAttributes() const 
{
	return &mAttributes;
}

void CTinyXElement::setAttribute( const char* name, const char* value )
{
	TinyXAttributeMap::iterator it;
	it = mAttributes.find( name );
	if( it != mAttributes.end() )
	{
		it->second->setValue( value );
	}
	else
	{
		CTinyXAttribute* pNew = new CTinyXAttribute();
		pNew->setName( name );
		pNew->setValue( value );
		mAttributes[name] = pNew;
	}
}

const char* CTinyXElement::getAttribute( const char* name )
{
	TinyXAttributeMap::iterator it;
	it = mAttributes.find( name );
	if( it != mAttributes.end() )
		return it->second->getValue().c_str();
	
	return NULL;
		
}

void CTinyXElement::insertAttribute( CTinyXAttribute* pAttribute )
{
	TinyXAttributeMap::iterator it;
	it = mAttributes.find( pAttribute->getName() );
	if( it != mAttributes.end() )
	{
		delete it->second;
		mAttributes.erase( it );
	}

	mAttributes.insert( TinyXAttributeMap::value_type( pAttribute->getName(), pAttribute ) ); //  [pAttribute->getName()] = pAttribute;
}

void CTinyXElement::removeAttribute( const char* name )
{
	TinyXAttributeMap::iterator it;
	it = mAttributes.find( name );
	if( it != mAttributes.end() )
	{
		delete it->second;
		mAttributes.erase( it );
	}	
}

