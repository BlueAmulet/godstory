#include "SimpleZip.h"
#include "ZipStruct.h"
#include "ZipHandler.h"
#include "ZipCommon.h"
#include <hash_map>
#include "zlib/zlib.h"
#include <assert.h>
#include "Crc.h"

class CSimpleZip
{ 
public:
    CSimpleZip(const char *pwd) : 
          zfis(0),
          writ(0),
          hasputcen(false),
          encwriting(false),
          encbuf(0),
          password(0),
          crc(0),
          encbufsize(0),
          m_isInitialEmpty(true),
          m_isCpyFromTmp(false),
          m_zipTarget(NULL),
          m_tmpTarget(NULL),
          m_curTarget(NULL),
          csize(0),
          m_isAdded(false),
          m_mode(E_NONE)
      {
          if (pwd != 0 && *pwd!=0) 
          {
              password=new char[strlen(pwd)+1]; 
              strcpy(password,pwd);
          }
      }

    ~CSimpleZip() 
    {
        if (encbuf!=0) 
            delete[] encbuf; 

        encbuf=0; 

        if (password!=0) 
            delete[] password; 

        password=0;

        for (TZipFileInfo *zfi = zfis; zfi != NULL;)
        { 
            TZipFileInfo *tmp = zfi->nxt;
            delete zfi;
            zfi = tmp;
        }

        zfis = NULL;
    }

    bool Open(ZIPTARGET& zipFile,std::vector<std::string>& excludeList);

    int GetZipItemCount(void);

    TZipFileInfo* GetZipItem(int index,ZipItemInfo& info);

    bool Add(const char* innerName,ZIPTARGET& fileAdded);

    bool UnzipItem(int index,ZIPTARGET& target);

    bool Add2ExcludeList(const char* name);

    bool Add2TempFile(void);

    bool Close();

    ZIPMODE GetZipMode(void) {return m_mode;}

    void SetZipMode(ZIPMODE mode) { m_mode = mode;}
private:
    bool __CopyZip2(ZIPTARGET& dest,ZIPTARGET& src);

    bool __IsInExcludeList(char* name);

    bool __ScanDirList(ZIPTARGET& src);

    bool __DelFile2Dir(ZIPTARGET& target,TZipFileInfo* pZip,ZipHandler* pHandler);

    unsigned int __Write(const char *buf,unsigned int size);

    unsigned __Read(ZIPTARGET& src,char *buf, unsigned size);

    bool __AddCentral(void);

    bool __Deflate(ZIPTARGET& src);

    stdext::hash_map<std::string,bool> m_excludeList;

    ZIPTARGET* m_zipTarget;  //打开的操作对象
    ZIPTARGET* m_tmpTarget;  //临时保存的操作对象
    ZIPTARGET* m_curTarget;  //当前操作的对象

    bool m_isInitialEmpty; //开始是否是空的ZIP文件
    bool m_isCpyFromTmp;     //是否从临时文件夹覆盖回来
    bool m_isAdded;
    ZIPMODE m_mode;

    // These variables say about the file we're writing into
    // We can write to pipe, file-by-handle, file-by-name, memory-to-memmapfile
    char *password;           // keep a copy of the password

    size_t writ;            // how far have we written. This is maintained by Add, not write(), to avoid confusion over seeks
    bool hasputcen;           // have we yet placed the central directory?
    bool encwriting;          // if true, then we'll encrypt stuff using 'keys' before we write it to disk
    unsigned long keys[3];    // keys are initialised inside Add()
    char *encbuf;             // if encrypting, then this is a temporary workspace for encrypting the data
    unsigned int encbufsize;  // (to be used and resized inside write(), and deleted in the destructor)
    //
    TZipFileInfo *zfis;       // each file gets added onto this list, for writing the table at the end; 

    ulg crc;                                 // crc is not set until close(). iwrit is cumulative

    size_t csize;                               // compressed size, set by the compression routines
    // and this is used by some of the compression routines
    char buf[16384];
};

namespace
{
#define BUFREADCOMMENT (0x400) //每次读入的comment的大小
#define BACKMEM_2_READ (BUFREADCOMMENT + 4)

