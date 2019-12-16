//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "base/Locker.h"
#include "Util/TinyGC/TinyGC.h"
#include "platform/platform.h"
#include "console/console.h"
#include "core/tVector.h"
#include "core/stream.h"

#include "core/fileStream.h"
#include "core/resizeStream.h"
#include "core/frameAllocator.h"
#include "core/TPKStream.h"

#include "core/zip/zipArchive.h"

#include "core/resManager.h"
#include "core/findMatch.h"

#include "console/console.h"
#include "console/consoleTypes.h"

#include "util/safeDelete.h"
#include "util/tempAlloc.h"

#include "tpklib/TPKLib.h"
#include "Common/BackWorker.h"

#include "util/BackgroundLoadMgr.h"




ResManager *ResourceManager = NULL;

char *ResManager::smExcludedDirectories = ".svn;CVS;Launch";

//-----------------------------------------------------------------------------
// 截去文件扩展名（只对validExtension有效扩展名进行匹配）
StringTableEntry stripExtension(const char* fileName, U32 numValidExt, const char* validExtension[])
{
	static char buf[4096];
	AssertISV(dStrlen(fileName) < 4096, "文件名太长(stripExtension)!");
	S32 len = dStrlen(fileName);
	const char* ext = dStrrchr(fileName, '.');
	if(ext && numValidExt)
	{
		bool validExt = false;
		for( U32 i = 0; i < numValidExt; i++ )
		{
			if (dStricmp(ext, validExtension[i]) == 0)
			{
				validExt = true;
				break;
			}
		}

		if (!validExt)
			ext = NULL;
	}

	if (ext)
		len = ext - fileName;

	dMemcpy(buf, fileName, len);
	buf[len] = 0;

	return StringTable->insert(buf);
}

//-----------------------------------------------------------------------------
// 拼接文件名与文件扩展名
StringTableEntry addExtension(const char* fileName, const char* extension)
{
	S32 filelen = dStrlen(fileName);
	S32 extlen = dStrlen(extension);
	TempAlloc<char> buf(filelen + extlen + 1);
	AssertISV(filelen + extlen < 4096, "文件名太长(stripExtension)!");
	dStrcpy(buf, filelen + extlen + 1, fileName);
	dStrcpy(buf + filelen, extlen + 1, extension);
	buf[filelen + extlen] = 0;
	return StringTable->insert(buf);
}

//===============================================================================
//	资源文件链表节点类（ResourceObject）相关方法
ResourceObject::ResourceObject ()
{
	next = NULL;
	prev = NULL;
	lockCount = 0;
	mInstance = NULL;
#ifdef USE_TPK
	mTPKArchive = NULL;
	BlockIndex = 0;
#else
	mZipArchive = NULL;
	mCentralDir = NULL;
#endif	
}

void ResourceObject::unlink()
{
	if (next)
		next->prev = prev;
	if (prev)
		prev->next = next;
	next = prev = 0;
}

void ResourceObject::linkAfter(ResourceObject* res)
{
	unlink();
	prev = res;
	if ((next = res->next) != 0)
		next->prev = this;
	res->next = this;
}

void ResourceObject::destruct ()
{
	SAFE_DELETE(mInstance);

	// 仅释放非文件包内的文件的压缩包对象
	if((flags & ResourceObject::File) && !(flags & ResourceObject::VolumeBlock) )
	{
		// 因为如果文件为zip压缩包内的文件，可能在zip压缩包对象释放时，这个对象也
		// 无效了，所以这里只处理本地文件的情况
#ifdef USE_TPK
		if(mTPKArchive)
			TPKCloseArchive(mTPKArchive);
		mTPKArchive = NULL;
#else
		SAFE_DELETE(mZipArchive);
#endif
	}
}

//-----------------------------------------------------------------------------
// 获取完整路径文件名
const char* ResourceObject::getFullPath()
{
	char buffer[1024];
	dSprintf (buffer, sizeof (buffer), "%s/%s", path, name);
	return StringTable->insert(buffer);
}

//=============================================================================
//	资源文件管理类（ResManager）相关方法

ResManager::ResManager ()
{
	echoFileNames = false;
	primaryPath[0] = 0;
	writeablePath[0] = 0;
	pathList = NULL;
	resourceList.nextResource = NULL;
	resourceList.next = NULL;
	resourceList.prev = NULL;
	timeoutList.nextResource = NULL;
	timeoutList.next = NULL;
	timeoutList.prev = NULL;
	registeredList = NULL;
	mLoggingMissingFiles = false;
	usingVFS = false;

	m_pWorker = new CBackWorker( 5 );
}

ResManager::~ResManager ()
{
	purge ();

	if (pathList)
		dFree (pathList);

	for (ResourceObject * walk = resourceList.nextResource; walk;
		walk = walk->nextResource)
		walk->destruct ();

	while (resourceList.nextResource)
		freeResource (resourceList.nextResource);

	while (registeredList)
	{
		RegisteredExtension *temp = registeredList->next;
		delete registeredList;
		registeredList = temp;
	}

	delete m_pWorker;
}

//------------------------------------------------------------------------------
// 创建资源文件管理对象
void ResManager::create ()
{
	AssertFatal (ResourceManager == NULL, "资源文件管理对象已经存在!");
	ResourceManager = new ResManager;
	// 关联脚本变量，提供排除目录的设置途径
	Con::addVariable("Pref::ResourceManager::excludedDirectories", TypeString, &smExcludedDirectories);
}


//------------------------------------------------------------------------------
// 释放资源文件管理对象
void ResManager::destroy ()
{
	AssertFatal (ResourceManager != NULL, "资源文件管理对象不存在!");
	if(ResourceManager)
		delete ResourceManager;
	ResourceManager = NULL;
}

//------------------------------------------------------------------------------
// 创建有效路径文件名
static const char* buildPath (StringTableEntry path, StringTableEntry file)
{
	static char buf[1024];
	if (path)
		Platform::makeFullPathName(file, buf, sizeof(buf), path);
	else
		dStrcpy (buf, sizeof(buf), file);
	return buf;
}

