#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <sstream>
#include <hash_map>
#include <fstream>
#include <deque>

namespace CONF
{
	struct Types
	{
		typedef std::deque< std::string > NamespaceStack;
	};

	struct Utils
	{
		static void skipSpaces( char*& pChar );
		static void skipToNextLine( char*& pChar );
		static std::string getToken( char*& pChar );
		static std::string getNamespace( Types::NamespaceStack& namespaceStack );
		static std::string getParentNamespace( Types::NamespaceStack& namespaceStack );
	};

	class Value
	{
	public:
		Value()
		{
			;
		}

		Value( const Value& v )
		{
			mValue = v.mValue;
		}

		void operator = ( const Value& v )
		{
			mValue = v.mValue;
		}

		template< typename _Ty >
		operator _Ty()
		{
			std::stringstream ss;
			ss << mValue;

			_Ty result;
			ss >> result;

			return result;	
		}

		operator std::string()
		{
			return mValue;
		}

		operator const char* ()
		{
			return mValue.c_str();
		}

		template< typename _Ty >
		void operator = ( const _Ty& value )
		{
			std::stringstream ss;

			ss << value;

			mValue = ss.str();
		}

	private:
		std::string mValue;
	};

	struct Namespace
	{
		typedef stdext::hash_map< std::string, Value > ValueMap;

		ValueMap valueMap;
	};

	class File
	{
		typedef stdext::hash_map< std::string, Value > KeyValueMap;
		typedef stdext::hash_map< std::string, Namespace > NamespaceMap;

	public:
		File();

		void LoadFile( std::string fileName );

		template< typename _Ty >
		_Ty getValue( std::string keyName )
		{
			return mKeyValueMap[keyName];
		}

	private:
		KeyValueMap mKeyValueMap;
		NamespaceMap mNamespaceMap;
	};
}

#endif /*CONFIG_H*/