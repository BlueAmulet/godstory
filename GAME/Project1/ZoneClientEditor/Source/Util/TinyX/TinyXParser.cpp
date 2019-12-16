#include "TinyXParser.h"


CTinyXParser::CTinyXParser(  const  char* mText )
{
	mLexer = new CTinyXLexer( mText );
}

CTinyXParser::~CTinyXParser()
{
	delete mLexer;
}

bool CTinyXParser::parseContent( CTinyXDocument& document, bool clearDocument  )
{
	mElementStack.clear();
	mElementStack.push_front( document.getRoot() );

	if( clearDocument )
		document.getRoot()->removeAllChildren();

	TinyXToken token;
	int nRet = 0;
	CTinyXElement* pElement = NULL;

	while( true )
	{
		nRet = mLexer->nextUntil( token, '<' );
		if( nRet == -1 )
		{
			break;
		}
		else if( nRet == 1 )
		{
			if( pElement == NULL)
			{		
				pElement = document.createElement();
				mElementStack.front()->insertChild( pElement );
				pElement->setParent( mElementStack.front() );
				pElement->setTagName( "t" );
			}
			pElement->setContent( token.buf );
		}
		else
		{
			// 起始不合法
			if( mLexer->nextChar() != '<' )
				return false;

			mLexer->skipWhiteSpaces();

			mLexer->nextLabel( token );
			
			// 标签结束
			if( token.buf[0] == '/' )
			{
				mLexer->skipWhiteSpaces();
				mLexer->nextLabel( token );
				
				// 结束标签名不符合起始标签名
				if( strcmp( mElementStack.front()->getTagName(), token.buf ) !=0 )
					return false;

				mLexer->skipWhiteSpaces();
				// 结束不合法
				if( mLexer->nextChar() != '>' )
					return false;
				
				mElementStack.pop_front();
				pElement = NULL;
			}
			else
			{
				pElement = document.createElement();
				mElementStack.front()->insertChild( pElement );	
				pElement->setParent( mElementStack.front() );

				// 所有标签名称均为小写
				token.toLower();

				pElement->setTagName( token.buf );

				// 取得所有属性
				while( true )
				{
					mLexer->skipWhiteSpaces();

					mLexer->nextLabel( token );			
					
					// 属性结束
					if( token.buf[0] == '>' )
					{
						mElementStack.push_front( pElement );
						break;
					}
					else if( token.buf[0] == '/' )
					{
						mLexer->skipWhiteSpaces();
						
						// 结束不合法
						if( mLexer->nextChar() != '>' )
							return false;

						break;
					}
					else
					{
						CTinyXAttribute* pAttribute = new CTinyXAttribute();

						pAttribute->setParent( pElement );
						
						// 所有属性名称为小写
						token.toLower();
						pAttribute->setName( token.buf );

						mLexer->skipWhiteSpaces();

						// 赋值不合法
						if( mLexer->nextChar() != '=' )
							return false;

						mLexer->skipWhiteSpaces();

						// 不是有效值
						if( !mLexer->nextString( token ) )
							return false;

						pAttribute->setValue( token.buf );

						pElement->insertAttribute( pAttribute );
					}
				}
			}
		}
	}

	document.refresh();
	return true;
}