//------------------------------------------------------------------------------
// 分解长路径名为路径名和文件名二部分
void ResManager::getPaths (const char *fullPath, StringTableEntry & path, StringTableEntry & fileName)
{
	static char buf[1024];
	char *ptr = (char *) dStrrchr (fullPath, '/');
	if (!ptr)
	{
		path = NULL;
		fileName = StringTable->insert (fullPath);
	}
	else
	{
		S32 len = ptr - fullPath;
		dMemcpy(buf, fullPath, len);
		buf[len] = 0;
		fileName = StringTable->insert (ptr + 1);
		path = StringTable->insert (buf);
	}
}

#ifdef USE_TPK
//------------------------------------------------------------------------------
// 扫描TPK包所有文件到文件映射表中
bool ResManager::scanTPK (ResourceObject* tpkObject)
{
	S32 length = 0;
	const char* PackPath = buildPath(tpkObject->zipPath, tpkObject->zipName);
	if(tpkObject->mTPKArchive == NULL)
	{
		if(!TPKOpenArchive(PackPath, OPEN_EXISTING, (HANDLE*)&tpkObject->mTPKArchive))
		{
			Con::printf("TPK文件包[ %s ]无法打开!(scanTPK方法)", PackPath);
			return false;
		}
	}

	TPKArchive*	pArchive	= tpkObject->mTPKArchive;
	TPKHash*	pHashBegin  = pArchive->pHashTable;
	TPKHash*	pHash		= pHashBegin;
	TFileNode*	pNode;

	for(DWORD i = 0; i < pArchive->pHeader->dwHashTableSize; i++)
	{
		pNode = pArchive->pListFile[i];

		// 检查Block索引值和文件列表节点
		if(pHash->dwBlockIndex < DATA_ENTRY_DELETED && (DWORD_PTR)pNode < DATA_ENTRY_DELETED)
		{
			if(_stricmp(pNode->szFileName, TPK_LISTFILE_NAME) == 0)
			{
				pHash++;
				continue;
			}

			char buf[1024];
			dStrncpy(buf, sizeof(buf), pNode->szFileName, sizeof(buf));
			buf[sizeof(buf)-1] = 0;
			char* scan = buf;
			// 替换路径中\\为/
			while (*scan != '\0')
			{
				if (*scan == '\\')
					*scan = '/';
				scan++;
			}
			
			char* szTPKFileName = pArchive->szFileName;
			length = dStrlen(szTPKFileName) + strlen(buf) + 2;
			FrameTemp<char> packPath(length);
			dStrcpy(packPath, length, szTPKFileName);

			char* dot = dStrrchr(packPath, '.');
			if(dot)
			{
				dot -= 2;
				dot[2] = '\0';
				dStrcat(packPath, length, "/");
			}

			dStrcat(packPath, length, buf);

			char* pPathEnd = dStrrchr(packPath, '/');
			if(pPathEnd == NULL)
				continue;
			
			pPathEnd[0] = '\0';
			const char* path = StringTable->insert(packPath);
			const char* file = StringTable->insert(pPathEnd + 1);

			TPKBlock* pBlock = pArchive->pBlockTable + pHash->dwBlockIndex;

			ResourceObject *ro = createZipResource(path, file, tpkObject->zipPath, tpkObject->zipName);
			ro->flags = ResourceObject::VolumeBlock;
			ro->fileSize = pBlock->dwFSize;
			ro->compressedFileSize = pBlock->dwCSize;
			ro->fileOffset = pBlock->dwFilePos;
			ro->BlockIndex = pHash->dwBlockIndex;
			ro->mTPKArchive = tpkObject->mTPKArchive;
			dictionary.pushBehind (ro, ResourceObject::File);
		}
		pHash ++;
	}
	return true;
}
#else

//------------------------------------------------------------------------------
// 添加ZIP包根路径
bool ResManager::addVFSRoot(Zip::ZipArchive *vfs)
{
	ResourceObject *ro = createResource (StringTable->insert(""), StringTable->insert(""));
	dictionary.pushBehind (ro, ResourceObject::File);

	// [tom, 10/28/2006] Using VolumeBlock here so that destruct() doesnt try and
	// delete the archive.
	ro->flags = ResourceObject::VolumeBlock;
	ro->fileOffset = 0;
	ro->fileSize = 0;
	ro->compressedFileSize = 0;
	ro->mZipArchive = vfs;
	ro->zipPath = StringTable->insert("");
	ro->zipName = StringTable->insert("");

	usingVFS = true;
	return scanZip(ro);
}
//------------------------------------------------------------------------------
// 扫描ZIP包所有文件到文件映射表中
bool ResManager::scanZip (ResourceObject* zipObject)
{
	S32 length = 0;
	const char *zipPath = buildPath(zipObject->zipPath, zipObject->zipName);
	if(zipObject->mZipArchive == NULL)
	{
		zipObject->mZipArchive = new Zip::ZipArchive;
		if(! zipObject->mZipArchive->openArchive(zipPath))
		{
			SAFE_DELETE(zipObject->mZipArchive);
			return false;
		}
	}

	for(S32 i = 0;i < zipObject->mZipArchive->numEntries();++i)
	{
		const Zip::CentralDir &dir = (*zipObject->mZipArchive)[i];

		// FIXME [tom, 10/26/2006] This is pretty lame
		char buf[1024];
		dStrcpy(buf, sizeof(buf), dir.mFilename, sizeof(buf));
		buf[sizeof(buf)-1] = 0;

		// 替换路径中\\为/
		char* scan = buf;
		while (*scan != '\0')
		{
			if (*scan == '\\')
				*scan = '/';
			scan++;
		}

		const char *zipFN = zipObject->mZipArchive->getFilename() ? zipObject->mZipArchive->getFilename() : "";

		length = dStrlen(zipFN) + dStrlen(buf) + 2;
		FrameTemp<char> zipPath(length);
		dStrcpy(zipPath, length, zipFN);

		char* dot = dStrrchr(zipPath, '.');
		if(dot)
		{
			dot -= 2;
			dot[2] = '\0';
			dStrcat(zipPath, length, "/");
		}

		dStrcat(zipPath, length, buf);

		char* pPathEnd = dStrrchr(zipPath, '/');
		if(pPathEnd == NULL)
			continue;

		pPathEnd[0] = '\0';
		const char* path = StringTable->insert(zipPath);
		const char* file = StringTable->insert(pPathEnd + 1);

		ResourceObject *ro = createZipResource(path, file, zipObject->zipPath, zipObject->zipName);

		ro->flags = ResourceObject::VolumeBlock;
		ro->fileSize = dir.mUncompressedSize;
		ro->compressedFileSize = dir.mCompressedSize;
		ro->fileOffset = dir.mLocalHeadOffset;
		ro->mZipArchive = zipObject->mZipArchive;
		ro->mCentralDir = &dir;

		dictionary.pushBehind (ro, ResourceObject::File);
	}

	return true;
}

