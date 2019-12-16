//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _PLATFORMVFS_H_
#define _PLATFORMVFS_H_

namespace Zip
{
   class ZipArchive;
}
// ���¶���������Ҫ������Ƕ����ִ�г������Դ���ѹ���ļ�������ѹ�����ļ�����
extern Zip::ZipArchive *openEmbeddedVFSArchive();
extern void closeEmbeddedVFSArchive();

#endif // _PLATFORMVFS_H_