    /************************************************************************/
    /* 获取ZIP的last central dir的位置                                                                     */
    /************************************************************************/
    uLong __SearchLastCentralDir(ZIPTARGET& target)
    { 
        size_t fSize = target.Size();

        //文件末尾可能的最大长度
        size_t uMaxBack = 0xffff; // maximum size of global comment    //comment的长度是2个字节

        if (uMaxBack > fSize) 
            uMaxBack = fSize;

        unsigned char *buf = (unsigned char*)malloc(BACKMEM_2_READ); //4 for central dir signature

        if (buf == NULL) 
            return 0xFFFFFFFF;

        size_t uPosFound = 0xFFFFFFFF;   //返回的位置
        size_t uBackRead = 4;            //signature size,cant smaller than that,leaest size to read in

        while (uBackRead < uMaxBack)
        { 
            size_t uReadSize,uReadPos ;

            if (uBackRead + BUFREADCOMMENT > uMaxBack)  //假如读入的已经最大
                uBackRead = uMaxBack;
            else 
                uBackRead += BUFREADCOMMENT;

            uReadPos  = fSize - uBackRead; //文件末的读入位置
            uReadSize = (BACKMEM_2_READ < (fSize - uReadPos)) ? BACKMEM_2_READ : (fSize-uReadPos);    //最大能够读入的大小

            target.SetPos(uReadPos);

            uReadSize = target.Read((char*)buf,uReadSize);

            for (int i = (int)uReadSize-3; (i--)>=0;)
            { 
                if (((*(buf+i))==0x50) && ((*(buf+i+1))==0x4b) &&	((*(buf+i+2))==0x05) && ((*(buf+i+3))==0x06))   //逐个分析是否是end of central directory
                { 
                    uPosFound = uReadPos+i;	
                    break;
                }
            }

            if (uPosFound!=0) 
                break;
        }

        if (buf) 
            free(buf);

        return uPosFound;
    }

   /* void update_keys(unsigned long *keys, char c)
    { 
        keys[0] = crc32(keys[0],(uch*)&c,1);
        keys[1] += keys[0] & 0xFF;
        keys[1] = keys[1]*134775813L +1;
        unsigned long tmp = keys[1] >> 24;
        keys[2] = crc32(keys[2],(Bytef *)&tmp,sizeof(unsigned long));
    }*/

    void update_keys(unsigned long *keys, char c)
    { 
        keys[0] = CRC32(keys[0],c);
        keys[1] += keys[0] & 0xFF;
        keys[1] = keys[1]*134775813L +1;
        keys[2] = CRC32(keys[2], keys[1] >> 24);
    }

    char decrypt_byte(unsigned long *keys)
    { 
        unsigned temp = ((unsigned)keys[2] & 0xffff) | 2;
        return (char)(((temp * (temp ^ 1)) >> 8) & 0xff);
    }

    char zencode(unsigned long *keys, char c)
    { 
        int t=decrypt_byte(keys);
        update_keys(keys,c);
        return (char)(t^c);
    }

    void Uupdate_keys(unsigned long *keys, char c)
    { 
        keys[0] = CRC32(keys[0],c);
        keys[1] += keys[0] & 0xFF;
        keys[1] = keys[1]*134775813L +1;
        unsigned long tmp = keys[1] >> 24;
        keys[2] = CRC32(keys[2],keys[1] >> 24);
    }

    char Udecrypt_byte(unsigned long *keys)
    { 
        unsigned temp = ((unsigned)keys[2] & 0xffff) | 2;
        return (char)(((temp * (temp ^ 1)) >> 8) & 0xff);
    }

    char zdecode(unsigned long *keys, char c)
    { 
        c^=Udecrypt_byte(keys);
        Uupdate_keys(keys,c);
        return c;
    }

