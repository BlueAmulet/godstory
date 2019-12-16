#include "dbCore.h"
#include "dbException.h"
#include <atltime.h>

bool CDBManager::m_bLibInited	= false;
CDBManager CDBManager::m_Instance;

#define PTAIL(x)	x+strlen(x)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//字符串操作
char * _replace(char * InBuf, const char * _rep, int _n, int _size=2)
{
	char * pc, *pc2;
	char tmp[10],fmt[10];
	int len;
	int pos_before,pos_after,pos_tmp;

	while((pc = strstr(InBuf, _rep))!=NULL)
	{
		if(_size == -1)
			strcpy(fmt, "%d");
		else
			sprintf(fmt, "%%0%dd",_size);
		pc2 = pc+strlen(_rep);
		len = (int)(pc2-pc);
		sprintf(tmp, fmt, _n);
		pos_before = (int)(pc - InBuf);
		pos_after = (int)strlen(pc2);
		pos_tmp = (int)strlen(tmp);
		if(pos_after>0 && pos_tmp != len)
			memmove(pc+pos_tmp, pc2, pos_after);
		memcpy(pc, tmp, pos_tmp);
		if(pos_tmp != len)
			memset(InBuf+pos_before+pos_tmp+pos_after, 0, 1);
	}
	return InBuf;
}

