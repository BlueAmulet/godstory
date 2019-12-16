#include "TinyXAttribute.h"

template CTinyXAttribute<char>;
template CTinyXAttribute<wchar_t>;

template < typename T >
CTinyXAttribute<T>::CTinyXAttribute()
{
	mName = new T[ MAX_NAME_LENGTH + 1];
	mValue = new T[ MAX_VALUE_LENGTH + 1];
	mName[0] = 0;
	mValue[0] = 0;
}

template < typename T >
CTinyXAttribute<T>::~CTinyXAttribute()
{
	delete[] mName;
	delete[] mValue;
}

template<>
inline void CTinyXAttribute<char>::setName( const char* name )
{
	dStrncpy( mName, MAX_NAME_LENGTH + 1, name, MAX_NAME_LENGTH );
}

template<>
inline void CTinyXAttribute<wchar_t>::setName( const wchar_t* name )
{
	dWcsncpy( mName, MAX_NAME_LENGTH + 1, name, MAX_NAME_LENGTH );
}

template<>
inline void CTinyXAttribute<char>::setValue( const char* value )
{
	dStrncpy( mValue, MAX_VALUE_LENGTH + 1, value, MAX_VALUE_LENGTH );
}

template<>
inline void CTinyXAttribute<wchar_t>::setValue( const wchar_t* value )
{
	dWcsncpy( mValue,  MAX_VALUE_LENGTH + 1, value, MAX_VALUE_LENGTH );
}

template< typename T >
inline const T* CTinyXAttribute<T>::getName()
{
	return mName;
}

template< typename T >
inline const T* CTinyXAttribute<T>::getValue()
{
	return mValue;
}