#endif

//------------------------------------------------------------------------------
// 搜索目录，添加文件到映射表
void ResManager::searchPath (const char *path, bool noDups /* = false */, bool ignoreZips /* = false */ )
{
	AssertFatal (path && *path, "搜索路径参数错误");
	if(path == NULL || *path == 0)
		return;

	// 设置排除目录
	initExcludedDirectories();

	Vector < Platform::FileInfo > fileInfoVec;
	Platform::dumpPath (path, fileInfoVec);

	for (U32 i = 0; i < fileInfoVec.size (); i++)
	{
		Platform::FileInfo & rInfo = fileInfoVec[i];
		if(noDups && dictionary.find(rInfo.pFullPath, rInfo.pFileName) != NULL)
			continue;

		ResourceObject *ro = createResource (rInfo.pFullPath, rInfo.pFileName);
		dictionary.pushBehind (ro, ResourceObject::File);

		ro->flags = ResourceObject::File;
		ro->fileSize = rInfo.fileSize;
		ro->fileOffset = 0;
		ro->compressedFileSize = rInfo.fileSize;

		const char *extension = dStrrchr (ro->name, '.');
#ifdef USE_TPK
		// 判断是否tpk文件包
		if (extension && !dStricmp (extension, ".tpk") && !ignoreZips )
		{
			ro->zipPath = rInfo.pFullPath;
			ro->zipName = rInfo.pFileName;
			scanTPK(ro);
		}
#else
		// 判断是否zip文件包
		if (extension && !dStricmp (extension, ".zip") && !ignoreZips )
		{
			ro->zipName = rInfo.pFileName;
			ro->zipPath = rInfo.pFullPath;
			scanZip(ro);
		}
#endif
	}

	// 清除排除列表目录
	Platform::clearExcludedDirectories();
}


//------------------------------------------------------------------------------
// 根据路径去搜索匹配相应的文件包，如果找到，则登记文件包内所有文件到文件映射表
bool ResManager::setModZip(const char* path)
{
#ifdef USE_TPK
	const char* ext =  ".tpk";
#else
	const char* ext =  ".zip";
#endif
	S32 length = dStrlen(path) + dStrlen(ext) + 1;
	TempAlloc<char> modPath(length);
	dStrcpy(modPath, length, path);
	dStrcat(modPath, length, ext);

	// 这里非常低效，但不得不遍历目录匹配modePath
	const char *basePath = Con::isCurrentScriptToolScript() ? Platform::getMainDotCsDir() : Platform::getCurrentDirectory();
	Vector < Platform::FileInfo > pathInfo;
	Platform::dumpPath (basePath, pathInfo);
	for(U32 i = 0; i < pathInfo.size(); i++)
	{
		Platform::FileInfo &file = pathInfo[i];

		if(!dStricmp(file.pFileName, modPath))
		{
			// 为文件包文件，创建资源对象
			ResourceObject *zip = createResource(basePath, file.pFileName);
			dictionary.pushBehind(zip, ResourceObject::File);
			zip->flags = ResourceObject::File;
			zip->fileOffset = 0;
			zip->fileSize = file.fileSize;
			zip->compressedFileSize = file.fileSize;
			zip->zipName = file.pFileName;
			zip->zipPath = basePath;

			// 遍历文件包所有文件，创建资源对象到文件映射表
#ifdef USE_TPK
			bool ret = scanTPK(zip);
#else
			bool ret = scanZip(zip);
#endif
			return ret;
		}
	}
	return false;
}

//------------------------------------------------------------------------------
// 解析分号分隔的字符串，用于添加排除目录列表
void ResManager::initExcludedDirectories()
{
	Platform::clearExcludedDirectories();

	char *working = dStrdup(smExcludedDirectories);
	char * context = NULL;
	char* temp = dStrtok( working, ";", &context );
	while ( temp )
	{
		Platform::addExcludedDirectory(temp);
		temp = dStrtok( NULL, ";", &context );
	}
	dFree(working);
}

void ResManager::setModPaths (U32 numPaths, const char **paths)
{
	// [tom, 10/28/2006] If we're using a VFS, we don't want to do this
	// since it'll remove all the stuff we've already added.
	if(usingVFS)
		return;
	// detach all the files.
	for(ResourceObject * pwalk = resourceList.nextResource; pwalk; pwalk = pwalk->nextResource)
		pwalk->flags |= ResourceObject::Added;

	U32 pathLen = 0;

	// 设置排除目录
	initExcludedDirectories();

	Vector<const char*> validPaths;

	for (U32 i = 0; i < numPaths; i++)
	{
#ifndef POWER_TOOLS
		// 判断路径是否为当前目录下的子目录或排除的目录，如果是，则从zip包里查找
		if (!Platform::isSubDirectory (Platform::getCurrentDirectory(), paths[i]) || Platform::isExcludedDirectory(paths[i]))
		{
			if (!setModZip(paths[i]))
			{
				Con::errorf ("setModPaths: invalid mod path directory name: '%s'", paths[i]);
				continue;
			}
		}
#endif
		pathLen += (dStrlen (paths[i]) + 1);
		setModZip(paths[i]);
		searchPath (paths[i]);
		validPaths.push_back(paths[i]);
	}

	Platform::clearExcludedDirectories();

	if (!pathLen)
		return;

	// 创建内部游戏目录列表字符串
	pathList = (char *) dRealloc (pathList, pathLen);
	dStrcpy (pathList, pathLen, validPaths[0]);
	U32 strlen;
	for (U32 i = 1; i < validPaths.size(); i++)
	{
		strlen = dStrlen (pathList);
		dSprintf (pathList + strlen, pathLen - strlen, ";%s", validPaths[i]);
	}

	// 解除所有标志为"Added"的资源对象
	ResourceObject *rwalk = resourceList.nextResource, *rtemp;
	while (rwalk != NULL)
	{
		if ((rwalk->flags & ResourceObject::Added) && !rwalk->mInstance)
		{
			rwalk->unlink ();
			dictionary.remove (rwalk);
			rtemp = rwalk->nextResource;
			freeResource (rwalk);
			rwalk = rtemp;
		}
		else
			rwalk = rwalk->nextResource;
	}
}