    FILETIME dosdatetime2filetime(WORD dosdate,WORD dostime)
    { 
        // date: bits 0-4 are day of month 1-31. Bits 5-8 are month 1..12. Bits 9-15 are year-1980
        // time: bits 0-4 are seconds/2, bits 5-10 are minute 0..59. Bits 11-15 are hour 0..23
        SYSTEMTIME st;
        st.wYear = (WORD)(((dosdate>>9)&0x7f) + 1980);
        st.wMonth = (WORD)((dosdate>>5)&0xf);
        st.wDay = (WORD)(dosdate&0x1f);
        st.wHour = (WORD)((dostime>>11)&0x1f);
        st.wMinute = (WORD)((dostime>>5)&0x3f);
        st.wSecond = (WORD)((dostime&0x1f)*2);
        st.wMilliseconds = 0;
        FILETIME ft; SystemTimeToFileTime(&st,&ft);
        return ft;
    }

}

/************************************************************************/
/* 遍历ZIP文件,产生目录列表                                                                     */
/************************************************************************/
bool CSimpleZip::__ScanDirList(ZIPTARGET& src)
{
    //获取last central dir的位置
    size_t lastCentralDir = __SearchLastCentralDir(src);

    //获取起始central dir的位置
    if (lastCentralDir == 0xFFFFFFFF) 
        return false;

    src.SetPos(lastCentralDir);    

    //读取尾目录信息
    CEndCentralDir endDir;
    endDir.Read(src);

    //遍历需要拷贝的文件
    src.SetPos(endDir.dirStart);

    for (int i = 0; i < endDir.nEntry; i++)
    {
        TZipFileInfo *pzfi = new TZipFileInfo;

        pzfi->Read(src);

        //判断需要拷贝的文件是否在排除列表里面
        if (__IsInExcludeList(pzfi->name))
        {
            delete pzfi;
            continue;
        }

        //跳过目录
        if (pzfi->att & FILE_ATTRIBUTE_DIRECTORY || pzfi->name[pzfi->nam - 1] == '/')
        {
            delete pzfi;
            continue;
        }

        if (zfis == NULL) 
            zfis = pzfi;
        else
        {
            TZipFileInfo *z = zfis; //push back

            while (z->nxt != NULL) 
                z=z->nxt; 

            z->nxt = pzfi;
        }
    }

    return true;
}

/************************************************************************/
/* 打开当前的ZIP文件                                                                     */
/************************************************************************/
bool CSimpleZip::Open(ZIPTARGET& zipFile,std::vector<std::string>& excludeList)
{ 
    if (!zipFile.IsValid())
        return false;

    m_zipTarget      = &zipFile;
    m_curTarget      = m_zipTarget;
    m_isInitialEmpty = true; //开始默认为打开空的ZIP文件
    m_isCpyFromTmp   = false;

    for (size_t i = 0; i < excludeList.size(); i++)
    {
        m_excludeList.insert(std::make_pair(strupr((char*)excludeList[i].c_str()),true));
    }

    size_t fSize = zipFile.Size();

    if (fSize > 0)
    {
        if (!__ScanDirList(zipFile))
            return false; //maybe not a valid zip file

        m_isInitialEmpty = false;
    }

    return true;
}

int CSimpleZip::GetZipItemCount(void)
{
    int n = 0;

    for (TZipFileInfo *zfi = zfis; zfi != NULL;zfi = zfi->nxt)
    { 
        n++;
    }

    return n;
}

TZipFileInfo* CSimpleZip::GetZipItem(int index, ZipItemInfo& info)
{
    int n = 0;

    for (TZipFileInfo *zfi = zfis; zfi != NULL;n++)
    { 
        if (index == n)
        {
            info.name = zfi->name;
            info.size = zfi->len;
            return zfi;
        }

        zfi = zfi->nxt;
    }

    return NULL;
}

/************************************************************************/
/* 1个简单的类,实现zip文件间的拷贝                                                                     */
/************************************************************************/
class CZipCpyHandler:
    public ZipHandler
{
public:
    CZipCpyHandler(ZIPTARGET& dest):m_dest(dest) {}

    //method from ZipHandler
    virtual bool OnHeader(CLocalHeader& header)
    {
        return header.Write(m_dest);
    }

    virtual bool OnPswd(char* buf,size_t len) //callback if the file is encrypt
    {
        if (12 != len) //password always goes 12.
            return false;

        return m_dest.Write(buf,len) == 12;
    }

    virtual bool OnBody(char* pBody,size_t len) 
    {
        return m_dest.Write(pBody,len) == len;
    }

    virtual bool OnExtention(char* buf,size_t len)
    {
        if (16 != len)
            return false;

        return m_dest.Write(buf,len) == 16;
    }
private:
    ZIPTARGET& m_dest; //目标文件
};

