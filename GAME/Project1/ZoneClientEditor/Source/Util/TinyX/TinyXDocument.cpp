#include "TinyXDocument.h"


CTinyXElement* CTinyXDocument::createElement( const char* id )
{
	CTinyXElement* pElement = new CTinyXElement();
	if( id != NULL )
		pElement->setAttribute( "id", id );
	mElementList.push_back( pElement );
	return pElement;
}

CTinyXElement* CTinyXDocument::getRoot()
{
	return &mRoot;
}


TinyXElementList* CTinyXDocument::getAllElements() const
{
	return (TinyXElementList*)&mElementList;
}


CTinyXDocument::CTinyXDocument()
{
}


CTinyXDocument::~CTinyXDocument()
{

}


void CTinyXDocument::refresh()
{
	const char* pId;
	const char* pName;
	
	mIdElementMap.clear();
	mNameElementMap.clear();

	for each( CTinyXElement* pElement in mElementList )
	{
		pId = pElement->getAttribute( "id" );
		pName = pElement->getAttribute( "name" );

		if( pId )
			mIdElementMap[pId] = pElement;

		if( pName )
			mNameElementMap[pName] = pElement;
	}
}



void CTinyXDocument::GetElementsByTagName( std::list< CTinyXElement* >& elementList, const char* tagName )
{
	for each( CTinyXElement* pElement in mElementList )
	{
		if( strcmp( pElement->getTagName(), tagName ) == 0 )
		{
			elementList.push_back( pElement );
		}
	}
}


CTinyXElement* CTinyXDocument::GetElementById( const char* id )
{
	TinyXElementMap::iterator it;

	it = mIdElementMap.find( id );
	if( it != mIdElementMap.end() )
		return it->second;

	return NULL;
}

CTinyXElement* CTinyXDocument::GetElementByName( const char* name )
{
	TinyXElementMap::iterator it;

	it = mNameElementMap.find( name );
	if( it != mNameElementMap.end() )
		return it->second;

	return NULL;
}

void CTinyXDocument::clear()
{
	mRoot.removeAllChildren();

	mElementList.clear();
}