#pragma once

#ifndef _DB_CORE_H
#define _DB_CORE_H

#include <hash_map>

#define _WIN32_WINNT 0x0500
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>

#define DBNTWIN32
#include "sqlfront.h"
#include "sqldb.h"

#include "base/Locker.h"

#define DBE_DATA_BUFFER		2048
#define DBE_SQL_BUFFER		4096
#define DBE_CHUNK_SIZE		1024
#define PTAIL(x)			x+strlen(x)

//时间戳
typedef struct tagTimeStamp{
	unsigned short	year;
	unsigned char	month;
	unsigned char	day;
	unsigned char	hour;
	unsigned char	minute;
	unsigned char	second;
	unsigned short	millisecond;
	tagTimeStamp()
	{
		year = 1900;
		month = 1;
		day = 1;
		hour = 0;
		minute =0;
		second = 0;
		millisecond = 0;
	};
}TimeStamp;

enum eStatus
{
	E_NONE,
	E_SET_SQL,
	E_EXE_SQL,
};

/*SQLServer数据库处理操作的基类
*/
class CDBCore
{
	int m_InnerIndex;
public:
	CDBCore(void);
	virtual ~CDBCore(void);

public:
	bool Open(char *svr,char *usr,char *pwd,char *db);		//打开数据库连接
	void Close();								//关闭数据库连接

	void SQL(const char * cmd,...);				//准备执行SQL
	long Exec();								//执行SQL命令语句，丢弃结果集
	bool More();								//执行SQL语句并取出结果集
	bool Eval();								//执行SQL语句,返回结果集等待处理。 主要配合GetBLOBData使用
	bool Cancel();								//取消当前命令语句执行及所有处于等待状态的结果数据
	bool Clone(CDBCore * pDB);					//克隆当前连接

	void*	GetHandle(){return m_pdbHandle;}
public:
	//返回结果列的类型
	//@nIndex 结果列索引,以0开始
	//@strType 结果列的类型
	int		GetColType(int nIndex);
	void	GetColType(int nIndex, char * strType);
	//返回结果列的名称
	//@nIndex 结果列的索引,以0开始
	const char * GetColName(int nIndex);
	//返回结果列的长度
	//@nIndex 结果列的索引,以0开始
	int		GetColLen(int nIndex);
	//返回结果列数据长度
	//@nIndex 结果列的索引,以0开始
	int		GetColDataLen(int nIndex);
	//返回结果列的总数
	int		GetNumCols();

public:
	//获取某一列的数据
	//@nIndex 指定结果列，以0开始
	const char * Get(int nIndex);
	const char * Get(const char * pszFieldName);

	inline const char * GetString(int nIndex) 
    {
        const char* ptr = Get(nIndex);
        return ptr ? ptr : "";
    }

	inline const char * GetString(const char * pszFieldName) 
    {
        const char* ptr = Get(pszFieldName);
        return ptr ? ptr : "";
    }

	//获取某一列的整型数据
	long	GetLong(int nIndex);
	long	GetLong(const char * pszFieldName);
	inline int GetInt(int nIndex) {return (int)GetLong(nIndex);};
	inline int GetInt(const char * pszFieldName) {return (int)GetLong(pszFieldName);};
	//获取某一列的浮点型数据
	double	GetDouble(int nindex);
	double  GetDouble(const char * pszfieldname);

    PBYTE   GetBinary(int nIndex, int &len,PBYTE pBuf=NULL);
    PBYTE   GetBinary(const char * pszFieldName, int &len);

	//Image字段的操作，必须为唯一字段
	PBYTE	GetBlob(int &len,PBYTE pData);
	bool	SetBlob(const char* pszTableName, const char* pszFieldName, PBYTE pData,int len);