/************************************************************************/
/* 处理当前ZIP目录对应的内容                                                                     */
/************************************************************************/
bool CSimpleZip::__DelFile2Dir(ZIPTARGET& target,TZipFileInfo* pZip,ZipHandler* pHandler)
{
    if (NULL == pZip || NULL == pHandler)
        return false;

    target.SetPos(pZip->off);

    if (!pHandler->Begin())
        return false;

    //zip header
    CLocalHeader localHeader;
    localHeader.Read(target);

    if (!pHandler->OnHeader(localHeader))
    {
        pHandler->End();
        return false;
    }

    //判断文件是否加密
    if(pZip->flg & 1)
    {
        static char pswd[12] ={0};
        size_t dwReaded = target.Read(pswd,12);
        
        if (!pHandler->OnPswd(pswd,dwReaded))
        {
            pHandler->End();
            return false;
        }
    }

    //zip body,读取压缩后的大小
    size_t byte2Read   = CHUNK;
    size_t byteReaded  = 0;
    size_t compareSize = (pZip->flg & 1) ? pZip->siz - 12 : pZip->siz;

    static char buf[CHUNK] = {0};

    while(byteReaded != compareSize)
    {
        if (byte2Read + byteReaded > compareSize)
        {
            byte2Read = compareSize - byteReaded;
        }

        size_t dwReaded = target.Read(buf,byte2Read);
        byteReaded += dwReaded;

        if (!pHandler->OnBody(buf,dwReaded))
        {
            pHandler->End();
            return false;
        }
    }

    if (pZip->flg & 1)
    {
        static char extend[16] ={0};
        DWORD dwReaded = target.Read(extend,16);

        if (!pHandler->OnExtention(extend,dwReaded))
        {
            pHandler->End();
            return false;
        }
    }

    return pHandler->End();
}

/************************************************************************/
/* 把ZIP的信息拷贝到hDest文件                                                                     */
/************************************************************************/
bool CSimpleZip::__CopyZip2(ZIPTARGET& dest,ZIPTARGET& src)
{
    //把当前列表里面对应的ZIP内容拷贝到hDest
    TZipFileInfo* pZip = zfis;
    CZipCpyHandler handler(dest);

    while(NULL != pZip)
    {
        if (!(pZip->flg & FILE_ATTRIBUTE_DIRECTORY))
        {
            size_t fileSize = dest.Size();

            //非文件夹
            if (!__DelFile2Dir(src,pZip,&handler))
                return false;

            //修改pZip的local header偏移
            pZip->off = (ulg)fileSize;
        }

        pZip = pZip->nxt;
    }

    return true;
}

static const char* TMPFILENAME = "tianchang008";
static unsigned int   count = 1;
/************************************************************************/
/* 把当前的ZIP文件拷贝到临时文件夹,并改为对临时文件夹操作                                                                     */
/************************************************************************/
bool CSimpleZip::Add2TempFile(void)
{
    //假如文件大小不为0,则转为对临时文件进行操作,然后拷贝过来
    DWORD fSize = m_zipTarget->Size();

    if (0 != fSize)
    {
        //在临时目录下创建1个写入文件
        TCHAR dir[MAX_PATH] = {0};
        ::GetTempPath(MAX_PATH, dir);

        sprintf(dir,"%s\\%s_%d.tmp",dir,TMPFILENAME,count++);

        //创建临时对象
        if (NULL != m_tmpTarget)
            delete m_tmpTarget;

        m_tmpTarget = new ZIPTARGET((const char*)dir,true);
        
        if (!m_tmpTarget->IsValid())
            return false;

        //把当前ZIP文件的local header拷贝到临时文件,并保存相应的central dir
        if (!__CopyZip2(*m_tmpTarget,*m_zipTarget))
            return false;

        writ = m_tmpTarget->Size();
        //后续的操作改成临时文件的操作,并在结束后把相应的文件覆盖回来
        m_curTarget = m_tmpTarget;
    }

    return true;
}

