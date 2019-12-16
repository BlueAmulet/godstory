//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _RESMANAGER_H_
#define _RESMANAGER_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif
#ifndef _TVECTOR_H_
#include "core/tVector.h"
#endif
#ifndef _STRINGTABLE_H_
#include "core/stringTable.h"
#endif

#ifndef _FILESTREAM_H_
#include "core/fileStream.h"
#endif
#ifndef _CRC_H_
#include "core/crc.h"
#endif

#include "platform/threads/mutex.h"

#define USE_TPK

class Stream;
class FileStream;
class ZipSubRStream;
class ResManager;
class FindMatch;
struct TPKArchive;

namespace Zip
{
   class ZipArchive;
   class CentralDir;
}


extern ResManager *ResourceManager;

#define IMAGE_EXT_ARRAY_SIZE 6
static const char* imageExtArray[IMAGE_EXT_ARRAY_SIZE] = { ".jpg", ".png", ".gif", ".bmp", ".dds", ".jng" };

#ifdef POWER_NO_OGGVORBIS
   #define SOUND_EXT_ARRAY_SIZE 1
   static const char* soundExtArray[SOUND_EXT_ARRAY_SIZE] = { ".wav" };
#else
   #define SOUND_EXT_ARRAY_SIZE 3
   static const char* soundExtArray[SOUND_EXT_ARRAY_SIZE] = { ".wav", ".ogg", ".mp3" };
#endif

#define InvalidCRC 0xFFFFFFFF

StringTableEntry stripExtension(const char* fileName, U32 numValidExt = 0, const char* validExtension[] = NULL);
StringTableEntry addExtension(const char* fileName, const char* extension);

//------------------------------------------------------------------------------
class ResourceObject;

//===============================================================================
//	资源文件基类（ResourceInstance）
//		所有需要通过虚拟文件系统管理的文件类型都需要继承此类。首先需要为此文件资
//	源类构建一个参数为Stream的流数据读取的操作函数，然后将此操作函数登记注册给虚
//	拟文件系统来管理维护。例如：
//	@code
//	class GBitmap : ResourceInstance 
//	{
//		//任何你想要实现的代码
//	}
//	ResourceInstance* constructBitmapBMP(Stream &stream)
//	{
//		GBitmap *bmp = new GBitmap;
//		if(bmp->readMSBmp(stream))
//			return bmp;
//		else
//		{
//			delete bmp;
//			return NULL;
//		}
//	}
//	ResourceManager->registerExtension(".bmp", constructBitmapBMP);
//	@endcode
//===============================================================================
class ResourceInstance:public CVirtualAllocBase
{
public:
	ResourceObject *mSourceResource;
	ResourceInstance() { mSourceResource = NULL; }
	virtual ~ResourceInstance() {}
};

// 虚拟文件系统资源文件构建的函数指针定义
typedef ResourceInstance* (*RESOURCE_CREATE_FN)(Stream &stream, ResourceObject *obj);

//===============================================================================
//	资源文件链表节点类（ResourceObject）
//		定义了虚拟文件系统的文件链表结构上节点对象的数据，诸如文件名，路径名，文件
//	位置，容量，引用计数等等, 此为虚拟文件对象操作模板类的使用类，通常不直接使用，
//	而使用虚拟文件的资源处理类Resource.
//===============================================================================
class ResourceObject
{
	friend class ResDictionary;
	friend class ResManager;

	ResourceObject *prev, *next;		// 超时文件链表, 前一个文件或后一个文件
	ResourceObject *nextEntry;			// 虚拟文件映射表的后一个ID的文件，用于ResDictionary
	ResourceObject *prevResource,*nextResource;// 文件链表的前一个节点对象和后一个节点对象

public:
	enum Flags
	{
		VolumeBlock   = BIT(0),			// 有文件包的
		File          = BIT(1),			// 普通本地文件
		Added         = BIT(2),
	};
	S32 flags;

	StringTableEntry path;				// 资源文件路径
	StringTableEntry name;				// 资源文件名

	ResourceInstance *mInstance;		// 资源文件句柄（资源未加载，则为NULL或未定义）
	S32  lockCount;						// 引用计数. 当一个文件被多次打开的时候, 虚拟文件系统会自动返回当前的handle并为引用计数加1.
	U32  crc;							// 资源文件的CRC32值

	// -----------------------------------------------------
	// ZIP压缩文档管理
	StringTableEntry zipPath;			// zip压缩包文件本地路径
	StringTableEntry zipName;			// zip压缩包文件名
	S32 fileOffset;						// 资源文件在zip包内的偏移位置
	S32 fileSize;						// 资源文件大小
	S32 compressedFileSize;				// 资源数据的真实大小（可能是压缩后字节）				

#ifdef USE_TPK
	TPKArchive* mTPKArchive;			// TPK文件包对象
	U32 BlockIndex;						// TPK文件包中当前文件所在的Block项索引值	
#else
	Zip::ZipArchive *mZipArchive;		// zip压缩包对象
	const Zip::CentralDir *mCentralDir;
#endif
	ResourceObject();
	~ResourceObject() { unlink(); }
	void destruct();

