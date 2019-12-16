#pragma once

#include "TinyXElement.h"
#include <hash_map>
#include <list>

class CTinyXDocument
{
public:
	CTinyXDocument();
	virtual ~CTinyXDocument();

	void clear();
	
	CTinyXElement* createElement( const char* id = NULL );
	CTinyXElement* getRoot();

	TinyXElementList* getAllElements() const;
	void GetElementsByTagName( TinyXElementList& elementList, const char* tagName );
	CTinyXElement* GetElementById( const char* id );
	CTinyXElement* GetElementByName( const char* name );

	void refresh();
private:
	CTinyXElement mRoot;

	TinyXElementMap mIdElementMap;
	TinyXElementMap mNameElementMap;
	TinyXElementList mElementList;
};
