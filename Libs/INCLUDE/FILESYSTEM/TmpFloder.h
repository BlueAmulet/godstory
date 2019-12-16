#ifndef _TMPFLODER_H_
#define _TMPFLODER_H_

#include <string>
#include <hash_map>

/************************************************************************/
/* 封装对临时文件夹的操作
/************************************************************************/
class CTmpFloder
{
public:
    static std::string Create(const char* pathName,bool isAutoDel = true);
};

#endif /*_TMPFLODER_H_*/