/************************************************************************/
/* 对文件做压缩操作                                                                     */
/************************************************************************/
bool CSimpleZip::__Deflate(ZIPTARGET& src)
{
    z_stream strm;
    ZeroMemory(&strm,sizeof(strm));

    //strm.avail_out = buf;

    strm.zalloc = Z_NULL;
    strm.opaque = Z_NULL;
    strm.zfree  = Z_NULL;

    //对于在内存上操作deflate的,需要调用deflateInit2
    if (Z_OK != deflateInit2(&strm,Z_BEST_COMPRESSION,Z_DEFLATED,-MAX_WBITS,DEFAULT_MEM,Z_DEFAULT_STRATEGY))
        return false;

    int flushMode     = Z_NO_FLUSH;
    size_t fileSize    = src.Size();
    size_t totalReaded = 0;
    csize  = 0;

    do
    {
         strm.avail_in = __Read(src,buf,CHUNK);

        totalReaded += strm.avail_in;

        //读入失败
        if (strm.avail_in <= 0)
        {
            deflateEnd(&strm);
            return false;
        }

        //是否已经到文件末尾
        if(fileSize == totalReaded)
        {
            flushMode = Z_FINISH;
        }

        strm.next_in = (Bytef*)buf;

        static char tmpOut[CHUNK] = {0};

        do
        {
            strm.next_out  = (Bytef*)tmpOut;
            strm.avail_out = CHUNK;

            deflate(&strm,flushMode);

            unsigned int writtens = CHUNK - strm.avail_out;

            unsigned int cout = __Write(tmpOut,writtens); 

            if (cout != writtens)   //写文件失败
            {
                deflateEnd(&strm);
                return false;
            }

            csize += cout;
        }
        while(strm.avail_out == 0); //无法输出

    }
    while(flushMode != Z_FINISH);


    deflateEnd(&strm);

    return true;
}

#define DEFLATE 8               // Deflation method

static bool has_seeded = false;

