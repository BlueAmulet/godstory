#ifndef _FSUTILS_H_
#define _FSUTILS_H_

#include <string>
#include <vector>

/************************************************************************/
/* �ļ�ϵͳ�õ���1Щ�����ĺ���                                          */
/************************************************************************/

//�ַ���ģ��ƥ��
extern bool StringMatch(const std::string& str, const std::string& pattern, bool caseSensitive);

//�Ƿ��Ǳ�����Ŀ¼
extern bool is_reserved_dir(const char *fn);

extern bool is_exist(const char* filename);

//�Ƿ����ļ���
extern bool IsFloder(const char* filename);

extern std::string& ToLowerCase(std::string& str);

//��fullname�з���·�����ļ���
extern void GetPathAndName(const char* fullName,std::string& path,std::string& fileName);

//��ʾ�ļ�����Ŀ¼�Ի���
extern std::string GetSelectedFolder(const char* desc = NULL);

//�ָ��ַ���
extern std::vector<std::string> SplitString(std::string str,char splitter);

//��ȡ�ļ���С
extern size_t GetFileSize(const char* file);

//����ʱ�ļ����´����ļ�
extern std::string CreateTempFile(const char* name,bool isClear = true);

//��ȡ��ʱ�ļ���
extern std::string GetTempFloder(void);

//ѡ��Ŀ¼�µ��ļ�
extern std::string GetSelectedFile(void);

//ɾ��Ŀ¼
extern bool DelDir(const char* dir);

//ɾ��Ŀ¼�µ������ļ�
extern bool DelFileUnderDir(const char* dir);

//��ȡ�ļ���չ��
extern std::string GetFileExt(std::string fileName);

//�Ƿ��Ǿ���·��
extern bool IsAbsolutlyPath(std::string path);

//��ȡ·���а������ļ���
extern std::string GetFileName(const char* path);

//��֤�����ļ���·���Ǵ��ڵ�
extern bool EnsureFloder(const char* filePath);

//��Ŀ¼�е�"\\"�滻��"/"
extern void ReplaceSlash(char* pBuffer,size_t len);

extern void ReplaceSlash(std::string& buffer);

#endif /*_FSUTILS_H_*/