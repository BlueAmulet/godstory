#include "Config.h"



void CONF::Utils::skipSpaces( char*& pChar )
{
	while( *pChar != 0 && ( *pChar == ' ' || *pChar == '\r' || *pChar == '\n' || *pChar == '\t' ) )
		pChar++;
}

std::string CONF::Utils::getToken( char*& pChar )
{
	std::stringstream ss;

	std::deque< char > s;

	if( *pChar == '\"' || *pChar == '"' )
		s.push_back( *pChar++ );

	switch( *pChar )
	{
	case '=':
		pChar++;
		return "=";
	case ':':
		pChar++;
		return ":";
	}

	while( *pChar != 0 && *pChar != '\r' && *pChar != '\n' )
	{
		if( *pChar == '\"' || *pChar == '"' )
		{
			if( *pChar == *(s.rbegin()) )
			{
				s.pop_back();

				pChar++;

				if( s.size() == 0 )
					break;
			}
			else
			{
				s.push_back( *pChar++ );
			}		
		}

		if( *pChar == ' ' && s.size() == 0 )
			break;

		ss << *pChar++;
	}

	return ss.str();
}

std::string CONF::Utils::getNamespace( Types::NamespaceStack& namespaceStack )
{
	Types::NamespaceStack::iterator it;

	if( namespaceStack.size() == 0 )
		return "";

	std::stringstream ss;

	it = namespaceStack.begin();

	ss << *(it++);

	for( ; it != namespaceStack.end(); it++ )
		ss << "." << *it;

	return ss.str();
}

void CONF::Utils::skipToNextLine( char*& pChar )
{
	while( *pChar != 0 && *pChar != '\r' && *pChar != '\n' )
		pChar++;

	while( *pChar != 0 && ( *pChar == '\r' || *pChar == '\n') )
		pChar++;
}

std::string CONF::Utils::getParentNamespace( Types::NamespaceStack& namespaceStack )
{

	Types::NamespaceStack::iterator it;

	if( namespaceStack.size() == 1 )
		return "";

	std::stringstream ss;

	it = namespaceStack.begin();

	int i = 0;
	int size = namespaceStack.size();

	ss << *(it++);

	for( ; i < size - 1; it++, i++ )
		ss << "." << *it;

	return ss.str();
}

void CONF::File::LoadFile( std::string fileName )
{
	std::fstream fs;
	fs.open( fileName.c_str() );

	fs.seekg( 0, std::fstream::end );
	int length = fs.tellg();

	fs.seekg( 0, std::fstream::beg );

	char* buf = new char[length + 1];

	fs.read( buf, length );

	buf[length] = 0;

	Types::NamespaceStack namespaceStack;


	char* p = buf;
	std::string token;
	std::string ns;
	std::string nsCur;
	std::string value;
	Namespace* parentNs = NULL;

	while( *p != 0 )
	{
		Utils::skipSpaces( p );

		token = Utils::getToken( p );

		if( token[0] == '/' && token[1] == '/' )
		{
			Utils::skipToNextLine( p );
			continue;
		}

		if( token == "}" )
		{
			if( parentNs )
			{
				std::string sns = Utils::getNamespace( namespaceStack );
				Namespace& n = mNamespaceMap[ sns ];

				Namespace::ValueMap::iterator it;

				for( it = parentNs->valueMap.begin(); it != parentNs->valueMap.end(); it++ )
				{
					n.valueMap[ it->first ] = it->second;
					mKeyValueMap[ sns + "." + it->first ] = it->second;
				}


				parentNs = NULL;
			}

			namespaceStack.pop_back();

			continue;
		}

		ns = token;

		Utils::skipSpaces( p );

		token = Utils::getToken( p );

		if( token == "=" )
		{
			Utils::skipSpaces( p );

			nsCur = Utils::getNamespace( namespaceStack ) + "." + ns;

			value = Utils::getToken( p );

			mKeyValueMap[ nsCur ] = value;
			mNamespaceMap[ Utils::getNamespace( namespaceStack ) ].valueMap[ns] = value;
		}
		else if( token == "{" )
		{
			namespaceStack.push_back( ns );
		}
		else if( token == ":" )
		{
			Utils::skipSpaces( p );

			token = Utils::getToken( p );

			parentNs = &mNamespaceMap[ Utils::getNamespace( namespaceStack ) + "." + token ];

			Utils::skipSpaces( p );

			token = Utils::getToken( p );

			if( token == "{" )
			{
				namespaceStack.push_back( ns );		
			}
			else
			{
				// unexcepted char
				break;
			}
		}
		else
		{
			// unexcepted char
			break;
		}

	}


	delete[] buf;

	fs.close();	
}

CONF::File::File()
{

}