char * _replace(char * InBuf, const char * _source, const char * _dest)
{
	char * pc, *pc2;
	int len;
	int pos_before,pos_after,pos_tmp;
	pos_tmp = (int)strlen(_dest);

	while((pc = strstr(InBuf, _source))!=NULL)
	{
		pc2 = pc+strlen(_source);
		len = (int)(pc2-pc);
		pos_before = (int)(pc - InBuf);
		pos_after = (int)strlen(pc2);
		if(pos_after>0 && pos_tmp != len)
			memmove(pc+pos_tmp, pc2, pos_after);
		memcpy(pc, _dest, pos_tmp);
		if(pos_tmp != len)
			memset(InBuf+pos_before+pos_tmp+pos_after, 0, 1);
	}
	return InBuf;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//CDBCore
CDBCore::CDBCore(void)
{
	m_pdbHandle = NULL;

	m_nStatus		= E_NONE;
	m_nCode			= 0;			//SQL执行状态

	m_strSvr		= NULL;
	m_strUser		= NULL;
	m_strPwd		= NULL;
	m_strDB			= NULL;
	
	m_nCount		= 0;			//操作结果行数
	m_nSetCount		= 0;			//操作结果记录集数

	m_pSQL[0]		= '\0';			//SQL语句
	m_pData[0]		= '\0';			//缓存

	m_bAutoRelink	= true;			//自动重连接

	m_InnerIndex	= 0;

	e_dberrstr[0] = 0;	//数据库错误信息
	e_oserrstr[0] = 0;	//操作系统错误信息


	w_msgtext[0] = 0;		//消息文本
	w_srvname[0] = 0;		//服务器名
	w_procname[0] = 0;		//过程名
}

CDBCore::~CDBCore(void)
{
	Close();
}

void CDBCore::ValidateIndex(int nIndex)
{
	if(nIndex<0 || nIndex>=dbnumcols(m_pdbHandle))
	{
		strcpy(w_msgtext,"Index out of range!");
		CDBException e(w_msgno, w_msgstate, w_severity, w_msgtext, w_srvname, w_procname, w_line);
		throw e;
	}
}

void CDBCore::AssignSQLError(int severity, int dberr, int oserr, char *dberrstr, char *oserrstr)
{
    //若是本类已关闭,对后继错误不予提交

	e_severity = severity;
	e_dberr = dberr;
	e_oserr = oserr;
	if(dberrstr != NULL)
		strcpy_s(e_dberrstr, sizeof( e_dberrstr ), dberrstr);
	else
		e_dberrstr[0]='\0';
	if(oserrstr != NULL)
		strcpy_s(e_oserrstr, sizeof( e_oserrstr ), oserrstr);
	else
		e_oserrstr[0] = '\0';
}

void CDBCore::AssignSQLMessage(long msgno, int msgstate, int severity, char * msgtext, char * srvname,char *procname, unsigned short line)
{
	w_msgno = msgno;
	w_msgstate = msgstate;
	w_severity = severity;
	if(msgtext != NULL)
		strcpy(w_msgtext, msgtext);
	else
		w_msgtext[0]='\0';
	if(srvname != NULL)
		strcpy(w_srvname, srvname);
	else
		w_srvname[0]='\0';
	if(procname != NULL)
		strcpy(w_procname, procname);
	else
		w_procname[0]='\0';
	w_line = line;
}

bool CDBCore::Relink()
{
	memset(&m_pData[0], 0,DBE_DATA_BUFFER);

	CDBManager::InitDBLib();

	if(m_pdbHandle != (PDBPROCESS)NULL)
	{
		CDBManager::GetInstance()->Unregist(this);
		dbclose(m_pdbHandle);
	}

	PLOGINREC pLoginHandle = dblogin();
	if(pLoginHandle == (PLOGINREC)NULL)
	{
		return false;
	}

	DBSETLUSER(pLoginHandle, m_strUser);
	DBSETLPWD(pLoginHandle,m_strPwd);
	DBSETLVERSION(pLoginHandle, DBVER60);
	DBSETLAPP(pLoginHandle, "DBLib");

	unsigned long nSize = 255;
	char Buf[256];

	BCP_SETL(pLoginHandle, true);
	if(GetComputerName(Buf, &nSize))
		DBSETLHOST(pLoginHandle, Buf);
	else
		DBSETLHOST(pLoginHandle, "UNKNOW");

	strcpy(Buf, m_strSvr);
	m_pdbHandle=dbopen(pLoginHandle, (LPCSTR)Buf);

	dbfreelogin(pLoginHandle);

	if(m_pdbHandle == (PDBPROCESS)NULL)
	{
		CDBException e(w_msgno, w_msgstate, w_severity, w_msgtext, w_srvname, w_procname, w_line);
		throw e;
	}

	dbuse(m_pdbHandle, (LPCSTR)m_strDB);

	CDBManager::GetInstance()->Regist(this);
	return true;
}

bool CDBCore::Open(char *svr,char *usr,char *pwd, char *db)
{
	m_strSvr	= svr;
	m_strUser	= usr;
	m_strPwd	= pwd;
	m_strDB		= db;

	return Relink();
}

void CDBCore::Close()
{
	if(m_pdbHandle != (PDBPROCESS)NULL)
	{
		CDBManager::GetInstance()->Unregist(this);

		dbclose(m_pdbHandle);
		m_pdbHandle = NULL;
	}

	m_nStatus = E_NONE;
}

void CDBCore::SQL(const char * sql, ...)
{
	if(m_nStatus==E_EXE_SQL && m_pdbHandle)
	{
		if(dbcancel(m_pdbHandle) == FAIL){
			int error = 0;
		}
	}

	m_pSQL[0] = 0;

	va_list s1;
	va_start(s1, sql);
	vsprintf(PTAIL(m_pSQL), sql, s1);
	va_end(s1);

	m_nStatus = E_SET_SQL;
	m_nCount = 0;
	m_nSetCount = 0;
	m_nCode = 0;
}

bool CDBCore::IsValidate()
{
	if(m_pdbHandle == NULL || dbdead(m_pdbHandle) )
	{
		CDBManager::GetInstance()->Unregist(this);

		dbcancel(m_pdbHandle);
		dbclose(m_pdbHandle);		//Ray:可能会有异常抛出,所以所有数据库异常都需要加catch(...)
		m_pdbHandle = NULL;
		return false;
	}

	return true;
}

bool CDBCore::Execute()
{
	if(m_nStatus == E_NONE)
		return false;

	if(m_nStatus == E_EXE_SQL)
	{
		dbcancel(m_pdbHandle);		//缓存中还有数据未取出
		m_nStatus = E_SET_SQL;
	}

	m_nCount = 0;
	m_InnerIndex = 0;

	//判断是否已经断连接，若断链接，则自动重连
	if(!IsValidate())
	{
		if(!m_bAutoRelink || !Relink())
		{
			CDBException e(w_msgno, w_msgstate, w_severity, w_msgtext, w_srvname, w_procname, w_line);
			throw e;
		}
	}

	//执行SQL
	dbcmd(m_pdbHandle, m_pSQL);
	if(dbsqlexec(m_pdbHandle) == FAIL)
	{
		//CDBException e(w_msgno, w_msgstate, w_severity, w_msgtext, w_srvname, w_procname, w_line);

		IsValidate();
		m_nCode		= 0;
		m_nStatus	= E_NONE;

		//throw e;
        printf("error exec sql:%s",w_msgtext);
        return false;
	}

	return true;
}

long CDBCore::Exec()
{
	if(!Execute())
		return 0;

	//丢弃结果集
	while(dbresults(m_pdbHandle) != NO_MORE_RESULTS)
		while(dbnextrow(m_pdbHandle)!=NO_MORE_ROWS);

	m_nCode = dbretstatus(m_pdbHandle);
	m_nStatus = E_NONE;
	m_nCount = DBCOUNT(m_pdbHandle);
	return m_nCount;
}

bool CDBCore::Eval()
{
	if(!Execute())
		return false;

	if(dbresults(m_pdbHandle) != SUCCEED)
	{
		CDBException e(w_msgno, w_msgstate, w_severity, w_msgtext, w_srvname, w_procname, w_line);

		IsValidate();
		m_nCode		= 0;
		m_nStatus	= E_NONE;

		throw e;
	}

	m_nStatus = E_EXE_SQL;
	m_nSetCount = 1;
	m_nCode = dbretstatus(m_pdbHandle);
	return true;
}

bool CDBCore::More()
{
	m_InnerIndex = 0;

	if(m_nStatus != E_EXE_SQL)
	{
		if(!Eval())
			return false;
	}

	do{
		if(dbnextrow(m_pdbHandle)!=NO_MORE_ROWS)
		{
			m_nCount++;
			return true;
		}

		if(dbresults(m_pdbHandle)!=NO_MORE_RESULTS)
		{
			m_nSetCount++;
			continue;
		}
	}while(0);

	m_nStatus = E_NONE;
	return false;
}

bool CDBCore::Cancel()
{
	if(dbcancel(m_pdbHandle) == SUCCEED)
		return true;
	else
		return false;
}

bool CDBCore::Clone(CDBCore * pDB)
{
	return pDB->Open(m_strSvr, m_strUser, m_strPwd, m_strDB);
}


int CDBCore::GetColType(int nIndex)
{
	ValidateIndex(nIndex);

	return dbcoltype(m_pdbHandle, nIndex+1);
}

void CDBCore::GetColType(int nIndex, char * strType)
{
	ValidateIndex(nIndex);

	switch(dbcoltype(m_pdbHandle, nIndex+1))
	{
	case SQLCHAR: 
	case SQLVARCHAR:
		strcpy(strType, "C");
		break;
	case SQLINT1:
	case SQLINT2:
	case SQLINT4:
		strcpy(strType, "I");
		break;
	case SQLFLT4:
	case SQLFLT8:
	case SQLNUMERIC:
		strcpy(strType, "R");
		break;
	case SQLDATETIM4:
	case SQLDATETIME:
		strcpy(strType, "D");
		break;
	case SQLIMAGE:
		strcpy(strType, "IMAGE");
		break;
	case SQLTEXT:
		strcpy(strType, "TEXT");
		break;
	case SQLBINARY:
		strcpy(strType, "BINARAY");
		break;
	case SQLMONEY :
	case SQLMONEY4:
		strcpy(strType, "MONEY");
		break;
	default:
		strcpy(strType, "N/A");
		break;
	}
}

const char * CDBCore::GetColName(int nIndex)
{
	ValidateIndex(nIndex);
	return dbcolname(m_pdbHandle, nIndex+1);
}

int CDBCore::GetColLen(int nIndex)
{
	ValidateIndex(nIndex);
	return dbcollen(m_pdbHandle, nIndex+1);
}

int CDBCore::GetColDataLen(int nIndex)
{
	ValidateIndex(nIndex);
	return dbdatlen(m_pdbHandle, nIndex+1);
}

int CDBCore::GetNumCols()
{
	return dbnumcols(m_pdbHandle);
}

const char * CDBCore::Get(int nIndex)
{
	ValidateIndex(nIndex);

	DBDATETIME date;
	DBDATEREC tm;
	memset(&m_pData[0], 0, DBE_DATA_BUFFER);
	int n;
	switch(dbcoltype(m_pdbHandle,nIndex+1)){
	case SQLDATETIME:
	case SQLDATETIM4:
		dbconvert(m_pdbHandle, dbcoltype(m_pdbHandle,nIndex+1 ),dbdata(m_pdbHandle,nIndex+1), dbdatlen(m_pdbHandle,nIndex+1),
					SQLDATETIME,(BYTE *)&date, (DBINT)-1);
		dbdatecrack(m_pdbHandle,&tm,&date);
		sprintf(m_pData,"%04d.%02d.%02d %02d:%02d:%02d",tm.year,tm.month,tm.day,tm.hour,tm.minute,tm.second);
		n = 20;
		break;
	default:
		n = dbconvert(m_pdbHandle, dbcoltype(m_pdbHandle,nIndex+1),dbdata(m_pdbHandle,nIndex+1), dbdatlen(m_pdbHandle,nIndex+1),
					SQLCHAR, (unsigned char *)m_pData,(DBINT)-1);
		if( n==-1)
			return (const char *)m_pData;
	}
	return (const char *)m_pData;
}

const char * CDBCore::Get(const char * pszFieldName)
{
	return Get(FindIndex(pszFieldName));
}

int CDBCore::FindIndex(const char * pszFieldName)
{
	for(int nIndex=1;nIndex<=dbnumcols(m_pdbHandle);nIndex++)
		if(!strcmp(dbcolname(m_pdbHandle,nIndex), pszFieldName))
			return (nIndex-1);
	return -1;
}

long CDBCore::GetLong(int nIndex)
{
	ValidateIndex(nIndex);

	DBINT n;
	if(dbwillconvert(dbcoltype(m_pdbHandle,nIndex+1 ),SQLINTN)==FALSE) return 0L;
	if(dbconvert(m_pdbHandle, dbcoltype(m_pdbHandle,nIndex+1),dbdata(m_pdbHandle,nIndex+1), dbdatlen(m_pdbHandle,nIndex+1),
					SQLINT4, (unsigned char *)&n,(DBINT)-1)==-1) return 0L;
	return n;
}

long CDBCore::GetLong(const char * pszFieldName)
{
	return GetLong(FindIndex(pszFieldName));
}

double CDBCore::GetDouble(int nIndex)
{
	ValidateIndex(nIndex);

	double d;
	if(dbwillconvert(dbcoltype(m_pdbHandle,nIndex+1),SQLFLT8)==FALSE){
		return 0.00;
	}
	if(dbconvert(m_pdbHandle, dbcoltype(m_pdbHandle,nIndex+1 ),dbdata(m_pdbHandle,nIndex+1), dbdatlen(m_pdbHandle,nIndex+1),
		SQLFLT8, (unsigned char *)&d,(DBINT)-1)==-1){		
		return 0.00;
	}
	return d;
}

double CDBCore::GetDouble(const char * pszFieldName)
{
	return CDBCore::GetDouble(FindIndex(pszFieldName));
}

TimeStamp CDBCore::GetDateTime(int nIndex)
{
	ValidateIndex(nIndex);

	//CTime ct = CTime::CTime(1900,1,1,0,0,0);	//1900.01.01为空值
	TimeStamp ts;
	DBDATETIME date;
	DBDATEREC tm;

	switch(dbcoltype(m_pdbHandle,nIndex+1))
	{
	case SQLDATETIME:
	case SQLDATETIM4:
		dbconvert(m_pdbHandle, dbcoltype(m_pdbHandle,nIndex+1 ),dbdata(m_pdbHandle,nIndex+1), dbdatlen(m_pdbHandle,nIndex+1),
					SQLDATETIME,(BYTE *)&date, (DBINT)-1);
		dbdatecrack(m_pdbHandle,&tm,&date);
		//ct = CTime::CTime(tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second);
		ts.year = tm.year;
		ts.month = tm.month;
		ts.day = tm.day;
		ts.hour = tm.hour;
		ts.minute = tm.minute;
		ts.second = tm.second;
		ts.millisecond = tm.millisecond;
		break;
	}
	return ts;
}

int CDBCore::GetDateTimeEx(int nIndex)
{
    TimeStamp ts = GetDateTime(nIndex);

    if (ts.year == 1900)
        return 0;

    CTime ct(ts.year,ts.month,ts.day,ts.hour,ts.minute,ts.second);
    return ct.GetTime();
}

TimeStamp CDBCore::GetDateTime(const char * pszFieldName)
{
	return CDBCore::GetDateTime(FindIndex(pszFieldName));
}

const char * CDBCore::GetDateTime(int nIndex, const char * dtfmt)
{
	ValidateIndex(nIndex);

	DBDATETIME date;
	DBDATEREC tm;
	m_pData[0] = '\0';
	strcpy(m_pData, dtfmt);
	switch(dbcoltype(m_pdbHandle,nIndex+1))
	{
	case SQLDATETIME:
	case SQLDATETIM4:
		dbconvert(m_pdbHandle, dbcoltype(m_pdbHandle,nIndex+1 ),dbdata(m_pdbHandle,nIndex+1), dbdatlen(m_pdbHandle,nIndex+1),
					SQLDATETIME,(BYTE *)&date, (DBINT)-1);
		dbdatecrack(m_pdbHandle,&tm,&date);
		_replace(m_pData, "%MS", tm.millisecond, 3);
		_replace(m_pData, "%Y", tm.year, -1);
		_replace(m_pData, "%m", tm.month);
		_replace(m_pData, "%d", tm.day);
		_replace(m_pData, "%H", tm.hour);
		_replace(m_pData, "%M", tm.minute);
		_replace(m_pData, "%S", tm.second);
		break;
	}
	return (const char *)m_pData;
}

const char * CDBCore::GetDateTime(const char * pszFieldName, const char * dtfmt)
{
	return CDBCore::GetDateTime(CDBCore::FindIndex(pszFieldName), dtfmt);
}

PBYTE CDBCore::GetBinary(int nIndex, int &len, PBYTE pBuffer)
{
	ValidateIndex(nIndex);

	PBYTE pBuf = NULL;
	if(pBuffer == NULL){
		memset(&m_pData[0], 0, DBE_DATA_BUFFER);
		pBuf = (PBYTE)&m_pData[0];
		len = DBE_DATA_BUFFER;
	}else{
		pBuf = pBuffer;
	}
	
	int DataLength = dbdatlen(m_pdbHandle,nIndex+1);
	if(DataLength>len)
	{
		len = DataLength;
		return NULL;
	}

	len = DataLength;

	int n;
	switch(dbcoltype(m_pdbHandle,nIndex+1)){
	case SQLBINARY:
	case SQLVARBINARY:
		n=dbconvert(m_pdbHandle, dbcoltype(m_pdbHandle,nIndex+1), dbdata(m_pdbHandle,nIndex+1), dbdatlen(m_pdbHandle,nIndex+1),
					SQLVARBINARY,(BYTE *)pBuf, (DBINT)-1);
		if( n==-1)
			return (PBYTE)NULL;
		break;
	default:
		n = dbconvert(m_pdbHandle, dbcoltype(m_pdbHandle,nIndex+1),dbdata(m_pdbHandle,nIndex+1), dbdatlen(m_pdbHandle,nIndex+1),
					SQLCHAR, (unsigned char *)pBuf,(DBINT)-1);
		if( n==-1)
			return (PBYTE)NULL;
	}
	return (PBYTE)pBuf;
}

PBYTE CDBCore::GetBinary(const char *pszFieldName, int &len)
{
    return CDBCore::GetBinary(CDBCore::FindIndex(pszFieldName), len);
}

PBYTE CDBCore::GetBlob(int &len,PBYTE pData)
{
	//判断是否只为单字段
	if (dbnumcols (m_pdbHandle) != 1)
	{
		strcpy(w_msgtext,"Must be one and only one blob field!");
		CDBException e(w_msgno, w_msgstate, w_severity, w_msgtext, w_srvname, w_procname, w_line);
		throw e;
	}

	//判断是否为BLOB字段类型
	int nColumnType = dbcoltype (m_pdbHandle, 1);
	if ((nColumnType != SQLTEXT) && (nColumnType != SQLIMAGE))
	{
		strcpy(w_msgtext,"Must be a blob field(Image or Text)!");
		CDBException e(w_msgno, w_msgstate, w_severity, w_msgtext, w_srvname, w_procname, w_line);
		throw e;
	}

	int DataLength = dbdatlen(m_pdbHandle,1);
	if(!pData)
		len = DBE_DATA_BUFFER;
	if(DataLength>len)
	{
		len = DataLength;
		return NULL;
	}

	len = DataLength;

	DBINT iBytes;
	DBINT iPos = 0;
	PBYTE pBuf = NULL;
	if(pData)
		pBuf = pData;
	else
	{
		memset(&m_pData[0], 0, DBE_DATA_BUFFER);
		pBuf = (PBYTE)&m_pData[0];
	}

	do
	{
		iBytes = dbreadtext (m_pdbHandle, pBuf, DBE_CHUNK_SIZE);
		switch(iBytes)
		{
		case -1:
			return false;
		case 0:
			break;
		default:
			iPos += iBytes;
			pBuf+=iBytes;
			break;
		}
	}while(iBytes>0);

	return pBuf;
}

bool CDBCore::SetBlob(const char* pszTableName, const char* pszColumnName, PBYTE pData,int len)
{
	if(!pData || !len)
		return true;

	DBBINARY aTextPointer[DBTXPLEN], aTextTimestamp[DBTXTSLEN];
	DBBINARY *pTextPointer, *pTextTimestamp;

	//判断是否有数据
	STATUS status= dbnextrow (m_pdbHandle);
	if (status != REG_ROW)
	{
		strcpy(w_msgtext,"Must have one row data!");
		CDBException e(w_msgno, w_msgstate, w_severity, w_msgtext, w_srvname, w_procname, w_line);
		throw e;
	}
	
	//定位到列
	int nIndex = FindIndex(pszColumnName);
	if(nIndex==-1)
		return FALSE; 

	//判断是否为Blob字段类型
	int nColumnType = dbcoltype (m_pdbHandle,nIndex+1);
	if ((nColumnType != SQLTEXT) && (nColumnType != SQLIMAGE))
	{
		strcpy(w_msgtext,"Must be a blob field(Image or Text)!");
		CDBException e(w_msgno, w_msgstate, w_severity, w_msgtext, w_srvname, w_procname, w_line);
		throw e;
	}

	// 获取Blob 指针和时间戳值
	pTextPointer = dbtxptr (m_pdbHandle, nIndex+1);
	pTextTimestamp = dbtxtimestamp (m_pdbHandle, nIndex+1);
	if ((pTextPointer == NULL) || (pTextTimestamp == NULL))
	{
		strcpy(w_msgtext,"Image pointer and timestamp retrieval failed!");
		CDBException e(w_msgno, w_msgstate, w_severity, w_msgtext, w_srvname, w_procname, w_line);
		throw e;
	}

	memcpy (aTextPointer, pTextPointer, DBTXPLEN);
	memcpy (aTextTimestamp, pTextTimestamp, DBTXTSLEN);

	status = dbnextrow (m_pdbHandle);
	if(status != NO_MORE_ROWS)
	{
		strcpy(w_msgtext,"Should only have received one row data!");
		CDBException e(w_msgno, w_msgstate, w_severity, w_msgtext, w_srvname, w_procname, w_line);
		throw e;
	}

	RETCODE ret = dbresults (m_pdbHandle);
	if (ret != NO_MORE_RESULTS)
	{
		strcpy(w_msgtext,"Query results failed!");
		CDBException e(w_msgno, w_msgstate, w_severity, w_msgtext, w_srvname, w_procname, w_line);
		throw e;
	}

	char TableAndFieldName[128];
	sprintf(TableAndFieldName,"%s.%s",pszTableName,pszColumnName);

	ret = dbwritetext (m_pdbHandle, TableAndFieldName, aTextPointer, DBTXPLEN, aTextTimestamp, TRUE, len, NULL);
	if (ret == FAIL)
	{
		strcpy(w_msgtext,"(dbwritetext):Text or image write failed!");
		CDBException e(w_msgno, w_msgstate, w_severity, w_msgtext, w_srvname, w_procname, w_line);
		throw e;
	}

	// 在调用dbmoretext之前预处理dbsqlok和dbresults
	ret = dbsqlok (m_pdbHandle);
	if (ret == FAIL)
	{
		strcpy(w_msgtext,"(dbsqlok和dbresults):Text or image write failed!");
		CDBException e(w_msgno, w_msgstate, w_severity, w_msgtext, w_srvname, w_procname, w_line);
		throw e;
	}

	ret = dbresults (m_pdbHandle);
	if (ret == FAIL)
	{
		strcpy(w_msgtext,"Query results failed!");
		CDBException e(w_msgno, w_msgstate, w_severity, w_msgtext, w_srvname, w_procname, w_line);
		throw e;
	}

	// 分块写入SQLServer
	DBINT nReadBytes;
	PBYTE pBuf = pData;
	DBINT iCount = 0;
	do
	{
		if( (len - iCount) > DBE_CHUNK_SIZE)
			nReadBytes = DBE_CHUNK_SIZE;
		else
			nReadBytes = len - iCount;

		ret = dbmoretext (m_pdbHandle, nReadBytes, pBuf);
		if (ret == FAIL)
		{
			strcpy(w_msgtext,"(dbmoretext):Text or image write failed!");
			CDBException e(w_msgno, w_msgstate, w_severity, w_msgtext, w_srvname, w_procname, w_line);
			throw e;
		}

		iCount += nReadBytes;
		pBuf += nReadBytes;
	}while (len>iCount);

	ret = dbsqlok (m_pdbHandle);
	if (ret == FAIL)
	{
		strcpy(w_msgtext,"(dbsqlok):Text or image write failed!");
		CDBException e(w_msgno, w_msgstate, w_severity, w_msgtext, w_srvname, w_procname, w_line);
		throw e;
	}
	ret = dbresults (m_pdbHandle);
	if (ret == FAIL)
	{
		strcpy(w_msgtext,"Query results failed!");
		CDBException e(w_msgno, w_msgstate, w_severity, w_msgtext, w_srvname, w_procname, w_line);
		throw e;
	}

	return true;
}

bool CDBCore::BeginTrans(const char * pszTransName)
{
	SQL("begin transaction ");
	if(pszTransName != NULL)
		SQL(pszTransName);
	return Exec()>=0 ? true : false;
}

bool CDBCore::Commit(const char * pszTransName)
{
	SQL("commit transaction ");
	if(pszTransName != NULL)
		SQL(pszTransName);
	return Exec()>=0 ? true : false;
}

bool CDBCore::Rollback(const char * pszTransName)
{
	SQL("rollback transaction ");
	if(pszTransName != NULL)
		SQL(pszTransName);
	return Exec() >= 0 ? true : false;
}

int msg_handler(DBPROCESS *dbproc, DBINT msgno, int msgstate, int severity, char * msgtext, char * srvname,char *procname, DBUSMALLINT line)
{
	CDBCore * pObject = CDBManager::GetInstance()->GetObject((int)dbproc);
	if(pObject)
	{
		pObject->AssignSQLMessage(msgno, msgstate, severity, msgtext, srvname, procname, line);
	}
	return INT_CONTINUE;
}

int err_handler(DBPROCESS *dbproc, int severity, int dberr, int oserr, char *dberrstr, char *oserrstr)
{
	CDBCore * pObject = CDBManager::GetInstance()->GetObject((int)dbproc);
	if(pObject)
	{
		pObject->AssignSQLError(severity, dberr, oserr, dberrstr, oserrstr);
	}
	return INT_CONTINUE;
}

void CDBManager::InitDBLib()
{
	if(!m_bLibInited)
	{
		dbinit();

		dberrhandle((DBERRHANDLE_PROC)err_handler);
		dbmsghandle((DBMSGHANDLE_PROC)msg_handler);
		m_bLibInited = true;
	}
}

void CDBManager::FreeDBLib()
{
	if(m_bLibInited)
	{
		dbexit();
		m_bLibInited = false;
	}
}

void CDBManager::SetOpenTimeout(int openTimeout)
{
	dbsetlogintime(openTimeout);	//连接超时设置(default:60)
}

void CDBManager::SetExecTimeout(int execTimeout)
{
	dbsettime(execTimeout);			//SQL执行超时设置(default:0 infinit)
}


CDBManager::CDBManager()
{
	CDBManager::InitDBLib();
}

CDBManager::~CDBManager()
{
	CDBManager::FreeDBLib();
}

bool CDBManager::Regist(CDBCore *pObject)
{
	CLocker Lock(m_cs);
	m_List[(int)pObject->GetHandle()] = pObject;
	return true;
}

bool CDBManager::Unregist(CDBCore *pObject)
{
	CLocker Lock(m_cs);
	m_List.erase((int)pObject->GetHandle());
	return true;
}

CDBCore *CDBManager::GetObject(int Handle)
{
	CLocker Lock(m_cs);

	stdext::hash_map<int,CDBCore *>::iterator it=m_List.find(Handle);
	if(it!=m_List.end())
	{
		CDBCore *pTemp = it->second;
		return pTemp;
	}
	return NULL;
}


