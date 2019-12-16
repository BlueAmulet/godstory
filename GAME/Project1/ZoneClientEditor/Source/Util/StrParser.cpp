#include "StrParser.h"


//**********************************************************
CString::CString( int nSize /* = 255 */ )
{
	mBuffer = new char[nSize];
	*mBuffer = 0;
	mUsedSize = 0;
	mSize = nSize;
}

CString::~CString()
{
	delete[] mBuffer;
}

CString::operator const char *()
{
	return mBuffer;
}

void CString::operator += ( const char* string )
{
	int nStrLen = strlen( string );
	
	if( nStrLen > mSize - mUsedSize )
		return ;

	dStrcat( mBuffer, mSize, string );

	mUsedSize += nStrLen;
}

void CString::operator = ( const char* string )
{
	int nStrLen = strlen( string );

	if( nStrLen > mSize )
		return ;

	dStrcpy( mBuffer, mSize, string );

	mUsedSize = nStrLen;
}

void CString::operator = ( const CString& string )
{
	int nStrLen = strlen( string.mBuffer );

	if( nStrLen > mSize )
		return ;

	dStrcpy( mBuffer, mSize, string.mBuffer );

	mUsedSize = nStrLen;
}

int CString::size() const
{
	return mUsedSize;
}

void CString::clear()
{
	mUsedSize = 0;
	*mBuffer = 0;
}

CStrParser::CStrParser( CStrLexer& lexer )
{
	m_pLexer = &lexer;
	mStartPrefix = '\\';
}

CStrParser::~CStrParser()
{

}

bool CStrParser::nextToken( stStrParserToken& token )
{
	stStrLexerToken lexerToken;

	int nLexerTokenCount = 0;

	token.mType = STR_PARSER_TOKEN_TYPE_UNKNOWN;
	token.mContent.clear();

	while( m_pLexer->PreGetNext( lexerToken ) )
	{
		nLexerTokenCount ++;

		if( lexerToken.nTokenType == STR_LEXER_TOKEN_TYPE_SYMBOL )
		{	
			if( *lexerToken.szBuffer == mStartPrefix && nLexerTokenCount > 1 )
				return true;

			// skip the symbol
			if(	!m_pLexer->GetNext( lexerToken ) )
				return false;

			if( *lexerToken.szBuffer ==mStartPrefix && m_PrefixMap.find( *( lexerToken.szBuffer + 1) ) != m_PrefixMap.end() )
			{
				token.mType = m_PrefixMap[*( lexerToken.szBuffer + 1)];

				if( m_pLexer->GetNext( lexerToken ) != STR_LEXER_TOKEN_TYPE_END )
				{		
					token.mContent = lexerToken.szBuffer;
					if( *lexerToken.szBuffer == '"' )
					{
						m_pLexer->getTokenUntil( lexerToken, '"' );
						token.mContent = lexerToken.szBuffer;
					}
					return true;
				}

				token.mContent.clear();
				return false;
			}
			else
			{
				token.mContent += lexerToken.szBuffer;	
				
				if( !m_pLexer->PreGetNext( lexerToken ) )
				{
					//token.mContent.clear();   暂时屏蔽掉,修正客户端聊天框如果内容中有符号,不正常
					return true;
				}


			}
		}
		else
		{
			if( !m_pLexer->GetNext( lexerToken ) )
			{
				token.mContent.clear();
				return false;
			}

			token.mContent += lexerToken.szBuffer;
		}
	}

	if( nLexerTokenCount == 0 )
	{
		token.mContent.clear();
		return false;
	}

	return true;
}

void CStrParser::addPrefix( char cPrefix, int nType )
{
	m_PrefixMap[cPrefix] = nType;
}

