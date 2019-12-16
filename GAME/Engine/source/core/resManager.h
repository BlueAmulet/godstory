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
//	��Դ�ļ����ࣨResourceInstance��
//		������Ҫͨ�������ļ�ϵͳ������ļ����Ͷ���Ҫ�̳д��ࡣ������ҪΪ���ļ���
//	Դ�๹��һ������ΪStream�������ݶ�ȡ�Ĳ���������Ȼ�󽫴˲��������Ǽ�ע�����
//	���ļ�ϵͳ������ά�������磺
//	@code
//	class GBitmap : ResourceInstance 
//	{
//		//�κ�����Ҫʵ�ֵĴ���
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

// �����ļ�ϵͳ��Դ�ļ������ĺ���ָ�붨��
typedef ResourceInstance* (*RESOURCE_CREATE_FN)(Stream &stream, ResourceObject *obj);

//===============================================================================
//	��Դ�ļ�����ڵ��ࣨResourceObject��
//		�����������ļ�ϵͳ���ļ�����ṹ�Ͻڵ��������ݣ������ļ�����·�������ļ�
//	λ�ã����������ü����ȵ�, ��Ϊ�����ļ��������ģ�����ʹ���࣬ͨ����ֱ��ʹ�ã�
//	��ʹ�������ļ�����Դ������Resource.
//===============================================================================
class ResourceObject
{
	friend class ResDictionary;
	friend class ResManager;

	ResourceObject *prev, *next;		// ��ʱ�ļ�����, ǰһ���ļ����һ���ļ�
	ResourceObject *nextEntry;			// �����ļ�ӳ���ĺ�һ��ID���ļ�������ResDictionary
	ResourceObject *prevResource,*nextResource;// �ļ������ǰһ���ڵ����ͺ�һ���ڵ����

public:
	enum Flags
	{
		VolumeBlock   = BIT(0),			// ���ļ�����
		File          = BIT(1),			// ��ͨ�����ļ�
		Added         = BIT(2),
	};
	S32 flags;

	StringTableEntry path;				// ��Դ�ļ�·��
	StringTableEntry name;				// ��Դ�ļ���

	ResourceInstance *mInstance;		// ��Դ�ļ��������Դδ���أ���ΪNULL��δ���壩
	S32  lockCount;						// ���ü���. ��һ���ļ�����δ򿪵�ʱ��, �����ļ�ϵͳ���Զ����ص�ǰ��handle��Ϊ���ü�����1.
	U32  crc;							// ��Դ�ļ���CRC32ֵ

	// -----------------------------------------------------
	// ZIPѹ���ĵ�����
	StringTableEntry zipPath;			// zipѹ�����ļ�����·��
	StringTableEntry zipName;			// zipѹ�����ļ���
	S32 fileOffset;						// ��Դ�ļ���zip���ڵ�ƫ��λ��
	S32 fileSize;						// ��Դ�ļ���С
	S32 compressedFileSize;				// ��Դ���ݵ���ʵ��С��������ѹ�����ֽڣ�				

#ifdef USE_TPK
	TPKArchive* mTPKArchive;			// TPK�ļ�������
	U32 BlockIndex;						// TPK�ļ����е�ǰ�ļ����ڵ�Block������ֵ	
#else
	Zip::ZipArchive *mZipArchive;		// zipѹ��������
	const Zip::CentralDir *mCentralDir;
#endif
	ResourceObject();
	~ResourceObject() { unlink(); }
	void destruct();

	// ------------------------------------------------------
	// �ļ�����Ĺ���
	ResourceObject* getNext() const { return next; }
	void unlink();
	void linkAfter(ResourceObject* res);

	// ------------------------------------------------------
	// ����

	// ��ȡ�ļ�����ʱ����޸�ʱ��
	void getFileTimes(FileTime *createTime, FileTime *modifyTime);
	// ��ȡ����·��
	const char* getFullPath();
};



