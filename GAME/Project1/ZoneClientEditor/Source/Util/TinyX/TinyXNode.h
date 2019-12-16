#pragma once

#include <string>
#include <list>

#include "platform/MemoryManagerInclude.h"

class CTinyXNode : public CVirtualAllocBase
{
public:
	typedef std::list<CTinyXNode*> NodeList;
	
	CTinyXNode();
	virtual ~CTinyXNode();

	void removeAllChildren();

	CTinyXNode* getParent();
	NodeList& getChildren();

	void setParent( CTinyXNode* pParent );
	void insertChild( CTinyXNode* pNewNode, CTinyXNode* pInsertAfter = NULL );
	void removeChild( CTinyXNode* pNode, bool isDelete = true );

private:
	NodeList::iterator _findChild( CTinyXNode* pNode );

private:
	NodeList 		mChildren;
	CTinyXNode* 	mParent;
} ;