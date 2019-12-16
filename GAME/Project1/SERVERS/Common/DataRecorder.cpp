#include "DataRecorder.h"

CDataRecorder::CDataRecorder( string strFileName )
{
	m_bIsReady = false;
	Open( strFileName );
}

CDataRecorder::~CDataRecorder()
{
	Close();
}

void CDataRecorder::Close()
{
	if( m_bIsReady )
	{
		Flush();

		fs.close();
	}
}

void CDataRecorder::Open( string strFileName )
{
	Close();

	m_bIsReady = false;
	memset( &m_Header, 0, sizeof( stDataRecordFileHeader ) );
	fs.open( strFileName.c_str() );

	if( !fs.is_open() )
		return ;

	fs.seekg( 0, ios::beg );

	fs.read( (char*)&m_Header, sizeof( stDataRecordFileHeader ) );

	if( m_Header.cMagic != DR_MAGIC_CODE )
		return ;

	m_pDataBuffer = new char[m_Header.nPackSize];

	// 将写指针定位到文件最后
	fs.seekp( 0, ios::end );

	m_bIsReady = true;
}

CDataRecorder::stDataRecordFileHeader* CDataRecorder::GetHeader()
{
	return &m_Header;
}

bool CDataRecorder::CreateRecordFile( string strFileName, int nPacketSize, char cFlags )
{
	stDataRecordFileHeader header;

	header.cMagic = DR_MAGIC_CODE;
	header.cVersion = DR_VERSION;
	header.nCount = 0;
	header.nUnknown = 0;
	header.nPackSize = nPacketSize;
	header.cFlags = cFlags;

	fstream fs;
	fs.open( strFileName.c_str(), ios::ate | ios::out );
	if( !fs.is_open() )
		return false;
	fs.seekp( 0, ios::beg );
	fs.write( (char*)&header, sizeof( stDataRecordFileHeader ) );
	fs.flush();
	fs.close();

	return true;
}

void CDataRecorder::ReadData( const char *pBuffer, int* pSize )
{
	if( !pBuffer || !m_bIsReady )
		return ;

	unsigned short wSize;
	fs.read( (char*)&wSize, 2 );

	fs.read( m_pDataBuffer, wSize );
	memcpy( (void*)pBuffer, m_pDataBuffer, wSize );

	if( pSize )
		*pSize = wSize;

	if( m_Header.cFlags & DR_FLAG_PACKET_FIXED )
		fs.seekg( m_Header.nPackSize - wSize, ios::cur );
}

void CDataRecorder::Flush()
{
	if( !m_bIsReady )
		return ;

	fs.seekp( 0, ios::beg );
	fs.write( (char*)&m_Header, sizeof( stDataRecordFileHeader ) );

	fs.flush();

	// 重置写指针
	fs.seekp( 0, ios::end );
}

void CDataRecorder::WriteData( const char* pBuffer, int nSize )
{
	if( !m_bIsReady ) 
		return ;

	if( nSize > m_Header.nPackSize )
		nSize = m_Header.nPackSize;

	memset( m_pDataBuffer, 0, m_Header.nPackSize );
	memcpy( m_pDataBuffer, pBuffer, nSize );

	unsigned short wSize;
	wSize = (unsigned short)nSize;

	fs.write( (char*)&wSize, 2 );

	if( m_Header.cFlags & DR_FLAG_PACKET_FIXED )
        fs.write( m_pDataBuffer, m_Header.nPackSize );
	else
		fs.write( m_pDataBuffer, wSize );

	m_Header.nCount++;
}

bool CDataRecorder::IsReady()
{
	return m_bIsReady;
}