//===============================================================================
//	���ڲ�����Դ�ļ�����Ĳ���ģ��(Resource).
//		���һ����Դ�ļ����󱻷���, ������Ĭ��Ϊ������. ���ü����ڸ��ļ����ٴδ򿪻�
//	������Դ�ļ����������趨Ϊ���ļ���ʱ���1. ��������:
//	@code
//	Resource<TerrainFile> terrRes;
//	terrRes = ResourceManager->load(fileName);
//		if(!bool(terrRes))
//			Con::errorf(ConsoleLogEntry::General, "Terraformer::terrainFile - invalid terrain file '%s'.", fileName);
//	@endcode
//	�����Resource<>���ͷ�ʱ�������ͷ�����,���ü�����1
//===============================================================================
template <class T> class Resource
{
private:
	ResourceObject *obj;

	// ***WARNING***
	// Using a faster lock that bypasses the resource manger.
	// void _lock() { if (obj) obj->rm->lockResource( obj ); }
	void _lock();		// ����Դ�ļ��������ü�����1
	void _unlock();     // ����Դ�ļ��������ü�����1

public:
	Resource() : obj(NULL) { ; }
	Resource(ResourceObject *p) : obj(p) { ; }
	Resource(const Resource &res) : obj(res.obj) { _lock(); }
	~Resource() { unlock(); }
	
	const char *getFilePath() const { return (obj ? obj->path : NULL); }	// ��ȡ·��
	const char *getFileName() const { return (obj ? obj->name : NULL); }	// ��ȡ�ļ���

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
//	�ļ�ӳ�����
//		ʹ�ù�������ݶ����ID������ӳ�䵽���󡣲��ṩ���ٵĶ���������ɾ��������
//	���౻�����ļ�ϵͳ�ڲ����ʣ�������Ա����Ҫֱ�ӷ��ʺͲ������ࡣ
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

   // �����Դ�ļ����ļ�ӳ�����
   void insert(ResourceObject *obj, StringTableEntry path, StringTableEntry file);

   // ������Դ�ļ�����
   //	����������ù�������������������Դ�Ѿ����أ��򷵻������Դ�ļ����󣬷��򷵻ؿ�
   ResourceObject* find(StringTableEntry path, StringTableEntry file);
   ResourceObject* find(StringTableEntry path, StringTableEntry file, StringTableEntry filePath, StringTableEntry fileName);
   ResourceObject* find(StringTableEntry path, StringTableEntry file, U32 flags);
   /// @}

   // �ƶ��ļ�ӳ�����ĳ��Դ�ļ������ض�ƥ���������Դ�ļ�����ĺ��棨��ͬ��һ���ض�����
   void pushBehind(ResourceObject *obj, S32 mask);

   // ���ļ�ӳ�����ȥ��ĳ��Դ�ļ�����
   void remove(ResourceObject *obj);
   // ��ȡ��ǰӳ����ļ����Ӷ���͹�������
   void getHash( ResourceObject ***hash, U32 *hashSize ){ *hash = hashTable; *hashSize = hashTableSize; }
};

//===============================================================================
//	�����ļ�ϵͳ������(ResManager)
//		�����ļ�ϵͳ�ж������Լ��ĸ�Ŀ¼. ���Ŀ¼λ������Ϸ��ִ���ļ����ڵ�·��. 
//	�����ļ�ϵͳ���ļ����Ĺ�������UNIX�ļ���Լ��, ͨ��б��(/)�������ָ�·��, ͬʱ,
//	�Ϸ��ļ�����Ҫ�������¹涨:
//		@li	��һ���ַ�������"/". ������Ҫ����CommonĿ¼�µ��ļ�sample.abc, ��ֱ��
//	����"Common/sample.abc"����, �����ڵ�һ����ĸǰ��б��.
//		@li	���ܰ���ð��(:)������Windows�ļ�ϵͳ��������ַ�, ��"C:/SomeFile.abc"
//	�ǷǷ��ļ���.
//
//		������C:\MyGameĿ¼�д������MyGame.exe�ļ�, ��Ҫ���������ļ�:
//		@li	C:\MyGame\textfile.txt
//		@li	C:\MyGame\Data\Images\myphoto.jpg
//		@li	C:\MyGame\Data\Actors\myself.mtj
//
//		ͨ�������ļ�ϵͳ������Щ�ļ�, ����Ҫ����������ļ����ļ����ֱ���:
//		@li	textfile.txt
//		@li	Data/Images/myphoto.jpg
//		@li	Data/Actors/myself.mtj
//
//		����Windows�е�Windows Explorer, �����ļ�ϵͳ������(ResManager)���������ļ�ϵͳ
//	�Ĺ��������ṩ�������ڹ��������ļ�ϵͳ�Ľӿ�. ResManager����Ҫ���ܰ���:
//		@li	������Դ�ļ��������ļ�ϵͳ�е�λ��.
//		@li	ɨ�����·������·���е�������Դ�ļ�
//		@li	���ļ�װ�����ڴ�.
//		@li	�Ѿ�װ�ؽ��ڴ���ļ������ٱ����浽����ļ�(�����Ҫ).
//		@li	ͨ����֤CRC��֤�ļ���������.
//
//		���һ�������Ϊ�ܱ�ResManager�������Դ��, ����:
//		@li	��ResourceInstance�̳�.
//		@li	ʵ��һ���ļ�������.
//		@li	��ص��ļ���׺��Ҫͨ��ResManager����ע��.
//
//		ResManager��һ����ʵ����, ��ȫ�ֽ����ڸ����һ��ʵ��ResourceManager
//===============================================================================