	// ------------------------------------------------------
	// 文件链表的管理
	ResourceObject* getNext() const { return next; }
	void unlink();
	void linkAfter(ResourceObject* res);

	// ------------------------------------------------------
	// 其它

	// 获取文件创建时间和修改时间
	void getFileTimes(FileTime *createTime, FileTime *modifyTime);
	// 获取完整路径
	const char* getFullPath();
};



//===============================================================================
//	用于操作资源文件对象的操作模板(Resource).
//		如果一个资源文件对象被分配, 则它被默认为锁定的. 引用计数在该文件被再次打开或
//	其它资源文件操作器被设定为该文件的时候加1. 举例如下:
//	@code
//	Resource<TerrainFile> terrRes;
//	terrRes = ResourceManager->load(fileName);
//		if(!bool(terrRes))
//			Con::errorf(ConsoleLogEntry::General, "Terraformer::terrainFile - invalid terrain file '%s'.", fileName);
//	@endcode
//	当这个Resource<>被释放时，它将释放锁定,引用计数减1
//===============================================================================
template <class T> class Resource
{
private:
	ResourceObject *obj;

	// ***WARNING***
	// Using a faster lock that bypasses the resource manger.
	// void _lock() { if (obj) obj->rm->lockResource( obj ); }
	void _lock();		// 对资源文件对象引用计数加1
	void _unlock();     // 对资源文件对象引用计数减1

public:
	Resource() : obj(NULL) { ; }
	Resource(ResourceObject *p) : obj(p) { ; }
	Resource(const Resource &res) : obj(res.obj) { _lock(); }
	~Resource() { unlock(); }
	
	const char *getFilePath() const { return (obj ? obj->path : NULL); }	// 获取路径
	const char *getFileName() const { return (obj ? obj->name : NULL); }	// 获取文件名

	Resource& operator= (ResourceObject *p) { _unlock(); obj = p; return *this; }
	Resource& operator= (const Resource &r) { _unlock(); obj = r.obj; _lock(); return *this; }

	U32 getCRC() { return (obj ? obj->crc : 0); }
	bool isNull()   const { return ((obj == NULL) || (obj->mInstance == NULL)); }
	operator bool() const { return ((obj != NULL) && (obj->mInstance != NULL)); }
	bool operator!() const { return ((obj == NULL) || (obj->mInstance == NULL));}
	T* operator->()   { return (T*)obj->mInstance; }
	T& operator*()    { return *((T*)obj->mInstance); }
	operator T*() const    { return (obj) ? (T*)obj->mInstance : (T*)NULL; }
	const T* operator->() const  { return (const T*)obj->mInstance; }
	const T& operator*() const   { return *((const T*)obj->mInstance); }
	operator const T*() const    { return (obj) ? (const T*)obj->mInstance :  (const T*)NULL; }
	void unlock();
	void purge();

    ResourceObject* GetResourceObj(void) {return obj;}
};

#define INVALID_ID ((U32)(~0))

//===============================================================================
//	文件映射表类
//		使用哈西表根据对象的ID和名称映射到对象。并提供快速的对象搜索和删除方法。
//	此类被虚拟文件系统内部访问，程序人员不需要直接访问和操作该类。
//===============================================================================
class ResDictionary
{
   enum { DefaultTableSize = 1029 };

   ResourceObject **hashTable;
   S32 entryCount;
   S32 hashTableSize;
   DataChunker memPool;
   S32 hash(StringTableEntry path, StringTableEntry name);
   S32 hash(ResourceObject *obj) { return hash(obj->path, obj->name); }

public:
   ResDictionary();
   ~ResDictionary();

   // 添加资源文件到文件映射表里
   void insert(ResourceObject *obj, StringTableEntry path, StringTableEntry file);

   // 搜索资源文件对象
   //	这个函数利用哈西表快速搜索，如果资源已经加载，则返回这个资源文件对象，否则返回空
   ResourceObject* find(StringTableEntry path, StringTableEntry file);
   ResourceObject* find(StringTableEntry path, StringTableEntry file, StringTableEntry filePath, StringTableEntry fileName);
   ResourceObject* find(StringTableEntry path, StringTableEntry file, U32 flags);
   /// @}

   // 移动文件映射表内某资源文件对象到特定匹配的若干资源文件对象的后面（等同于一个特定排序）
   void pushBehind(ResourceObject *obj, S32 mask);

   // 从文件映射表内去除某资源文件对象
   void remove(ResourceObject *obj);
   // 获取当前映射表文件链接对象和哈西表长度
   void getHash( ResourceObject ***hash, U32 *hashSize ){ *hash = hashTable; *hashSize = hashTableSize; }
};

