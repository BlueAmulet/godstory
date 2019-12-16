#include "TinyXToken.h"
#include "string.h"

TinyXToken::TinyXToken()
{
	memset( this, 0, sizeof( char ) * MAX_TOKEN_SIZE );
}

void TinyXToken::toLower()
{
	_strlwr_s(buf, sizeof(buf));
}