class CBackWorker;

class ResManager
{
private:
   char writeablePath[1024];					// д�����ݵ�Ŀ��·����һ�����ڴ�Server��������ʱд�뱾�ش���
   char primaryPath[1024];						// �ɼ������ݵ�Դ·��
   char* pathList;								// ��Ч����ϷĿ¼��

   ResourceObject timeoutList;					// ��ʱ�ļ��б�
   ResourceObject resourceList;					// ��Դ�ļ��б�

   ResDictionary dictionary;					// �ļ�ӳ���

   bool echoFileNames;							// �ж��Ƿ���ʾ�ļ���,�����ڵ���
   bool usingVFS;								// �Ƿ�ʹ�������ļ�

   U32              mTraverseHashIndex;			// ����ѭ��������Դ�ļ�������
   ResourceObject * mTraverseCurObj;			// ����ѭ��������Դ�ļ��Ķ���

   bool isIgnoredSubdirectoryName(const char *name) const;	// �Ƿ������Ŀ¼����

#ifdef USE_TPK
   bool scanTPK(ResourceObject* tpkObject);		// ɨ��tpk�ļ����ڵ�������Դ
#else
   bool scanZip(ResourceObject* zipObject);		// ɨ��zipѹ�����ڵ�������Դ
#endif
   
    ResourceObject* createResource(StringTableEntry path, StringTableEntry file);	// ������Դ�ļ�����
   // Ϊzipѹ��������ļ�������Դ�ļ�����
   ResourceObject* createZipResource(StringTableEntry path, StringTableEntry file, StringTableEntry zipPath, StringTableEntry zipFle);

   // ����Ŀ¼·��������zip�ļ��������ļ���������Դ�ļ�����
   void searchPath(const char *pathStart, bool noDups = false, bool ignoreZips = false);
   bool setModZip(const char* path);

   struct RegisteredExtension
   {
      StringTableEntry     mExtension;			// ��׺��, ��Ҫ������(.)
      RESOURCE_CREATE_FN   mCreateFn;			// �����ļ���������.
      RegisteredExtension  *next;				// ��������
   };

   Vector<char *> mMissingFileList;				// ��ʧ���ļ�����δ�����ļ�ӳ����ڲ��ҵ����ļ���
   bool mLoggingMissingFiles;					// �Ƿ�ǼǶ�ʧ���ļ�����ʧ�ļ��б���

   RegisteredExtension *registeredList;			// ע����Դ�ļ����б�	

   static char *smExcludedDirectories;			// ���ų����Ե�Ŀ¼�б�

   CBackWorker* m_pWorker;						// ���̵߳���
   ResManager();
public:
   RESOURCE_CREATE_FN getCreateFunction( const char *name );

   ~ResManager();

   static void create();
   static void destroy();

	// ��ʼ���ų���Ŀ¼
   static void initExcludedDirectories();

   