const char * ResManager::getModPaths ()
{
	return ((const char *) pathList);
}

//------------------------------------------------------------------------------
#ifndef POWER_TOOLS
// ModPath 不用于Editor模式下，参考addResPath/removeResPath控制台函数
ConsoleFunction( setModPaths, void, 2, 2, "为管理资源设置有效目录，字符串以分号相隔")
{
	char buf[512];
	dStrncpy(buf, sizeof(buf), argv[1], sizeof(buf) - 1);
	buf[511] = '\0';

	Vector<char *> paths;
	char * context = NULL;
	char* temp = dStrtok( buf, ";", &context );
	while ( temp )
	{
		if ( temp[0] )
			paths.push_back(temp);

		temp = dStrtok( NULL, ";", &context );
	}

	ResourceManager->setModPaths( paths.size(), (const char**) paths.address() );
}

ConsoleFunction( getModPaths, const char*, 1, 1, "Return the mod paths the resource manager is using.")
{
	return( ResourceManager->getModPaths() );
}

#endif
//------------------------------------------------------------------------------
// 从文件映射表中查找对应文件信息，获取文件的大小
S32 ResManager::getSize (const char *fileName)
{
	ResourceObject * ro = find (fileName);
	if (!ro)
		return 0;
	else
		return ro->fileSize;
}

//------------------------------------------------------------------------------

const char* ResManager::getFullPath (const char *fileName, char *path, U32 pathlen)
{
	AssertFatal (fileName, "ResourceManager::getFullPath: fileName is NULL");
	AssertFatal (path, "ResourceManager::getFullPath: path is NULL");
	ResourceObject *obj = find (fileName);
	if (!obj)
		dStrcpy (path, pathlen, fileName);
	else
		Platform::makeFullPathName(obj->name, path, pathlen, obj->path);
	return path;
}

//------------------------------------------------------------------------------

const char* ResManager::getPathOf (const char *fileName)
{
	AssertFatal (fileName, "ResourceManager::getPathOf: fileName is NULL");
	ResourceObject *obj = find (fileName);
	if (!obj)
		return NULL;
	else
		return obj->path;
}

//------------------------------------------------------------------------------

const char* ResManager::getModPathOf (const char *fileName)
{
	AssertFatal (fileName, "ResourceManager::getModPathOf: fileName is NULL");

	if (!pathList)
		return NULL;

	ResourceObject *obj = find (fileName);
	if (!obj)
		return NULL;

	char buffer[256];
	char *base;
	const char *list = pathList;
	do
	{
		base = buffer;
		*base = 0;
		while (*list && *list != ';')
		{
			*base++ = *list++;
		}
		if (*list == ';')
			++list;

		*base = 0;

		if (dStrncmp (buffer, obj->path, (base - buffer)) == 0)
			return StringTable->insert (buffer);
	}
	while (*list);

	return NULL;
}

//------------------------------------------------------------------------------

void ResManager::registerExtension (const char *name, RESOURCE_CREATE_FN create_fn)
{
	AssertFatal (!getCreateFunction (name),
		"ResourceManager::registerExtension: file extension already registered.");

	const char *extension = dStrrchr (name, '.');
	AssertFatal (extension,
		"ResourceManager::registerExtension: file has no extension.");

	RegisteredExtension *add = new RegisteredExtension;
	add->mExtension = StringTable->insert (extension);
	add->mCreateFn = create_fn;
	add->next = registeredList;
	registeredList = add;
}

//------------------------------------------------------------------------------

RESOURCE_CREATE_FN ResManager::getCreateFunction (const char *name)
{
	const char * s = dStrrchr (name, '.');
	if (!s)
		return (NULL);

	RegisteredExtension * itr = registeredList;
	while (itr)
	{
		if (dStricmp (s, itr->mExtension) == 0)
			return (itr->mCreateFn);
		itr = itr->next;
	}
	return (NULL);
}


//------------------------------------------------------------------------------

void ResManager::unlock (ResourceObject * obj)
{
	if (!obj)
		return;
	AssertFatal (obj->lockCount > 0,"资源文件引用计数已经为0[ResourceManager::unlock]");

	// 如果引用计数到0,则将当前对象放到资源文件的超时链表里
	if (--obj->lockCount == 0)
		obj->linkAfter (&timeoutList);
}

//------------------------------------------------------------------------------

ResourceObject *ResManager::_load (const char *fileName, bool computeCRC)
{
	ResourceObject *obj = find (fileName);

	if (!obj)
	{
		delete []fileName;
		return NULL;
	}

	if (!obj->lockCount && computeCRC && obj->mInstance)
		obj->destruct ();
    
	obj->lockCount++;
	obj->unlink ();

    if (!obj->mInstance)
    {
        obj->mInstance = loadInstance (obj, computeCRC);

        if (!obj->mInstance)
        {
            obj->lockCount--;
            delete []fileName;
            return NULL;
        }
    }

	delete []fileName;
	return obj;
}

ResourceObject *ResManager::load (const char *fileName, bool computeCRC)
{
    BACKGROUND_LOAD_LOCK

	S32 length = dStrlen(fileName) + 1;
	char* str = new char[length];
	dStrcpy( str, length, fileName );
	return _load( str, computeCRC);
}

//------------------------------------------------------------------------------

ResourceInstance* ResManager::loadInstance (const char* fileName, bool computeCRC, bool noMissing)
{
    BACKGROUND_LOAD_LOCK

	ResourceObject *obj = find (fileName, noMissing);
	if (!obj)
		return NULL;

	return loadInstance (obj, computeCRC);
}

//------------------------------------------------------------------------------

static const char *alwaysCRCList = ".ter.dif.mod.atlas";