//===============================================================================
//	虚拟文件系统管理器(ResManager)
//		虚拟文件系统中定义了自己的根目录. 其根目录位置是游戏可执行文件所在的路径. 
//	虚拟文件系统中文件名的规则类似UNIX文件名约定, 通过斜杠(/)符号来分隔路径, 同时,
//	合法文件名需要满足如下规定:
//		@li	第一个字符不能是"/". 例如需要访问Common目录下的文件sample.abc, 则直接
//	输入"Common/sample.abc"即可, 不用在第一个字母前加斜杠.
//		@li	不能包含冒号(:)及其它Windows文件系统不允许的字符, 如"C:/SomeFile.abc"
//	是非法文件名.
//
//		例如在C:\MyGame目录中存放着用MyGame.exe文件, 需要访问如下文件:
//		@li	C:\MyGame\textfile.txt
//		@li	C:\MyGame\Data\Images\myphoto.jpg
//		@li	C:\MyGame\Data\Actors\myself.mtj
//
//		通过虚拟文件系统访问这些文件, 所需要输入的完整文件和文件名分别是:
//		@li	textfile.txt
//		@li	Data/Images/myphoto.jpg
//		@li	Data/Actors/myself.mtj
//
//		类似Windows中的Windows Explorer, 虚拟文件系统管理器(ResManager)负责虚拟文件系统
//	的管理工作并提供操作用于管理虚拟文件系统的接口. ResManager的主要功能包括:
//		@li	管理资源文件在虚拟文件系统中的位置.
//		@li	扫描基础路径和子路径中的所有资源文件
//		@li	将文件装载至内存.
//		@li	已经装载进内存的文件可以再被保存到相关文件(如果需要).
//		@li	通过验证CRC保证文件的完整性.
//
//		如果一个类想成为能被ResManager管理的资源类, 必须:
//		@li	从ResourceInstance继承.
//		@li	实现一个文件构建类.
//		@li	相关的文件后缀需要通过ResManager进行注册.
//
//		ResManager是一个单实例类, 在全局仅存在该类的一个实例ResourceManager
//===============================================================================

class CBackWorker;

class ResManager
{
private:
   char writeablePath[1024];					// 写入数据的目标路径，一般用于从Server下载数据时写入本地磁盘
   char primaryPath[1024];						// 可加载数据的源路径
   char* pathList;								// 有效的游戏目录名

   ResourceObject timeoutList;					// 超时文件列表
   ResourceObject resourceList;					// 资源文件列表

   ResDictionary dictionary;					// 文件映射表

   bool echoFileNames;							// 判定是否显示文件名,仅用于调试
   bool usingVFS;								// 是否使用虚拟文件

   U32              mTraverseHashIndex;			// 用于循环遍历资源文件的索引
   ResourceObject * mTraverseCurObj;			// 用于循环遍历资源文件的对象

   bool isIgnoredSubdirectoryName(const char *name) const;	// 是否忽略子目录名称

#ifdef USE_TPK
   bool scanTPK(ResourceObject* tpkObject);		// 扫描tpk文件包内的所有资源
#else
   bool scanZip(ResourceObject* zipObject);		// 扫描zip压缩包内的所有资源
#endif
   
    ResourceObject* createResource(StringTableEntry path, StringTableEntry file);	// 创建资源文件对象
   // 为zip压缩包里的文件创建资源文件对象
   ResourceObject* createZipResource(StringTableEntry path, StringTableEntry file, StringTableEntry zipPath, StringTableEntry zipFle);

   // 搜索目录路径，包括zip文件包，将文件创建到资源文件对象
   void searchPath(const char *pathStart, bool noDups = false, bool ignoreZips = false);
   bool setModZip(const char* path);

   struct RegisteredExtension
   {
      StringTableEntry     mExtension;			// 后缀名, 需要包含点(.)
      RESOURCE_CREATE_FN   mCreateFn;			// 虚拟文件构建函数.
      RegisteredExtension  *next;				// 向后的链表
   };

   Vector<char *> mMissingFileList;				// 丢失的文件链表（未能在文件映射表内查找到的文件）
   bool mLoggingMissingFiles;					// 是否登记丢失的文件到丢失文件列表中

   RegisteredExtension *registeredList;			// 注册资源文件的列表	

   static char *smExcludedDirectories;			// 被排除忽略的目录列表

   CBackWorker* m_pWorker;						// 多线程调度
   ResManager();
public:
   RESOURCE_CREATE_FN getCreateFunction( const char *name );

   ~ResManager();

   static void create();
   static void destroy();

	// 初始化排除的目录
   static void initExcludedDirectories();

   