   void setFileNameEcho(bool on);                     // �����Ƿ���ʾ�ļ��������ڵ��ԣ�
   void setModPaths(U32 numPaths, const char **dirs); // ������Ϸ���ݺ�����·��
   const char* getModPaths();

   void addPath(const char *path, bool ignoreZips=false);// ���·��
   void removePath(const char *path);			// �Ƴ�·��

   void fileIsMissing(const char *fileName);          // �ļ���ʧʱ����
   void setMissingFileLogging(bool log);              // �����Ƿ��¼��ʧ���ļ�
   bool getMissingFileList(Vector<char *> &list);     // ��ȡ��ʧ���ļ��б�
   void clearMissingFileList();                       // �����ʧ���ļ��б�

   //����¼��Դ�ļ����صķ�ʽ
   void registerExtension(const char *extension, RESOURCE_CREATE_FN create_fn);

   S32 getSize(const char* filename);                 // ��ȡ�ļ���С
   const char* getFullPath(const char * filename, char * path, U32 pathLen);  // ��ȡ����·��
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

   // ��һ����Դ�ļ������������ã�������ü���Ϊ0�������Դ�ļ����󽫱����뵽��ʱ�ļ��б��У�������Դ�����
   void unlock( ResourceObject* );

   // ���һ����Դ�ļ�ʵ��
   bool add(const char* name, ResourceInstance *addInstance, bool extraLock = false);

   // ���ļ�ӳ�����������Դ�ļ����󣬿��Կ��Ǵ����ڶ�ʧ�ļ��б��е����
   ResourceObject* find(const char * fileName, bool noMissing = false);
   bool            HasLoaded(const char * fileName);
   // �����ȡ��Դ�ļ�ʵ��(ͨ����Դ�ļ������ļ�ӳ����м�����
   ResourceInstance* loadInstance(const char *fileName, bool computeCRC = false, bool noMissing = false);
   // �����ȡ��Դ�ļ�ʵ��(ͨ����Դ�ļ��������ļ�ӳ����м�����
   ResourceInstance* loadInstance(ResourceObject *object, bool computeCRC = false);

   // �ļ�ӳ�����������Դ�ļ�
   ResourceObject* find(const char * fileName, U32 flags);
   // ͨ��ƥ����ʽ����ѯһ����Դ�ļ�����
   ResourceObject* findMatch(const char *expression, const char **fn, ResourceObject *start = NULL);
   // ͨ�����ƥ����ʽ������ѯһ����Դ�ļ�������" "���ָ�
   ResourceObject* findMatchMultiExprs(const char *multiExpression, const char **fn, ResourceObject *start = NULL);

   void purge();                                      // ͨ����ʱ�б����������������Դ
   void purge( ResourceObject *obj );                 // ���һ����Դ
   void freeResource(ResourceObject *resObject);      // �ͷ�һ����Դ
   void serialize(VectorPtr<const char *> &filenames);// ������Դ�ļ�����

   S32  findMatches( FindMatch *pFM );                // ƥ���������ļ�
   bool findFile( const char *name );                 // ����ļ��Ƿ����

   bool addVFSRoot(Zip::ZipArchive *vfs);
   bool isUsingVFS() { return usingVFS; }

   // ͨ����ͬ��crcInitialVal��ʼֵ�����Լ��㲻ͬ�ļ���Crc32ֵ
   bool getCrc(const char * fileName, U32 & crcVal, const U32 crcInitialVal = INITIAL_CRC_VALUE );

   void setWriteablePath(const char *path);           // ���ÿ�д·��
   bool isValidWriteFileName(const char *fn);         // ����Ƿ���Ч�Ŀ�д·�����ļ���

   // ���ļ���д
   bool openFileForWrite(FileStream &fs, const char *fileName, U32 accessMode = 1);

   void startResourceTraverse();
   ResourceObject * getNextResource();

   ResourceObject* reload(const char * fileName, bool computeCRC = false); // ���¼�����Դ�ļ�����
   void reloadResources();

#ifdef POWER_DEBUG
   void dumpLoadedResources();                        // ��ӡ���м��ص���Դ�ļ���Ϣ������̨
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