ResourceInstance * ResManager::loadInstance (ResourceObject* obj, bool computeCRC)
{
    BACKGROUND_LOAD_LOCK

	Stream *stream = openStream (obj);

	if (!stream)
		return NULL;

	if (!computeCRC)
	{
		const char *x = dStrrchr (obj->name, '.');
		if (x && dStrstr (alwaysCRCList, x))
			computeCRC = true;
	}

	if (computeCRC)
		obj->crc = calculateCRCStream (stream, InvalidCRC);
	else
		obj->crc = InvalidCRC;

	RESOURCE_CREATE_FN createFunction = ResourceManager->getCreateFunction (obj->name);

	if(!createFunction)
	{
		AssertWarn( false, "ResourceObject::construct: NULL resource create function.");
		Con::errorf("ResourceObject::construct: NULL resource create function for '%s'.", obj->name);
		return NULL;
	}

	ResourceInstance *ret = createFunction (*stream, obj);
	if(ret)
		ret->mSourceResource = obj;
	closeStream (stream);
	return ret;
}

//------------------------------------------------------------------------------

Stream * ResManager::openStream (const char* fileName)
{
    BACKGROUND_LOAD_LOCK

	ResourceObject *obj = find (fileName);
	if (!obj)
		return NULL;
	return openStream (obj);
}

//------------------------------------------------------------------------------

Stream * ResManager::openStream (ResourceObject* obj)
{
    BACKGROUND_LOAD_LOCK

	if (!obj)
		return NULL;

	if (echoFileNames)
		Con::printf ("FILE ACCESS: %s/%s", obj->path, obj->name);

	FileStream *diskStream = NULL;

	// 如果是普通文件
	if (obj->flags & (ResourceObject::File))
	{
		diskStream = new FileStream;		
		if( !diskStream->open (buildPath (obj->path, obj->name), FileStream::Read) )
		{
			delete diskStream;
			return NULL;
		}

		obj->fileSize = diskStream->getStreamSize ();
		return diskStream;
	}

	// 如果是zip文件包或tpk文件包
	if (obj->flags & ResourceObject::VolumeBlock)
	{
#ifdef USE_TPK
		AssertFatal(obj->mTPKArchive, "mTPKArchive is NULL");
		diskStream	= new FileStream;

		char szFileFullPath[MAX_PATH],szTPKPath[MAX_PATH];
		dSprintf(szFileFullPath, sizeof(szFileFullPath), "%s/%s", obj->path, obj->name);
		dSprintf(szTPKPath, sizeof(szFileFullPath), "%s/%s", obj->zipPath, obj->zipName);
		
		char* dot = dStrrchr(szTPKPath, '.');
		if(dot)
		{
			dot -= 2;
			dot[2] = '\0';
			dStrcat(szTPKPath, sizeof(szTPKPath), "/");
		}
		char* findfile = strstr(szFileFullPath, szTPKPath);

        if(0 == findfile)
            return 0;

		findfile = findfile + strlen(szTPKPath);
		// 替换路径中\\为/
		char* scan = findfile;
		while (*scan != '\0')
		{
			if (*scan == '/')
				*scan = '\\';
			scan++;
		}
		TPKStream* tpkStream = new TPKStream;
		tpkStream->attachStream(diskStream);
		tpkStream->SetArchive(obj->mTPKArchive, findfile);
        
        MemStream* pMemStream = new MemStream(tpkStream->getStreamSize());
        tpkStream->read(pMemStream->getStreamSize(),pMemStream->getBuffer());
        
        delete tpkStream;

		return pMemStream;
#else
		AssertFatal(obj->mZipArchive, "mZipArchive is NULL");
		AssertFatal(obj->mCentralDir, "mCentralDir is NULL");
		return obj->mZipArchive->openFileForRead(obj->mCentralDir);
#endif
	}
	return NULL;
}

//------------------------------------------------------------------------------

void ResManager::closeStream(Stream* stream)
{
    BACKGROUND_LOAD_LOCK

	// FIXME [tom, 10/26/2006] Note that this should really hand off to ZipArchive if it's
	// a zip stream, but there's currently no way to get the ZipArchive pointer from
	// here so we just repeat the relevant code. This is pretty lame.

	FilterStream *currentStream, *nextStream;

	// Try to cast the stream to a FilterStream
	nextStream = dynamic_cast<FilterStream*>(stream);
	bool isFilter = nextStream != NULL;

	// While the nextStream is valid (meaning it was successfully cast to a FilterStream)
	while (nextStream)
	{
		// Point currentStream to nextStream
		currentStream = nextStream;
		// Point stream to the Stream contained within the current FilterStream
		stream = currentStream->getStream();
		// Detach the current FilterStream from the Stream contained within it
		currentStream->detachStream();
		// Try to cast the stream (which was already contained within a FilterStream) to a FilterStream
		nextStream = dynamic_cast<FilterStream*>(stream);
		// Delete the FilterStream that was wrapping stream
		delete currentStream;
	}

	if(! isFilter)
		delete stream;
}

//------------------------------------------------------------------------------
// 在文件映射表中查找某文件
// noMissing  是否不记录到丢失文件列表中 
ResourceObject *ResManager::find (const char *fileName, bool noMissing)
{
    BACKGROUND_LOAD_LOCK

	if (!fileName || *fileName == 0)
		return NULL;
	StringTableEntry path, file;
	getPaths (fileName, path, file);
	ResourceObject *ret = dictionary.find (path, file);
	if(!ret)
	{
#ifndef POWER_SHIPPING
		// 如果文件映射表中找不到这个文件，则直接将此文件添加到文件映射表中
		if (Platform::isFile(fileName))
		{
			ret = createResource (path, file);
			dictionary.pushBehind (ret, ResourceObject::File);

			ret->flags = ResourceObject::File;
			ret->fileOffset = 0;
			S32 fileSize = Platform::getFileSize(fileName);
			ret->fileSize = fileSize;
			ret->compressedFileSize = fileSize;
			return ret;
		}
#endif

		if (!noMissing)
			fileIsMissing(fileName);
	}
	return ret;
}

bool ResManager::HasLoaded(const char * fileName)
{
    BACKGROUND_LOAD_LOCK

    ResourceObject* pRes = find(fileName);

    return 0 != pRes && 0 != pRes->mInstance;
}

//------------------------------------------------------------------------------

ResourceObject *ResManager::find (const char *fileName, U32 flags)
{
    BACKGROUND_LOAD_LOCK

	if (!fileName || *fileName == 0)
		return NULL;
	StringTableEntry path, file;
	getPaths (fileName, path, file);
	return dictionary.find (path, file, flags);
}


