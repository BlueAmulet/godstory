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
			// ��ʼ���Ϸ�
			if( mLexer->nextChar() != '<' )
				return false;

			mLexer->skipWhiteSpaces();

			mLexer->nextLabel( token );
			
			// ��ǩ����
			if( token.buf[0] == '/' )
			{
				mLexer->skipWhiteSpaces();
				mLexer->nextLabel( token );
				
				// ������ǩ����������ʼ��ǩ��
				if( strcmp( mElementStack.front()->getTagName(), token.buf ) !=0 )
					return false;

				mLexer->skipWhiteSpaces();
				// �������Ϸ�
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

				// ���б�ǩ���ƾ�ΪСд
				token.toLower();

				pElement->setTagName( token.buf );

				// ȡ����������
				while( true )
				{
					mLexer->skipWhiteSpaces();

					mLexer->nextLabel( token );			
					
					// ���Խ���
					if( token.buf[0] == '>' )
					{
						mElementStack.push_front( pElement );
						break;
					}
					else if( token.buf[0] == '/' )
					{
						mLexer->skipWhiteSpaces();
						
						// �������Ϸ�
						if( mLexer->nextChar() != '>' )
							return false;

						break;
					}
					else
					{
						CTinyXAttribute* pAttribute = new CTinyXAttribute();

						pAttribute->setParent( pElement );
						
						// ������������ΪСд
						token.toLower();
						pAttribute->setName( token.buf );

						mLexer->skipWhiteSpaces();

						// ��ֵ���Ϸ�
						if( mLexer->nextChar() != '=' )
							return false;

						mLexer->skipWhiteSpaces();

						// ������Чֵ
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

