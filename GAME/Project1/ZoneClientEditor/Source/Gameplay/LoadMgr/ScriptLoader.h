#pragma once

#include <string>
#include <hash_map>
#include <stack>
#include <sstream>
#include <fstream>
#include <exception>

class FileStream;

class CScriptLoader
{
	class ScriptNode
	{
	public:
		typedef stdext::hash_map< std::string, std::string > PropertyMap;
		typedef std::list< ScriptNode* > ScriptNodeList;

		ScriptNode( const char* type, const char* name, const char* nameExt = NULL );
		ScriptNode* newNode();
		virtual ~ScriptNode();
	public:
		PropertyMap* getProperyty();
		ScriptNodeList* getNodeList();

		std::string getName();
		std::string getType();
	private:
		std::string mName;
		ScriptNodeList mChildren;
		PropertyMap mPropertyMap;
		std::string mType;
		std::string mNameExt;
	};

	struct Utils
	{
		static void skipSpaces( char*& pChar );
		static void skipToNextLine( char*& pChar );
		static std::string getToken( char*& pChar );
	};

public:
	CScriptLoader();
	virtual ~CScriptLoader();

	void saveFile();
	void readFile();

	void preload( char* pText );
	void loadFile( const char* fileName );
	void loadScriptNode( std::fstream* pStream, std::string object, ScriptNode* pNode );

private:
	ScriptNode mRootNode;
	int mTotal;
	int mCount;
};