//------------------------------------------------------------------------------
// Add resource constructed outside the manager

bool ResManager::add (const char *name, ResourceInstance * addInstance,
					  bool extraLock)
{
	StringTableEntry path, file;
	getPaths (name, path, file);

	ResourceObject *obj = dictionary.find (path, file);
	if (obj && obj->mInstance)
		// Resource already exists?
		return false;

	if (!obj)
		obj = createResource (path, file);

	dictionary.pushBehind (obj,
		ResourceObject::File | ResourceObject::VolumeBlock);
	obj->mInstance = addInstance;
	addInstance->mSourceResource = obj;
	obj->lockCount = extraLock ? 2 : 1;
	unlock (obj);
	return true;
}

//------------------------------------------------------------------------------

void ResManager::purge ()
{
	bool found;
	do
	{
		ResourceObject *obj = timeoutList.getNext ();
		found = false;
		while (obj)
		{
			ResourceObject *temp = obj;
			obj = obj->next;
			temp->unlink ();
			temp->destruct ();
			found = true;
			if (temp->flags & ResourceObject::Added)
				freeResource (temp);
		}
	}
	while (found);
}

ConsoleFunction( purgeResources, void, 1, 1, "Purge resources from the resource manager.")
{
	ResourceManager->purge();
}

//------------------------------------------------------------------------------

void ResManager::purge (ResourceObject * obj)
{
	AssertFatal (obj->lockCount == 0,
		"ResourceManager::purge: handle lock count is not ZERO.") obj->
		unlink ();
	obj->destruct ();
}

//------------------------------------------------------------------------------
// serialize sorts a list of files by .zip and position within the zip
// it allows an aggregate (material list, etc) to find the preferred
// loading order for a set of files.
//------------------------------------------------------------------------------

struct ResourceObjectIndex
{
	ResourceObject *ro;
	const char *fileName;

	static S32 QSORT_CALLBACK compare (const void *s1, const void *s2)
	{
		const ResourceObjectIndex *r1 = (ResourceObjectIndex *) s1;
		const ResourceObjectIndex *r2 = (ResourceObjectIndex *) s2;

		if (r1->ro->path != r2->ro->path)
			return r1->ro->path - r2->ro->path;
		if (r1->ro->name != r2->ro->name)
			return r1->ro->name - r2->ro->name;
		return r1->ro->fileOffset - r2->ro->fileOffset;
	}
};

//------------------------------------------------------------------------------

void ResManager::serialize (VectorPtr < const char *>&filenames)
{
	Vector < ResourceObjectIndex > sortVector;

	sortVector.reserve (filenames.size ());

	U32 i;
	for (i = 0; i < filenames.size (); i++)
	{
		ResourceObjectIndex roi;
		roi.ro = find (filenames[i]);
		roi.fileName = filenames[i];
		sortVector.push_back (roi);
	}

	dQsort ((void *) &sortVector[0], sortVector.size (),
		sizeof (ResourceObjectIndex), ResourceObjectIndex::compare);
	for (i = 0; i < filenames.size (); i++)
		filenames[i] = sortVector[i].fileName;
}

//------------------------------------------------------------------------------

ResourceObject * ResManager::findMatch (const char *expression, const char **fn,
										ResourceObject * start)
{	
	if (!start)
		start = resourceList.nextResource;
	else
		start = start->nextResource;
	while (start)
	{
		const char *fname = buildPath (start->path, start->name);
		if (FindMatch::isMatch (expression, fname, false))
		{
			*fn = fname;
			return start;
		}
		start = start->nextResource;
	}
	return NULL;
}
//------------------------------------------------------------------------------
ResourceObject * ResManager::findMatchMultiExprs (const char *multiExpression, const char **fn,
												  ResourceObject * start)
{
	if (!start)
		start = resourceList.nextResource;
	else
		start = start->nextResource;
	while (start)
	{
		const char *fname = buildPath (start->path, start->name);
		if (FindMatch::isMatchMultipleExprs(multiExpression, fname, false))
		{
			*fn = fname;
			return start;
		}
		start = start->nextResource;
	}
	return NULL;
}
//------------------------------------------------------------------------------
S32 ResManager::findMatches (FindMatch * pFM)
{
	static char buffer[16384];
	S32 bufl = 0;
	ResourceObject * walk;
	for (walk = resourceList.nextResource; walk && !pFM->isFull (); walk = walk->nextResource)
	{
		const char * fpath =
			buildPath (walk->path, walk->name);
		if (bufl + dStrlen (fpath) >= 16380)
			return pFM->numMatches ();
		dStrcpy (buffer + bufl, sizeof(buffer) - bufl, fpath);
		if (pFM->findMatch (buffer + bufl))
			bufl += dStrlen (fpath) + 1;
	}
	return (pFM->numMatches ());
}

//------------------------------------------------------------------------------

bool ResManager::findFile (const char *name)
{
	return (bool) find (name);
}

//------------------------------------------------------------------------------

ResourceObject* ResManager::createResource (StringTableEntry path, StringTableEntry file)
{
	ResourceObject *newRO = dictionary.find (path, file);

	if (newRO)
		return newRO;

	newRO = new ResourceObject;
	newRO->path = path;
	newRO->name = file;
	newRO->lockCount = 0;
	newRO->mInstance = NULL;
	newRO->flags = ResourceObject::Added;
	newRO->next = newRO->prev = NULL;
	newRO->nextResource = resourceList.nextResource;
	resourceList.nextResource = newRO;
	newRO->prevResource = &resourceList;
	if (newRO->nextResource)
		newRO->nextResource->prevResource = newRO;
	dictionary.insert (newRO, path, file);
	newRO->fileSize = newRO->fileOffset = newRO->compressedFileSize = 0;
	newRO->zipPath = NULL;
	newRO->zipName = NULL;
	newRO->crc = InvalidCRC;

	return newRO;
}

//------------------------------------------------------------------------------

