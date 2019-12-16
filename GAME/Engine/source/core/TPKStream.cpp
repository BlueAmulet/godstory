//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "TPKLib/TPKLib.h"
#include "core/TPKStream.h"
#include "platform/platform.h"

TPKStream::TPKStream(): 
	m_pStream(NULL),
	m_File(NULL),
	m_StreamCaps(0),
	m_EOS(false),
	m_SteamSize(0)
{
}

TPKStream::~TPKStream()
{
	detachStream();
}

bool TPKStream::attachStream(Stream* io_pSlaveStream)
{
	if(io_pSlaveStream == NULL)
		return false;
	m_pStream     = io_pSlaveStream;
	setStatus(EOS);
	m_EOS = false;
	return true;
}

void TPKStream::detachStream()
{
	m_pStream     = NULL;
	setStatus(Closed);
	m_EOS = false;
	if(m_File)
		TPKCloseFile(m_File);
	m_File = NULL;
}

bool TPKStream::SetArchive(TPKArchive* ha, const char* szFileName)
{
	if(ha == NULL || szFileName == NULL || *szFileName == 0)
		return false;
	m_EOS = false;
	if(TPKOpenFile((HANDLE)ha, szFileName, 0, (HANDLE*)&m_File))
	{
		m_StreamCaps = U32(Stream::StreamRead) | U32(Stream::StreamPosition);
		setStatus(Ok);
		m_SteamSize = m_File->pBlock->dwFSize;
		return true;
	}
	else
	{
		m_SteamSize = 0;		
		setStatus(IOError);
		return false;
	}
}

bool TPKStream::hasCapability(const Capability in_streamCap) const
{
	return(0 != (U32(in_streamCap) & m_StreamCaps));
}


Stream* TPKStream::getStream()
{
	return m_pStream;
}

U32 TPKStream::getPosition() const
{
	AssertFatal(m_pStream != NULL && m_File != NULL, "Error, stream not attached");
	if (m_pStream == NULL || m_File == NULL)
		return 0;

	return TPKSetFilePointer(m_File, 0, 0, FILE_CURRENT);
}

bool TPKStream::setPosition(const U32 in_newPosition)
{
	AssertFatal(m_pStream != NULL && m_File != NULL, "Error, stream not attached");
	if (m_pStream == NULL || m_File == NULL)
		return false;

	if(in_newPosition < m_SteamSize)
	{
		DWORD dwPos = TPKSetFilePointer(m_File, in_newPosition, 0, FILE_BEGIN);
		if(dwPos != -1)
		{
			m_EOS = (dwPos == m_SteamSize);
			return true;
		}
	}

	TPKSetFilePointer(m_File, m_SteamSize, 0, FILE_BEGIN);
	m_EOS = true;
	return false;
}

U32 TPKStream::getStreamSize()
{
	AssertFatal(m_pStream != NULL && m_File != NULL, "Error, stream not attached");
	if(m_pStream == NULL || m_File == NULL)
		return 0;
	return m_File->pBlock->dwFSize;
}

bool TPKStream::_read(const U32 in_numBytes, void* out_pBuffer)
{
	AssertFatal(m_pStream != NULL && m_File != NULL, "文件或流未打开!");

	if (in_numBytes == 0)
		return true;

	AssertFatal(out_pBuffer != NULL, "无效的输入参数!");
	if (getStatus() == Closed) 
	{
		AssertFatal(false, "尝试打开一个关闭的TPKStream流错误!");
		return false;
	}

	if (m_EOS)
	{
		setStatus(EOS);
		return false;
	}

	DWORD dwBytes, dwErr;
	BOOL success = TPKReadFile(m_File, out_pBuffer, in_numBytes, &dwBytes, NULL);
	if(!success)
	{
		dwErr = GetLastError();
		if(dwErr == ERROR_HANDLE_EOF)
		{
			success = TRUE;
			m_EOS = true;
			setStatus(Ok);
		}
		else
			setStatus(IOError);
	}
	else
		setStatus(Ok);
	return success == TRUE;
}