/************************************************************************/
/* 往ZIP里面添加文件                                                     */
/************************************************************************/
bool CSimpleZip::Add(const char* innerName,ZIPTARGET& fileAdded)
{ 
    crc = 0;

    if (!m_isInitialEmpty && !m_isCpyFromTmp)
    {
        //把原有文件里面的东西拷贝到临时文件夹
        if (!Add2TempFile())
            return false;

        m_isCpyFromTmp = true;
    }

    char* ptr = const_cast<char*>(innerName); 

    //把目录里面"\\"转换成"/"
    while (*ptr != 0) 
    {
        if (*ptr == '\\') 
            *ptr='/';

        ptr++;
    }

    //往ZIP里面填写相应的local head和对应的data数据
    TZipFileInfo zfi; 

    strncpy(zfi.name,innerName,MAX_PATH); 
    zfi.name[MAX_PATH-1]=0;

    zfi.nam = (ush)strlen(zfi.name);

    zfi.vem         = (ush)0xB17; // 0xB00 is win32 os-code. 0x17 is 23 in decimal: zip 2.3
    zfi.ver         = (ush)20;    // Needs PKUNZIP 2.0 to unzip it

    int timestamp = 0;
    int attri     = 0;

    fileAdded.GetInfo(attri,timestamp); //获取文件的属性和时间信息
    fileAdded.SetPos(0);

    zfi.tim = timestamp;
    
    if (password != 0)
        zfi.flg = 9;  // and 1 means 'password-encrypted' ...似乎1定要9...

    zfi.lflg = zfi.flg;     // to be updated later
    zfi.how  = DEFLATE;
    zfi.len  = (ulg)fileAdded.Size();  // to be updated later
    zfi.dsk  = 0;
    zfi.atx  = attri;
    zfi.off  = (ulg)writ;         // offset within file of the start of this local record

    // (1) Start by writing the local header:
    CLocalHeader localHeader;

    localHeader.signature = 0x04034b50;
    localHeader.version   = zfi.ver;
    localHeader.flag      = static_cast<ush>(zfi.lflg);
    localHeader.cmethod   = zfi.how;
    localHeader.time      = zfi.tim;
    localHeader.crc       = zfi.crc;
    localHeader.csize     = zfi.siz;
    localHeader.ucsize    = zfi.len;
    localHeader.namelen   = zfi.nam;
    localHeader.extralen  = zfi.ext;

    if(zfi.nam)
    {
        if (NULL != localHeader.name)
            delete []localHeader.name;

        localHeader.name = new char[zfi.nam + 1];
        strcpy(localHeader.name,zfi.name);
    }

    if(zfi.ext)
    {
        if (NULL != localHeader.extra)
            delete []localHeader.extra;

        localHeader.extra = new char[zfi.ext + 1];
        strcpy(localHeader.extra,zfi.extra);
    }

    if(!localHeader.Write(*m_curTarget))
        return false;

    writ += 4 + LOCHEAD + (unsigned int)zfi.nam + (unsigned int)zfi.ext;

    // (1.5) if necessary, write the encryption header
    keys[0]=305419896L;
    keys[1]=591751049L;
    keys[2]=878082192L;

    for (const char *cp=password; cp!=0 && *cp!=0; cp++) 
        update_keys(keys,*cp);

    // generate some random bytes
    if (!has_seeded) 
        srand(GetTickCount()^(unsigned long)GetDesktopWindow());

    char encbuf[12]; 

    for (int i = 0; i < 12; i++) 
        encbuf[i] = (char)((rand()>>7)&0xff);

    encbuf[11] = (char)((zfi.tim>>8)&0xff); //这个在解密时用来做验证.加密是对称的.

    for (int ei = 0; ei < 12; ei++) 
        encbuf[ei] = zencode(keys,encbuf[ei]);

    if (password != 0) 
    {
        m_curTarget->Write(encbuf,12);  //追加密码
        writ += 12;
    }

    //(2) Write deflated/stored file to zip file
    encwriting = (password != 0);// an object member variable to say whether we write to disk encrypted

    if(!__Deflate(fileAdded))
    {
        if (fileAdded.Size() != 0) //允许添加的文件为空
            return false;
        else
        {
            csize = 0;
        }
    }

    encwriting = false;
    writ      += csize;

    // (3) Either rewrite the local header with correct information...

    int passex = (password != 0) ? 12 : 0; 
    
    zfi.crc = crc;
    zfi.siz = csize + passex;
    zfi.len = fileAdded.Size();

    //追加扩展信息
    ulg signatureExtend = 0x08074b50L;

    m_curTarget->Write(signatureExtend);
    m_curTarget->Write(zfi.crc);
    m_curTarget->Write(zfi.siz);
    m_curTarget->Write(zfi.len);

    writ += 16;

    localHeader.csize  = zfi.siz;
    localHeader.crc    = zfi.crc;
    localHeader.ucsize = zfi.len;

    m_curTarget->SetPos(zfi.off);

    if(!localHeader.Write(*m_curTarget))
        return false;

    m_curTarget->SetPos(writ); 

    TZipFileInfo *pzfi = new TZipFileInfo; 
    memcpy(pzfi,&zfi,sizeof(zfi));

    if (zfis==NULL) 
        zfis=pzfi;
    else
    {
        TZipFileInfo *z=zfis; 

        while (z->nxt!=NULL) 
            z=z->nxt; 

        z->nxt=pzfi;
    }

    fileAdded.SetCmpSize(zfi.siz);
    m_isAdded = true;
    return true;
}

