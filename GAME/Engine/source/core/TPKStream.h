//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef __TPKSTREAM_H__
#define __TPKSTREAM_H__

#ifndef _FILEIO_H_
#include "core/fileio.h"
#endif
#ifndef _STREAM_H_
#include "core/filterStream.h"
#endif

struct TPKFile;
struct TPKArchive;

class TPKStream : public FilterStream
{
	typedef FilterStream Parent;
public:
	TPKStream();
	~TPKStream();

	bool	SetArchive(TPKArchive* ha, const char* szFileName);
	bool    attachStream(Stream* io_pSlaveStream);
	void    detachStream();
	Stream* getStream();

protected:
	bool _read(const U32 in_numBytes,  void* out_pBuffer);
public:
	U32  getPosition() const;
	bool setPosition(const U32 in_newPosition);
	bool hasCapability(const Capability) const;

	U32  getStreamSize();
private:
	Stream*		m_pStream;
	TPKFile*	m_File;
	U32			m_StreamCaps;
	bool		m_EOS;
	U32			m_SteamSize;
};

#endif // __TPKSTREAM_H__
