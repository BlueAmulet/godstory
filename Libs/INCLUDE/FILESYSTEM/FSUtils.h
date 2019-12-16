#ifndef _FSUTILS_H_
#define _FSUTILS_H_

#include <string>
#include <vector>

/************************************************************************/
/* 文件系统用到的1些公共的函数                                          */
/************************************************************************/

//字符串模糊匹配
extern bool StringMatch(const std::string& str, const std::string& pattern, bool caseSensitive);

//是否是保留的目录
extern bool is_reserved_dir(const char *fn);

extern bool is_exist(const char* filename);

//是否是文件夹
extern bool IsFloder(const char* filename);

extern std::string& ToLowerCase(std::string& str);

//从fullname中分离路径和文件名
extern void GetPathAndName(const char* fullName,std::string& path,std::string& fileName);

//显示文件查找目录对话框
extern std::string GetSelectedFolder(const char* desc = NULL);

//分割字符串
extern std::vector<std::string> SplitString(std::string str,char splitter);

//获取文件大小
extern size_t GetFileSize(const char* file);

//在临时文件夹下创建文件
extern std::string CreateTempFile(const char* name,bool isClear = true);

//获取临时文件夹
extern std::string GetTempFloder(void);

//选择目录下的文件
extern std::string GetSelectedFile(void);

//删除目录
extern bool DelDir(const char* dir);

//删除目录下的所有文件
extern bool DelFileUnderDir(const char* dir);

//获取文件扩展名
extern std::string GetFileExt(std::string fileName);

//是否是绝对路径
extern bool IsAbsolutlyPath(std::string path);

//获取路径中包含的文件名
extern std::string GetFileName(const char* path);

//保证创建文件的路径是存在的
extern bool EnsureFloder(const char* filePath);

//把目录中的"\\"替换成"/"
extern void ReplaceSlash(char* pBuffer,size_t len);

extern void ReplaceSlash(std::string& buffer);

#endif /*_FSUTILS_H_*/