/************************************************************************/
/* 1个简单的类,实现zip文件的解压缩
/************************************************************************/
class CUnzipHandler:
    public ZipHandler
{
public:
    CUnzipHandler(ZIPTARGET& dest,TZipFileInfo* pZipFile,std::string pswd):m_dest(dest) , m_pZipFile(pZipFile) ,m_password(pswd) {}

    //method from ZipHandler
    virtual bool Begin(void) 
    { 
        ZeroMemory(&strm,sizeof(strm));

        if (Z_OK != inflateInit2(&strm,-MAX_WBITS))
            return false;

        return true;
    }

    virtual bool End(void) 
    {
        //比较文件的CRC值
        /*if (m_pZipFile->crc != m_crc)
            return false;*/

        inflateEnd(&strm);
        return true;
    }

    virtual bool OnPswd(char* buf,size_t len) //callback if the file is encrypt
    {
        if (12 != len) //password always goes 12.
            return false;

        //验证密码是否正确
        keys[0]=305419896L;
        keys[1]=591751049L;
        keys[2]=878082192L;

        for (const char *cp = m_password.c_str(); cp != 0 && *cp != 0; cp++) 
            update_keys(keys,*cp);

        for (unsigned int i=0; i< len; i++) 
            buf[i] = zdecode(keys,buf[i]);
        
        char test = (char)((m_pZipFile->tim>>8)&0xff);  //明码

        if (test != buf[11])
            return false;

        return true;
    }

    virtual bool OnBody(char* pBody,size_t len) 
    {
        if (NULL == pBody || 0 == len)
        {
            return false;
        }

        //对文件进行解密
        if ("" != m_password)
        { 
            for (int i = 0; i < len; i++) 
            {
                pBody[i] = zdecode(keys,pBody[i]);
            }
        }

        if (m_pZipFile->how == 0)
        {
            //普通的存储
            return len == m_dest.Write(pBody,len);
        }
        else
        {
            strm.next_in  = (Bytef*)pBody;
            strm.avail_in = len;

            static char tmpOut[CHUNK] = {0};
            unsigned have = 0;  //读入的数据量

            do
            {
                strm.next_out  = (Bytef*)tmpOut;
                strm.avail_out = CHUNK;

                int ret = inflate(&strm, Z_SYNC_FLUSH);

                switch (ret) 
                {
                case Z_NEED_DICT:
                case Z_DATA_ERROR:
                case Z_MEM_ERROR:
                    {
                        return false;
                    }
                }

                have = CHUNK - strm.avail_out;

                //计算CRC并写入相应的文件
                if (have != m_dest.Write(tmpOut,have))
                {
                    return false;
                }

                //m_crc = crc32(m_crc, (uch*)tmpOut,have);
            }
            while(strm.avail_out == 0);
        }
        
        return true;
    }
private:
    ZIPTARGET&    m_dest;       //目标文件
    TZipFileInfo* m_pZipFile;   //当前要解压的文件信息
    std::string   m_password;
    unsigned long keys[3];
    ulg           m_crc;        //保存解压后内容的CRC值和zipfileinfo里面的CRC做比较
    z_stream      strm;
};

/************************************************************************/
/* 解压莫个文件                                                                     */
/************************************************************************/
bool CSimpleZip::UnzipItem(int index,ZIPTARGET& target)
{
    ZipItemInfo info;
    TZipFileInfo* pFileInfo = GetZipItem(index,info);

    if (NULL == pFileInfo)
        return false;

    CUnzipHandler handler(target,pFileInfo,password ? password : "");

    if (!__DelFile2Dir(*m_curTarget,pFileInfo,&handler))
        return false;

    //关闭target
    target.Close();

    return true;
}

/************************************************************************/
/* 关闭当前的ZIP文件                                                                     */
/************************************************************************/
bool CSimpleZip::Close()
{ 
    bool rslt = true;

    if (!hasputcen && m_isAdded) 
        rslt = __AddCentral();

    if(!rslt)
        return false;

    if (m_curTarget)
        m_curTarget->Close();

    //检测是否需要从临时文件夹拷贝回来
    if(m_isCpyFromTmp && m_curTarget && m_zipTarget)
    {
        m_zipTarget->Close();
        *m_zipTarget = *m_curTarget;

        //删除临时文件
        ::DeleteFile(m_curTarget->GetPath());

        if(NULL != m_tmpTarget)
            delete m_tmpTarget;

        m_tmpTarget = NULL;
    }

    return true;
}