ResourceObject * ResManager::createZipResource (StringTableEntry path, StringTableEntry file,
												StringTableEntry zipPath,
												StringTableEntry zipName)
{
	ResourceObject *newRO = dictionary.find (path, file, zipPath, zipName);
	if (newRO)
		return newRO;

	newRO = new ResourceObject;
	newRO->path = path;
	newRO->name = file;
	newRO->lockCount = 0;
	newRO->mInstance = NULL;
	newRO->flags = ResourceObject::Added;
	newRO->next = newRO->prev = NULL;
	newRO->nextResource = resourceList.nextResource;
	resourceList.nextResource = newRO;
	newRO->prevResource = &resourceList;
	if (newRO->nextResource)
		newRO->nextResource->prevResource = newRO;
	dictionary.insert (newRO, path, file);
	newRO->fileSize = newRO->fileOffset = newRO->compressedFileSize = 0;
	newRO->zipPath = zipPath;
	newRO->zipName = zipName;
	newRO->crc = InvalidCRC;
#ifdef USE_TPK
	newRO->mTPKArchive = NULL;
	newRO->BlockIndex = 0;
#else
	newRO->mZipArchive = NULL;
	newRO->mCentralDir = NULL;
#endif
	return newRO;
}

//------------------------------------------------------------------------------

void ResManager::freeResource (ResourceObject * ro)
{
	ro->destruct ();
	ro->unlink ();

	//   if((ro->flags & ResourceObject::File) && ro->lockedData)
	//      delete[] ro->lockedData;

	if (ro->prevResource)
		ro->prevResource->nextResource = ro->nextResource;
	if (ro->nextResource)
		ro->nextResource->prevResource = ro->prevResource;
	dictionary.remove (ro);
	delete ro;
}

//------------------------------------------------------------------------------

bool ResManager::openFileForWrite (FileStream & stream, const char *fileName, U32 accessMode)
{
	if (!isValidWriteFileName (fileName))
		return false;

	// tag it on to the first directory
	char path[1024];
	dStrcpy (path, sizeof(path), fileName);
	char *file = dStrrchr (path, '/');
	if (!file)
		return false;      // don't allow storing files in root
	*file++ = 0;

	if (!Platform::createPath (fileName))   // create directory tree
		return false;
	if (!stream.open (fileName, (FileStream::AccessMode) accessMode))
		return false;

	// create a resource for the file.
	ResourceObject *ro = createResource (StringTable->insert (path), StringTable->insert (file));
	ro->flags = ResourceObject::File;
	ro->fileOffset = 0;
	ro->fileSize = 0;
	ro->compressedFileSize = 0;
	return true;
}

ConsoleFunction(isUsingVFS, bool, 1, 1, "()")
{
	return ResourceManager->isUsingVFS();
}
//------------------------------------------------------------------------------
// reload - reload updated resource
//------------------------------------------------------------------------------
ResourceObject *ResManager::reload (const char *fileName, bool computeCRC)
{
	if (!fileName)
		return NULL;

	StringTableEntry path, file;
	getPaths (fileName, path, file);
	ResourceObject *obj = dictionary.find (path, file);

	// If we found the resource it is already loaded.
	// Check the crc to see if it needs reloading
	if (obj)
	{
		bool reload = true;

		// If we have a valid crc check against the new crc
		if (obj->crc != InvalidCRC)
		{
			FileStream file;
			file.open(fileName, FileStream::Read);

			U32 newCRC = calculateCRCStream (&file, InvalidCRC);
			file.close();

			if (newCRC != obj->crc)
				reload = true;
		}

		if (reload)
			obj->destruct ();
	}
	// If not then try to find it outside of the dictionary
	else
		obj = find (fileName);

	if (!obj)
		return NULL;

	// if no one has a lock on this, but it's loaded and it needs to
	// be CRC'd, delete it and reload it.
	if (!obj->lockCount && computeCRC && obj->mInstance)
		obj->destruct ();

	obj->lockCount++;
	obj->unlink ();      // remove from purge list
	if (!obj->mInstance)
	{
		obj->mInstance = loadInstance (obj, computeCRC);
		if (!obj->mInstance)
		{
			obj->lockCount--;
			return NULL;
		}
	}
	return obj;
}

//------------------------------------------------------------------------------
// Start resource traversal - wipe internal data for a new traversal
//------------------------------------------------------------------------------
void ResManager::startResourceTraverse()
{
	mTraverseHashIndex = 0;
	mTraverseCurObj = NULL;
}

//------------------------------------------------------------------------------
// Get next resource in resource traversal
//------------------------------------------------------------------------------
ResourceObject* ResManager::getNextResource()
{
	if( mTraverseCurObj )
	{
		if( mTraverseCurObj->nextEntry )
		{
			mTraverseCurObj = mTraverseCurObj->nextEntry;
			return mTraverseCurObj;
		}
		else
		{
			++mTraverseHashIndex;
		}
	}

	U32 hashTableSize = 0;
	ResourceObject **hashTable = NULL;
	dictionary.getHash( &hashTable, &hashTableSize );

	for( U32 i=mTraverseHashIndex; i<hashTableSize; i++ )
	{
		ResourceObject * obj = hashTable[i];
		if( obj != NULL )
		{
			mTraverseHashIndex = i;
			mTraverseCurObj = obj;
			return mTraverseCurObj;
		}
	}

	return NULL;  // end of traversal
}

//-----------------------------------------------------------------------------
// 记录丢失的文件
void ResManager::fileIsMissing(const char *fileName)
{
	if(mLoggingMissingFiles)
	{
		char *name = dStrdup(fileName);
		//      Con::printf("> Missing file: %s", fileName);
		mMissingFileList.push_back(name);
	}
}

//-----------------------------------------------------------------------------
// 设置是否记录丢失的文件
void ResManager::setMissingFileLogging(bool logging)
{
	mLoggingMissingFiles = logging;
	if(!mLoggingMissingFiles)
		clearMissingFileList();
}

//-----------------------------------------------------------------------------
// 清除丢失文件的列表
void ResManager::clearMissingFileList()
{
	while(mMissingFileList.size())
	{
		dFree(mMissingFileList[0]);
		mMissingFileList.pop_front();
	}
	mMissingFileList.clear();
}

//-----------------------------------------------------------------------------
// 获取丢失文件的列表
bool ResManager::getMissingFileList(Vector<char *> &list)
{
	if(!mMissingFileList.size())
		return false;

	for(U32 i = 0; i < mMissingFileList.size();i ++)
	{
		for(U32 j = 0; j < list.size(); j++)
		{
			if(!dStrcmp(list[j], mMissingFileList[i]))
			{
				dFree(mMissingFileList[i]);
				mMissingFileList[i] = NULL;
				break;
			}
		}
		if(mMissingFileList[i])
			list.push_back(mMissingFileList[i]);
	}
	mMissingFileList.clear();
	return true;
}

