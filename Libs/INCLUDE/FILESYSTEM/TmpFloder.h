#ifndef _TMPFLODER_H_
#define _TMPFLODER_H_

#include <string>
#include <hash_map>

/************************************************************************/
/* ��װ����ʱ�ļ��еĲ���
/************************************************************************/
class CTmpFloder
{
public:
    static std::string Create(const char* pathName,bool isAutoDel = true);
};

#endif /*_TMPFLODER_H_*/