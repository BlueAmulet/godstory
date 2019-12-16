#include "TinySerializer.h"
#include "console/consoleTypes.h"
#include <windows.h>
#include "core/resManager.h"

tagTinySerializerFileHeader tagTinySerializerFileHeader::CurrentHeader;

CTinyStream& operator<<( CTinyStream& stream, int value )
{
	stream.writeByte( (char*)&value, sizeof( value ) );
	return stream;
}

CTinyStream::CTinyStream( const char* fileName, CTinyStream::OpenTypes type )
{
	if( type == Write )
	{
		fopen_s(&mFp, fileName, "wb+" );
		mStream = NULL;
	}
	else
	{
		mStream = ResourceManager->openStream(fileName);
		mFp = NULL;
	}
}

CTinyStream::~CTinyStream()
{
	if( mFp )
		fclose( mFp );

	if( mStream )
		ResourceManager->closeStream( mStream );
}

void CTinyStream::writeByte( const char* buf, size_t size )
{
	fwrite( buf, 1, size, mFp );
}

void CTinyStream::readByte( char* buf, size_t size )
{
	if( !mStream )
		return ;

	mStream->read( size, buf );
	//fread( buf, 1, size, mFp );
}

void CTinyStream::readString( char* buf, size_t size )
{
	size_t slen;
	*this >> slen;

	if( slen > size - 1  )
		slen = size - 1;

	readByte( buf, slen );
	buf[slen] = 0;
}

void CTinyStream::writeString( const char* pBuf )
{
	if( pBuf )
	{	
		size_t slen = strlen( pBuf );
		*this << slen;
		writeByte( pBuf, slen );
	}
	else
	{
		int slen = 0;
		*this << slen;
	}
}


void CTinySerializer::registerClassType( std::string objectTypeName, _createObjectFunction function )
{
	getCreateMap()[objectTypeName] = function;	
}

void* CTinySerializer::createObject( std::string objectTypeName )
{
	CreateMap::iterator it = getCreateMap().find( objectTypeName );
	if( it == getCreateMap().end() )
		return NULL;

	_createObjectFunction funct = it->second;
	return (*funct)();
}

void CTinySerializer::serializer( ITinySerialable* pSerialable, std::string objectTypeName, CTinyStream& stream )
{
	char buf[32] = { 0 };
	sprintf_s( buf, sizeof( buf ), objectTypeName.c_str() );
	stream.writeString( buf );
	pSerialable->OnSerialize( stream );
}

void* CTinySerializer::unserializer( CTinyStream& stream )
{
	char buf[32];
	stream.readString( buf, 32 );

	ITinySerialable* pSerialable = (ITinySerialable*)createObject( buf );
	pSerialable->OnUnserialize( stream );
	SimObject* pObj = dynamic_cast< SimObject* >( pSerialable );
	if( pObj )
		pObj->registerObject();

	return pSerialable;
}

CTinySerializer::CreateMap& CTinySerializer::getCreateMap()
{
	static CreateMap createMap;
	return createMap;
}