unsigned int CSimpleZip::__Write(const char *buf,unsigned int size)
{
    const char *srcbuf = buf;

    //对当前块进行加密
    if (encwriting)
    { 
        if (encbuf != 0 && encbufsize < size) 
        {
            delete[] encbuf; 
            encbuf = 0;
        }

        if (encbuf == 0) 
        {
            encbuf = new char[size*2]; 
            encbufsize = size;
        }

        memcpy(encbuf,buf,size);

        for (unsigned int i=0; i<size; i++) 
            encbuf[i] = zencode(keys,encbuf[i]);

        srcbuf = encbuf;
    }

    return m_curTarget->Write((char*)srcbuf,size);
}

unsigned CSimpleZip::__Read(ZIPTARGET& src,char *buf, unsigned size)
{
    int readed = src.Read(buf,size);
    crc = crc32(crc, (uch*)buf, readed);

    return readed;
}

bool CSimpleZip::__AddCentral(void)
{ 
    // write central directory
    int numentries = 0;
    ulg pos_at_start_of_central = writ;

    for (TZipFileInfo *zfi = zfis; zfi!=NULL; )
    { 
        if (!zfi->Write(*m_curTarget))
            return false;

        writ += 4 + CENHEAD + (unsigned int)zfi->nam + (unsigned int)zfi->cext + (unsigned int)zfi->com;

        numentries++;
        //
        TZipFileInfo *zfinext = zfi->nxt;

        delete zfi;
        zfi = zfinext;
    }

    zfis = NULL;

    ulg center_size = writ - pos_at_start_of_central;

    CEndCentralDir endDir;

    endDir.nEntry      = numentries;
    endDir.nEntryOnDir = numentries;
    endDir.size        = center_size;
    endDir.dirStart    = pos_at_start_of_central;

    if(!endDir.Write(*m_curTarget))
        return false;

    writ += 4 + ENDHEAD;

    return true;
}

bool CSimpleZip::__IsInExcludeList(char* name)
{
    std::string tmp = name;
    return m_excludeList.end() != m_excludeList.find(strupr((char*)tmp.c_str()));
}

HZIP OpenZip(ZIPTARGET& zipFile,char* pswd,std::vector<std::string>& excludeList)
{
    if (0 == zipFile.Size())
        return NULL;

    CSimpleZip *pZip  = new CSimpleZip(pswd);

    if(!pZip->Open(zipFile,excludeList))
    {
        delete pZip; 
        return NULL;
    }

    return (HZIP)pZip;
}

int GetZipItemCount(HZIP hZip)
{
    if (NULL == hZip)
        return 0;

    return ((CSimpleZip*)hZip)->GetZipItemCount();
}

bool GetZipItem(HZIP hZip, int index, ZipItemInfo& info)
{
    if (NULL == hZip || index < 0)
        return false;

    return NULL != ((CSimpleZip*)hZip)->GetZipItem(index,info);
}

bool ZipAdd(HZIP hZip,const char* innerName,ZIPTARGET& added)
{
    if (NULL == hZip || NULL == innerName || strlen(innerName) == 0)
    {
        return false;
    }

    CSimpleZip* pZip = (CSimpleZip*)hZip;

    assert(pZip->GetZipMode() != E_UNZIP);

    if (pZip->GetZipMode() == E_UNZIP)
        return false;

    pZip->SetZipMode(E_ZIP);

    //去掉最前面的"/"或"\\"
    if (innerName[0] == '/' || innerName[0] == '\\')
        innerName++;

    return pZip->Add(innerName,added);
}

bool UnzipItem(HZIP hZip,int index,ZIPTARGET& target)
{
    if (NULL == hZip || index < 0)
        return false;

    CSimpleZip* pZip = (CSimpleZip*)hZip;

    assert(pZip->GetZipMode() != E_ZIP);

    if (pZip->GetZipMode() == E_ZIP)
        return false;

    pZip->SetZipMode(E_UNZIP);

    return pZip->UnzipItem(index,target);
}

void CloseZip(HZIP hZip)
{
    if (NULL == hZip)
        return;

    CSimpleZip *pZip = (CSimpleZip*)hZip;
    pZip->Close();

    delete pZip;
}