   void setFileNameEcho(bool on);                     // 设置是否显示文件名（用于调试）
   void setModPaths(U32 numPaths, const char **dirs); // 设置游戏数据和内容路径
   const char* getModPaths();

   void addPath(const char *path, bool ignoreZips=false);// 添加路径
   void removePath(const char *path);			// 移除路径

   void fileIsMissing(const char *fileName);          // 文件丢失时调用
   void setMissingFileLogging(bool log);              // 设置是否记录丢失的文件
   bool getMissingFileList(Vector<char *> &list);     // 获取丢失的文件列表
   void clearMissingFileList();                       // 清除丢失的文件列表

   //　登录资源文件加载的方式
   void registerExtension(const char *extension, RESOURCE_CREATE_FN create_fn);

   S32 getSize(const char* filename);                 // 获取文件大小
   const char* getFullPath(const char * filename, char * path, U32 pathLen);  // 获取完整路径
   const char* getModPathOf(const char* fileName);
   const char* getPathOf(const char * filename);
   const char* getBasePath();
   
   //now background load is supported
   ResourceObject* load(const char * fileName, bool computeCRC = false);
   ResourceObject* _cdecl _load(const char * fileName, bool computeCRC = false);

   void preLoad( const char* fileName, bool computeCRC = false );

   Stream*  openStream(const char * fileName);
   Stream*  openStream(ResourceObject *object);
   void     closeStream(Stream *stream);

   // 对一个资源文件对象解除锁引用，如果引用计数为0，这个资源文件对象将被置入到超时文件列表中，便于资源的清除
   void unlock( ResourceObject* );

   // 添加一个资源文件实例
   bool add(const char* name, ResourceInstance *addInstance, bool extraLock = false);

   // 在文件映射表中搜索资源文件对象，可以考虑存在在丢失文件列表中的情况
   ResourceObject* find(const char * fileName, bool noMissing = false);
   bool            HasLoaded(const char * fileName);
   // 载入获取资源文件实例(通过资源文件名在文件映射表中检索）
   ResourceInstance* loadInstance(const char *fileName, bool computeCRC = false, bool noMissing = false);
   // 载入获取资源文件实例(通过资源文件对象在文件映射表中检索）
   ResourceInstance* loadInstance(ResourceObject *object, bool computeCRC = false);

   // 文件映射表中搜索资源文件
   ResourceObject* find(const char * fileName, U32 flags);
   // 通过匹配表达式，查询一个资源文件对象
   ResourceObject* findMatch(const char *expression, const char **fn, ResourceObject *start = NULL);
   // 通过多个匹配表达式，来查询一个资源文件对象，用" "来分隔
   ResourceObject* findMatchMultiExprs(const char *multiExpression, const char **fn, ResourceObject *start = NULL);

   void purge();                                      // 通过超时列表来处理清除所有资源
   void purge( ResourceObject *obj );                 // 清除一个资源
   void freeResource(ResourceObject *resObject);      // 释放一个资源
   void serialize(VectorPtr<const char *> &filenames);// 排序资源文件对象

   S32  findMatches( FindMatch *pFM );                // 匹配检索多个文件
   bool findFile( const char *name );                 // 检查文件是否存在

   bool addVFSRoot(Zip::ZipArchive *vfs);
   bool isUsingVFS() { return usingVFS; }

   // 通过不同的crcInitialVal初始值，可以计算不同文件的Crc32值
   bool getCrc(const char * fileName, U32 & crcVal, const U32 crcInitialVal = INITIAL_CRC_VALUE );

   void setWriteablePath(const char *path);           // 设置可写路径
   bool isValidWriteFileName(const char *fn);         // 检查是否有效的可写路径的文件名

   // 打开文件来写
   bool openFileForWrite(FileStream &fs, const char *fileName, U32 accessMode = 1);

   void startResourceTraverse();
   ResourceObject * getNextResource();

   ResourceObject* reload(const char * fileName, bool computeCRC = false); // 重新加载资源文件对象
   void reloadResources();

#ifdef POWER_DEBUG
   void dumpLoadedResources();                        // 打印所有加载的资源文件信息到控制台
#endif
  
   static void getPaths (const char *fullPath, StringTableEntry & path, StringTableEntry & fileName);
};

template<class T> inline void Resource<T>::unlock()
{
   if (obj)
   {
      ResourceManager->unlock( obj );
      obj=NULL;
   }
}

template<class T> inline void Resource<T>::purge()
{
   if (obj)
   {
      ResourceManager->unlock( obj );
      if (obj->lockCount == 0)
         ResourceManager->purge(obj);
      obj = NULL;
   }
}

template <class T> inline void Resource<T>::_lock()
{
   if (obj)
      obj->lockCount++;
}

template <class T> inline void Resource<T>::_unlock()
{
   if (obj)
      ResourceManager->unlock( obj );
}

#endif //_RESMANAGER_H_
