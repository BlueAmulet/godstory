#ifndef _ZIPSTRUCT_H_
#define _ZIPSTRUCT_H_

//to define some common zip struct.

#include "ZipCommon.h"

/************************************************************************/
/* ZIP头结构                                                                     */
/************************************************************************/
struct CLocalHeader
{
    CLocalHeader(void)
    {
        ZeroMemory(this,sizeof(CLocalHeader));
    }

    ~CLocalHeader(void)
    {
        if (NULL != name)
            delete []name;

        if (NULL != extra)
            delete []extra;

        name  = NULL;
        extra = NULL;
    };
    //
    bool Read(ZIPTARGET& hFile);

    bool Write(ZIPTARGET& hFile);

    ulg signature;      //local file header signature

    ush version;        //version needed to extract
    ush flag;           //general purpose bit flag
    ush cmethod;        //compression method

    ulg time;           
    ulg crc;            //crc-32
    ulg csize;          //compressed size
    ulg ucsize;         //uncompressed size

    ush namelen;        //file name length
    ush extralen;       //extra field length

    char* name;
    char* extra;
};

/************************************************************************/
/* ZIP的目录列表                                                         */
/************************************************************************/
typedef struct zlist 
{
    zlist(void)
    {
        ZeroMemory(this,sizeof(zlist));
    }

    ~zlist(void)
    {
        if (NULL != extra)
            delete []extra;

        extra = NULL;

        if (NULL != cextra)
            delete []cextra;

        cextra = NULL;

        if (NULL != comment)
            delete []comment;

        comment = NULL;
    };
    //
    bool Read(ZIPTARGET& hFile);

    bool Write(ZIPTARGET& hFile);

    ulg signature;

    ush vem;        //version made by 
    ush ver;        //version needed to extract
    ush flg;        //general purpose bit flag
    ush how;        //compression method

    ulg tim;        //time
    ulg crc;        //crc-32 
    ulg siz;        //compressed size
    ulg len;        //uncompressed size

    ush nam;        //file name length
    ush ext;        //用在local file里面的comment长度
    ush cext;       //extra field length
    ush com;        //file comment length

    ush dsk;        //disk number start
    ush att;        //internal file attributes
    ulg lflg;       //用在local file里面的flg

    ulg atx;        //external file attributes
    ulg off;        //relative offset of local header

    char name[MAX_PATH];          // File name in zip file
    char *extra;                  // Extra field (set only if ext != 0)
    char *cextra;                 // Extra in central (set only if cext != 0)
    char *comment;                // Comment (set only if com != 0)

    zlist *nxt;        // Pointer to next header in list
}
TZipFileInfo;

/************************************************************************/
/* ZIP目录尾结构                                                        */
/************************************************************************/
struct CEndCentralDir
{
    CEndCentralDir(void)
    {
        ZeroMemory(this,sizeof(CEndCentralDir));
    }

    ~CEndCentralDir(void)
    {
        if (NULL != comment)
            delete []comment;

        comment = NULL;
    };
    //
    bool Read(ZIPTARGET& hFile);

    bool Write(ZIPTARGET& hFile);

    ulg signature;       //end of central dir signature
    ush nDisk;           //number of this disk
    ush nDiskDir;        //number of the disk with the start of the central directory
    ush nEntryOnDir;     //total number of entries in the central directory on this disk
    ush nEntry;          //total number of entries in the central directory
    ulg size;            //size of the central directory offset of start of central
    ulg dirStart;       //the starting disk number

    ush commentLen;      //ZIP file comment length
    char* comment;
};

#endif /*_ZIPSTRUCT_H_*/