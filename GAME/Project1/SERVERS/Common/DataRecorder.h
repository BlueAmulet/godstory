#ifndef DATA_RECORDER_H
#define DATA_RECORDER_H

#include "SimpleGC.h"

#include <fstream>
#include <iostream>
#include <string>

#define DR_MAGIC_CODE				'@'
#define DR_VERSION					1
#define DR_DEFAULT_PACKET_SIZE		1024

#define DR_FLAG_PACKET_FIXED		1

using namespace std;

class CDataRecorder
{
public:
	CDataRecorder( string strFileName );
	virtual ~CDataRecorder();

	struct stDataRecordFileHeader
	{
		char	cMagic;		// DR_MAGIC_COCE
		char	cVersion;	// DR_VERSION
		char	cFlags;		// flags;
		int		nCount;
		int		nPackSize;
		int		nUnknown;
	};

	void Open( string strFileName );
	void Close();
	void ReadData( const char* pBuffer, int* pSize );
	void WriteData( const char* pBuffer, int nSize );
	void Flush();		// used for rewrite the file header
	bool IsReady();
	static bool CreateRecordFile( string strFileName, int nPacketSize = DR_DEFAULT_PACKET_SIZE, char cFlags = 0 );
	stDataRecordFileHeader* GetHeader();
private:
	fstream fs;
	stDataRecordFileHeader m_Header;

	gc_ptr m_pDataBuffer;

	bool m_bIsReady;
};

#endif