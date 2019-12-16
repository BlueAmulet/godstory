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
// 以下二个方法主要用于内嵌在主执行程序的资源里的压缩文件，进行压缩包文件管理
extern Zip::ZipArchive *openEmbeddedVFSArchive();
extern void closeEmbeddedVFSArchive();

#endif // _PLATFORMVFS_H_
