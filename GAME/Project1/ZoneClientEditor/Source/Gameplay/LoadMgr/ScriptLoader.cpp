#include "ScriptLoader.h"
#include "Console/console.h"
#include "core/fileStream.h"
#include "core/resManager.h"

void CScriptLoader::preload( char* pText )
{
	std::stack< ScriptNode* > scriptNodeStack;
	ScriptNode* pNode = NULL;
	std::string token;

	scriptNodeStack.push( &mRootNode );

	int id = 0;
	mTotal = 0;

	while( *pText != 0 )
	{
		Utils::skipSpaces( pText );

		token = Utils::getToken( pText );
		if( token[0] == '/' && token[1] == '/' )
		{
			Utils::skipToNextLine( pText );
			continue ;
		}

		if( token == "new" )
		{
			mTotal++;

			Utils::skipSpaces( pText );
			std::string type = Utils::getToken( pText );
			token = Utils::getToken( pText );  // should be '('
			std::string name = Utils::getToken( pText );

			if( name == ")" )
			{
				name = type;
				char buf[16];
				sprintf_s( buf, 16, "%d", id++ );
				name += buf;
			}
			else
			{
				token = Utils::getToken( pText ); // should be ')'
			}

			Utils::skipToNextLine( pText );

			if( atoi( name.c_str() ) > 0 )
				name = type + "_" + name;

			pNode = new ScriptNode( type.c_str(), name.c_str() ); 

			if( !scriptNodeStack.empty() )
				scriptNodeStack.top()->getNodeList()->push_back( pNode );

			scriptNodeStack.push( pNode );
		}
		else if( token == "}" )
		{
			scriptNodeStack.pop();

			pNode = scriptNodeStack.top();

			Utils::skipToNextLine( pText );
		}
		else
		{
			std::string name = token;
			std::string nameExt;
			Utils::skipSpaces( pText );
			token = Utils::getToken( pText ); // should be '='
			if( token != "=" )
			{
				nameExt = name;
				name = token;
				Utils::skipSpaces( pText );
				Utils::getToken( pText ); // shoude be '='
			}

			Utils::skipSpaces( pText );
			std::string value = Utils::getToken( pText );

			if( pNode )
			{
				pNode->getProperyty()->operator []( nameExt + ' ' + name ) = value;
			}

			Utils::skipToNextLine( pText );
		}
	}
}

CScriptLoader::CScriptLoader()
: mRootNode( "rootType", "rootName" )
{

}

CScriptLoader::~CScriptLoader()
{

}

void CScriptLoader::loadFile( const char* fileName )
{
	std::string file = fileName;
	file = "gameres";
	file += fileName;
	file += "l";

	std::fstream temp;
	temp.open( file.c_str() );
	if( temp.is_open() )
	{
		temp.close();
		file = "~";
		file += fileName;
		file += "l";
		Con::executef( "exec", file.c_str() );
		return ;
	}

	file = "gameres";
	file += fileName;


	Stream* pStream = ResourceManager->openStream(file.c_str());
	if( !pStream )
		return ;
	//FileStream  OutStream;// = static_cast<FileStream*>(ResourceManager->openStream(file.c_str()));
	//ResourceManager->openFileForWrite( OutStream, file.c_str() );
	file = "gameres";
	file += fileName;
	file += "l";

	std::fstream fs;
	fs.open( file.c_str(), std::ios_base::out | std::ios_base::ate );

	int length = pStream->getStreamSize();

	char* buf = new char[length + 1];

	pStream->read( length, buf );

	buf[length] = 0;

	ResourceManager->closeStream( pStream );

	preload( buf );

	delete[] buf;

	mCount = 0;
	
	// here we load all the object
	for each( ScriptNode* pNode in *mRootNode.getNodeList() )
	{
		loadScriptNode( &fs, "", pNode );
	}

	fs.flush();
	fs.close();
	//ResourceManager->closeStream( &OutStream );
}

void CScriptLoader::loadScriptNode( std::fstream* fileStream, std::string object, CScriptLoader::ScriptNode* pNode )
{
	std::string name = pNode->getName();
	std::string type = pNode->getType();

	std::stringstream ss;

	ss << "new " << type << "(" << name <<") {" << std::endl;

	ScriptNode::PropertyMap::iterator it;
	for( it = pNode->getProperyty()->begin(); it != pNode->getProperyty()->end(); it++ )
	{
		ss << it->first << " = \"" << it->second << "\";" <<std::endl;
	}
	ss << "};" << std::endl;

	mCount++;

	if( mCount % 10 == 0 )
	{
		if( fileStream )
		{
			char buf[256];
			sprintf_s( buf, 256, "SetLoadingValue(%f);", float(mCount ) / mTotal );
			*fileStream << buf << std::endl;
		}
		Con::evaluatef( "SetLoadingValue(%f);", float(mCount ) / mTotal );
	}
 
	if( fileStream )
	{
		*fileStream << ss.str();
	}

	Con::evaluatef( "%s", ss.str().c_str() );

	for each( ScriptNode* pN in *pNode->getNodeList() )
	{
		loadScriptNode( fileStream, name, pN );
	}

	if( object != "" )
	{
		if( fileStream )
		{
			char buf[256];
			sprintf_s( buf, 256, "%s.add(%s);", object.c_str(), name.c_str() );
			*fileStream << buf << std::endl;
		}
		Con::evaluatef( "%s.add(%s);", object.c_str(), name.c_str() );
	}
}

CScriptLoader::ScriptNode::~ScriptNode()
{
	for each( ScriptNode* pNode in mChildren )
	{
		delete[] pNode;
	}
}

CScriptLoader::ScriptNode::ScriptNode( const char* type, const char* name, const char* nameExt )
{
	mName = name;
	mType = type;
	if( nameExt )
		mNameExt = nameExt;
}

CScriptLoader::ScriptNode::PropertyMap* CScriptLoader::ScriptNode::getProperyty()
{
	return &mPropertyMap;
}

CScriptLoader::ScriptNode::ScriptNodeList* CScriptLoader::ScriptNode::getNodeList()
{
	return &mChildren;
}

std::string CScriptLoader::ScriptNode::getName()
{
	return mName;
}

std::string CScriptLoader::ScriptNode::getType()
{
	return mType;
}

void CScriptLoader::Utils::skipSpaces( char*& pChar )
{
	while( *pChar != 0 && ( *pChar == ' ' || *pChar == '\r' || *pChar == '\n' || *pChar == '\t' ) )
		pChar++;
}

std::string CScriptLoader::Utils::getToken( char*& pChar )
{
	std::stringstream ss;

	std::deque< char > s;

	if( *pChar == '/' && *( pChar + 1 ) == '*' )
	{
		while( !( *pChar == '*' && *(pChar + 1) == '/' ) && *pChar != 0 )
			pChar++;

		if( *pChar == 0 )
			return "";

		pChar += 2;

		skipSpaces( pChar );
	}

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
	case '(':
		pChar++;
		return "(";
	case ')':
		pChar++;
		return ")";
	case '}':
		pChar++;
		return "}";
	case '{':
		pChar++;
		return "{";
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

		if( *pChar == '(' )
			break;

		if( *pChar == ')' )
			break;

		ss << *pChar++;
	}

	return ss.str();
}

void CScriptLoader::Utils::skipToNextLine( char*& pChar )
{
	while( *pChar != 0 && *pChar != '\r' && *pChar != '\n' )
		pChar++;

	while( *pChar != 0 && ( *pChar == '\r' || *pChar == '\n') )
		pChar++;
}