	//获取某一列的时间值
	//@nIndex 列索引,以0为起点
	//@pszFieldName 列名
	//@dtfmt 返回的时间格式，可以为以下格式组合
	//	%Y	年
	//	%m	月
	//	%D	日
	//	%H	小时
	//	%M	分钟
	//	%S	秒
	//	%MS	毫秒
	TimeStamp	GetDateTime(int nIndex);
    int         GetDateTimeEx(int nIndex);
	TimeStamp	GetDateTime(const char * pszFieldName);
	const char * GetDateTime(int nIndex, const char * dtfmt);
	const char * GetDateTime(const char * pszFieldName, const char * dtfmt);
	//查询某一列名所对应的索引
	//@pszFieldName 列名
	//@@return -1为未找到对应列,其他值为列索引,以0开始
	int		FindIndex(const char * pszFieldName);

	//顺序获取所有列
    int         GetTime()   {   return GetDateTimeEx(m_InnerIndex++);}
	int			GetInt()	{	return GetInt(m_InnerIndex++);		 }
	const char* GetString()	{	return GetString(m_InnerIndex++);	 }
	long		GetLong()	{	return GetLong(m_InnerIndex++);		 }
	double		GetDouble()	{	return GetDouble(m_InnerIndex++);	 }

public:
	/*
	//事务处理程序
	//@pszTransName 事务各称,可以缺省，但缺省事务只能一个。
	//启动事务后必须调用Commit或Rollback处理，否则处理会出错。
	//事务名具有唯一性，事务可以嵌套。
	*/
	//启动事务
	bool BeginTrans(const char * pszTransName = NULL);
	//提交事务
	bool Commit(const char * pszTransName = NULL);
	//事务回滚
	bool Rollback(const char * pszTransName = NULL);

public:
	//以下两个函数只是为了与数据库警告和错误信息连接
	void AssignSQLError(int severity, int dberr, int oserr, char *dberrstr, char *oserrstr);
	void AssignSQLMessage(long msgno, int msgstate, int severity, char * msgtext, char * srvname,char *procname, unsigned short line);

protected:
	void			ValidateIndex(int nIndex);		//检查列号是否有效
	bool			Execute();						//内部公共执行函数
	bool			IsValidate();					//判断数据库连接是否断开
	bool			Relink();

protected:
	int				e_severity;			//错误的严重性
	int				e_dberr;			//数据库错误
	int				e_oserr;			//操作系统错误
	char			e_dberrstr[256];	//数据库错误信息
	char			e_oserrstr[256];	//操作系统错误信息

	long			w_msgno;			//消息ID
	int				w_msgstate;			//消息状态
	int				w_severity;			//消息严重性
	char			w_msgtext[256];		//消息文本
	char			w_srvname[64];		//服务器名
	char			w_procname[64];		//过程名
	unsigned short	w_line;				//消息行数

protected:
	void		*m_pdbHandle;					//数据库句柄

	int			m_nStatus;						//操作步骤
	int			m_nCode;		                //SQL执行状态

protected:
	char		*m_strSvr;						//服务器名
	char		*m_strUser;						//用户名
	char		*m_strPwd;						//密码
	char		*m_strDB;						//数据库

protected:
	int         m_nSetCount;					//记录集数量
	int			m_nCount;		                //结果行数

	char 	    m_pSQL[DBE_SQL_BUFFER];			//SQL语句
	char 	    m_pData[DBE_DATA_BUFFER];		//数据

protected:
	bool		m_bAutoRelink;					//自动重连接
};

//负责数据库连接管理和数据库参数设置
class CDBManager
{
	static bool							m_bLibInited;

	CMyCriticalSection					m_cs;
	stdext::hash_map<int,CDBCore *>		m_List;

	CDBManager();
	~CDBManager();

	static CDBManager					m_Instance;
public:
	static void InitDBLib();
	static void FreeDBLib();

	static void SetOpenTimeout(int openTimeout);
	static void SetExecTimeout(int execTimeout);

	static CDBManager*	GetInstance(){return &m_Instance;}
public:
	bool Regist(CDBCore *);
	bool Unregist(CDBCore *);
	CDBCore *GetObject(int);
};


#endif

