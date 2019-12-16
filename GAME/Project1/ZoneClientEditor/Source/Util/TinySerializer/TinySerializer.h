#ifndef TINY_SERIALIZER_H
#define TINY_SERIALIZER_H

#pragma once

#include <cstdio>
#include <string>
#include <hash_map>
#include "core\fileStream.h"

struct tagTinySerializerFileHeader
{
	int magic;
	int version;
	int unknown;

	tagTinySerializerFileHeader()
	{
		memset( this, 0, sizeof( tagTinySerializerFileHeader ) );
	}

	static tagTinySerializerFileHeader CurrentHeader;
	static const int CurrentVersion = 0x10022615;   //年月日时
};

class CTinyStream
{
public:
	enum OpenTypes
	{
		Read = (int)( std::ios_base::in | std::ios_base::binary ),
		Write = (int)( std::ios_base::out | std::ios_base::binary ),
	};

	CTinyStream( const char* fileName, OpenTypes type );
	virtual ~CTinyStream();

	void readString( char* buf, size_t size );
	void writeString( const char* pBuf ); 

	void readByte( char* buf, size_t size );
	void writeByte( const char* buf, size_t size );
private:
	FILE* mFp;
	Stream* mStream;
};

template< typename _Ty >
CTinyStream& operator << ( CTinyStream& stream, _Ty& value )
{
	stream.writeByte( (const char*)&value, sizeof( _Ty ) );
	return stream;
}

template< typename _Ty >
CTinyStream& operator >> ( CTinyStream& stream, _Ty& value )
{
	stream.readByte( (char*)&value, sizeof( _Ty ) );
	return stream;
}

struct ITinySerialable
{
	virtual void OnSerialize(CTinyStream& pStream) { }
	virtual void OnUnserialize(CTinyStream& pStream) { }
};

namespace TinySerializer
{
	template< typename _Ty >
	static void* createObject()
	{
		return (void*)( new _Ty );
	}
}

typedef void* (*_createObjectFunction)();

class CTinySerializer
{
public:
	static void registerClassType( std::string objectTypeName, _createObjectFunction function );
	typedef stdext::hash_map< std::string , _createObjectFunction > CreateMap;

	static CreateMap& getCreateMap();

	static void* createObject( std::string objectTypeName  );

	void serializer( ITinySerialable* pSerialable, std::string objectTypeName, CTinyStream& stream );
	void* unserializer( CTinyStream& stream );

	static const int CLASS_NAME_LENGTH = 32;
};

#define DECLARE_SERIALIBLE(p) \
struct class##p##serializer \
{ \
	class##p##serializer() \
	{ \
		CTinySerializer::registerClassType( typeid(p).name(), TinySerializer::createObject<p> ); \
	} \
}; \
static class##p##serializer class##p##serializerCaster; 




#endif