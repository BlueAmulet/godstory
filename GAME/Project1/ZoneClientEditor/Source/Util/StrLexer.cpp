#include "StrLexer.h"

char g_symbol_list[] = {'!','@','#','$','%','^','&','*','(',')','-','_','+','=','\\',':',';','"','\'',',','.','/','?','<','>','~','`'};


CStrLexer::CStrLexer( const char *pStrBuffer )
{
	m_nTokenType = GetType( *pStrBuffer );
	m_pPos = (char*)pStrBuffer;
	m_pBuffer = pStrBuffer;
}

CStrLexer::~CStrLexer()
{

}

int CStrLexer::GetNext(stStrLexerToken &strToken)
{
	if( !*m_pPos )
	{
		strToken.nTokenType = STR_LEXER_TOKEN_TYPE_END;
		return STR_LEXER_TOKEN_TYPE_END;
	}

	strToken.clear();

	int nType, nOldType;

	nOldType = m_nTokenType;

	char* pSrcBuf = m_pPos;
	char* pDestBuf = strToken.szBuffer;

	int nCount = 0;

	while( nCount++ < STR_LEXER_TOKEN_SIZE )
	{
		nType = GetType( *pSrcBuf );
		if( nType != m_nTokenType )
			break;

		*pDestBuf++ = *pSrcBuf++;
	}

	m_nTokenType = nType;
	m_pPos = pSrcBuf;
	strToken.nSize = nCount;
	strToken.nTokenType = nOldType;

	return nOldType;
}

int CStrLexer::PreGetNext(stStrLexerToken &strToken)
{
	if( !*m_pPos )
		return STR_LEXER_TOKEN_TYPE_END;

	strToken.clear();

	int nType, nOldType;

	nOldType = m_nTokenType;

	char* pSrcBuf = m_pPos;
	char* pDestBuf = strToken.szBuffer;

	int nCount = 0;

	while( nCount++ < STR_LEXER_TOKEN_SIZE )
	{
		nType = GetType( *pSrcBuf );
		if( nType != m_nTokenType )
			break;

		*pDestBuf++ = *pSrcBuf++;
	}

	strToken.nSize = nCount;
	strToken.nTokenType = nOldType;

	return nOldType;
}

int CStrLexer::GetType( char c )
{
	if( IsAlphabet(c) )
		return STR_LEXER_TOKEN_TYPE_ALPHABET;

	if( IsBlank(c) )
		return STR_LEXER_TOKEN_TYPE_BLANK;

	if( IsNumberic( c ) )
		return STR_LEXER_TOKEN_TYPE_NUM;

	if( IsReturn( c ) )
		return STR_LEXER_TOKEN_TYPE_RETURN;

	if( IsSymbol( c ) )
		return STR_LEXER_TOKEN_TYPE_SYMBOL;

	if( IsEnd( c ) )
		return STR_LEXER_TOKEN_TYPE_END;

	return STR_LEXER_TOKEN_TYPE_UNKNOWN;
}

bool CStrLexer::IsAlphabet(char c)
{
	if( c  < 0 || ( c >= 'A' && c <= 'Z') || ( c >='a' && c <= 'z' ) )
		return true;

	return false;
}

bool CStrLexer::IsNumberic(char c)
{
	if( c >= '0' && c <= '9' )
		return true;

	return false;
}

bool CStrLexer::IsReturn(char c)
{
	if( c == '\r' || c == '\n' )
		return true;

	return false;
}

bool CStrLexer::IsSymbol(char c)
{
	for( int i=0;i<27;i++)
	{
		if( c == g_symbol_list[i] )
			return true;
	}

	return false;
}

bool CStrLexer::IsBlank(char c)
{
	return c == ' ';
}

bool CStrLexer::IsEnd( char c )
{
	return c == 0;
}

void CStrLexer::getTokenUntil( stStrLexerToken& token, char cEnd )
{
	if( !*m_pPos )
	{
		token.nTokenType = STR_LEXER_TOKEN_TYPE_END;
		return ;
	}

	token.clear();

	char* pSrcBuf = m_pPos;
	char* pDestBuf = token.szBuffer;

	int nCount = 0;

	while( nCount++ < STR_LEXER_TOKEN_SIZE && *pSrcBuf != 0 )
	{
		if( *pSrcBuf == cEnd )
			break;

		*pDestBuf++ = *pSrcBuf++;
	}

	return ;
}