#ifdef POWER_DEBUG
void ResManager::dumpLoadedResources ()
{
	ResourceObject *walk = resourceList.nextResource;
	while (walk != NULL)
	{
		if (walk->mInstance != NULL)
		{
			Con::errorf ("LoadedRes: %s/%s (%d)", walk->path, walk->name,
				walk->lockCount);
		}
		walk = walk->nextResource;
	}
}

ConsoleFunction(dumpResourceStats, void, 1, 1, "打印所有已经装载的资源文件(用于调试)!")
{
	ResourceManager->dumpLoadedResources();
}

#endif

//------------------------------------------------------------------------------
// 添加搜索路径
void ResManager::addPath(const char *path, bool ignoreZips )
{
	searchPath(path, true, ignoreZips );
}

ConsoleFunction(addResPath, void, 2, 3, "(path, [ignoreZips=false]) Add a path to the resource manager")
{
	if( argc > 2 )
		ResourceManager->addPath(argv[1], dAtob(argv[2]));
	else
		ResourceManager->addPath(argv[1]);
}

//------------------------------------------------------------------------------
// 从文件映射表里搜索并移除某路径下的所有资源对象
void ResManager::removePath(const char *path)
{
	ResourceObject *rwalk = resourceList.nextResource, *rtemp;
	while (rwalk != NULL)
	{
		const char *fname = buildPath(rwalk->path, rwalk->name);
		if(!rwalk->mInstance && FindMatch::isMatch(path, fname, false))
		{
			rwalk->unlink ();
			dictionary.remove (rwalk);
			rtemp = rwalk->nextResource;
			freeResource (rwalk);
			rwalk = rtemp;
		}
		else
			rwalk = rwalk->nextResource;
	}
}

ConsoleFunction(removeResPath, void, 2, 2, "(pathExpression) Remove a path from the resource manager. Path is an expression as in findFirstFile()")
{
	ResourceManager->removePath(argv[1]);
}

//------------------------------------------------------------------------------
// 设置是否打印输出到控制台(用于调试)
void ResManager::setFileNameEcho (bool on)
{
	echoFileNames = on;
}

//------------------------------------------------------------------------------
// 是否有效的可写入的路径文件名
bool ResManager::isValidWriteFileName (const char *fn)
{
	if(isUsingVFS())
		return false;

	// #ifndef POWER_TOOLS
	//    // all files must be based off the VFS
	//    if (fn[0] == '/' || dStrchr (fn, ':'))
	//       return false;
	// #endif

	if (!writeablePath[0])
		return true;

#ifdef POWER_PLAYER
	char pathBuf[1024];
	Platform::makeFullPathName(fn, pathBuf, sizeof(pathBuf), Platform::getMainDotCsDir());
	fn = pathBuf;
#endif

	// get the path to the file
	const char * path = dStrrchr (fn, '/');
	if (!path)
		path = fn;
	else
	{
		if (!dStrchr (path, '.'))
			return false;
	}

	// now loop through the writeable path.
	const char * start = writeablePath;
	for (;;)
	{
		const char * end = dStrchr (writeablePath, ';');
		if (!end)
			end = writeablePath + dStrlen (writeablePath);

		//if (end - start == pathLen && !dStrnicmp (start, path, pathLen))
		if(dStrnicmp(start, fn, end - start) == 0)
			return true;
		if (end[0])
			start = end + 1;
		else
			break;
	}
	return false;
}

//------------------------------------------------------------------------------
// 设置可写入路径
void ResManager::setWriteablePath (const char *path)
{
	dStrcpy (writeablePath, sizeof(writeablePath), path);
}

//------------------------------------------------------------------------------
const char *ResManager::getBasePath ()
{
	if (!pathList)
		return NULL;
	const char *base = dStrrchr (pathList, ';');
	return base ? (base + 1) : pathList;
}

//-----------------------------------------------------------------------------
// 获取文件创建时间和修改时间
void ResourceObject::getFileTimes (FileTime* createTime, FileTime* modifyTime)
{
	if( !path || !name )
	{
		createTime = modifyTime = NULL;
		return;
	}

	char buffer[1024];
	Platform::makeFullPathName(path, buffer, sizeof(buffer));
	U32 len = dStrlen(buffer);
	dSprintf (buffer + len, sizeof (buffer) - len, "/%s", name);

	Platform::getFileTimes (buffer, createTime, modifyTime);
}

//------------------------------------------------------------------------------
// 计算文件的Crc32值，忽略stream类型
bool ResManager::getCrc (const char *fileName, U32 & crcVal, const U32 crcInitialVal)
{
	ResourceObject *obj = find (fileName);
	if (!obj)
		return (false);

	if (obj->flags & (ResourceObject::VolumeBlock | ResourceObject::File))
	{
		if (obj->lockCount)
			return false;

		obj->unlink ();
		obj->destruct ();

		Stream *stream = openStream (obj);

		U32 waterMark = 0xFFFFFFFF;

		U8 *buffer;
		U32 maxSize = FrameAllocator::getHighWaterMark () - FrameAllocator::getWaterMark ();
		if (maxSize < obj->fileSize)
			buffer = new U8[obj->fileSize];
		else
		{
			waterMark = FrameAllocator::getWaterMark ();
			buffer = (U8 *) FrameAllocator::alloc (obj->fileSize);
		}

		stream->read (obj->fileSize, buffer);

		crcVal = calculateCRC (buffer, obj->fileSize, crcInitialVal);
		if (waterMark == 0xFFFFFFFF)
			delete[]buffer;
		else
			FrameAllocator::setWaterMark (waterMark);

		closeStream (stream);
		return (true);
	}

	return (false);
}


void ResManager::preLoad( const char* fileName, bool computeCRC /*= false */ )
{
	S32 length = dStrlen(fileName) + 1;
	char* str = new char[length];
	dStrcpy( (char*)str, length, fileName );

	m_pWorker->postWork( WorkMethod( ResManager::_load ), this, 2, str, computeCRC );
}