#include "TinyXNode.h"

CTinyXNode::CTinyXNode()
{
	mParent = NULL;
}

CTinyXNode::~CTinyXNode()
{
	removeAllChildren();
}

void CTinyXNode::setParent( CTinyXNode* pParent )
{
	if( !pParent )
		return ;

	mParent = pParent;
}

CTinyXNode* CTinyXNode::getParent()
{
	return mParent;
}

CTinyXNode::NodeList::iterator CTinyXNode::_findChild( CTinyXNode* pNode )
{
	NodeList::iterator it;

	for( it = mChildren.begin(); it != mChildren.end(); it++ )
	{
		if( *it == pNode )
			break;
	}

	return it;
}

void CTinyXNode::insertChild( CTinyXNode* pNewNode, CTinyXNode* pInsertAfter )
{
	if( pInsertAfter == NULL )
	{
		mChildren.push_back( pNewNode );
	}
	else
	{
		NodeList::iterator it;
		it = _findChild( pInsertAfter );
		mChildren.insert( it,  pNewNode );
	}
}

void CTinyXNode::removeAllChildren()
{
	for each( CTinyXNode* pNode in mChildren )
	{
		delete pNode;
	}

	mChildren.clear();
}

CTinyXNode::NodeList& CTinyXNode::getChildren()
{
	return mChildren;
}