#include "dbException.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//CDBException
CDBException::CDBException(int severity, int dberr, int oserr, const char * dberrstr,const char * oserrstr)
{
	m_severity = severity;
	m_dberr = dberr;
	m_oserr = oserr;
	if(dberrstr != NULL)
		strcpy_s(m_dberrstr, sizeof( m_dberrstr ), dberrstr);
	else
		m_dberrstr[0] ='\0';
	if(oserrstr != NULL)
		strcpy_s(m_oserrstr, sizeof( m_oserrstr ), oserrstr);
	else
		m_oserrstr[0]='\0';
	//[��չ]��������Ϣ�ÿ�
	w_msgno = 0;
	w_msgstate = 0;
	w_severity = 0;
	w_msgtext[0]='\0';
	w_srvname[0]='\0';
	w_procname[0]='\0';
	w_line = 0;
}

CDBException::CDBException(int severity, int dberr, int oserr, const char * dberrstr,const char * oserrstr,
							 long msgno, int msgstate, int _severity, const char * msgtext, const char * srvname,
							 const char *procname, unsigned short line)
{
	//[��չ]����Ϊ������Ϣ
	w_msgno = msgno;
	w_msgstate = msgstate;
	w_severity = _severity;
	if(msgtext != NULL)
		strcpy_s(w_msgtext, sizeof( w_msgtext ), msgtext);
	else
		w_msgtext[0]='\0';
	if(srvname != NULL)
		strcpy_s(w_srvname, sizeof( w_srvname ), srvname);
	else
		w_srvname[0]='\0';
	if(procname != NULL)
		strcpy_s(w_procname, sizeof( w_procname ), procname);
	else
		w_procname[0]='\0';
	w_line = line;
    //�������δʹ��
	m_severity = severity;
	m_dberr = dberr;
	m_oserr = oserr;
	if(dberrstr != NULL)
		strcpy_s(m_dberrstr, sizeof( m_dberrstr ), dberrstr);
	else
		m_dberrstr[0] ='\0';
	if(oserrstr != NULL)
		strcpy_s(m_oserrstr, sizeof( m_oserrstr ), oserrstr);
	else
		m_oserrstr[0]='\0';
}

CDBException::CDBException(long msgno, int msgstate, int _severity, const char * msgtext, const char * srvname,
							 const char *procname, unsigned short line)
{
	w_msgno = msgno;
	w_msgstate = msgstate;
	w_severity = _severity;
	if(msgtext != NULL)
		strcpy_s(w_msgtext, sizeof( w_msgtext), msgtext);
	else
		w_msgtext[0]='\0';
	if(srvname != NULL)
		strcpy_s(w_srvname, sizeof( w_srvname ), srvname);
	else
		w_srvname[0]='\0';
	if(procname != NULL)
		strcpy_s(w_procname, sizeof( w_procname ), procname);
	else
		w_procname[0]='\0';
	w_line = line;
    //�������δʹ��
	m_severity=0;			//�����������
	m_dberr=0;			//���ݿ����
	m_oserr=0;			//����ϵͳ����
	m_dberrstr[0]='\0';	//���ݿ������Ϣ
	m_oserrstr[0]='\0';	//����ϵͳ������Ϣ

}

CDBException::CDBException() : m_severity(0), m_dberr(0), m_oserr(0)
{
	strcpy(m_dberrstr, "");
	strcpy(m_oserrstr, "");
}

CDBException::~CDBException()
{
}

void CDBException::Log(std::stringstream &LogStream)
{
	LogStream << "(SQL) "
		<< " DB:" << m_dberr << "(" << m_dberrstr << ")"
		<< " OS:" << m_oserr << "(" << m_oserrstr << ")" 
		<< " LV:" << w_severity << "(" << w_msgtext << ")"
		<< " PRG:(" << w_procname << ")"
		<< std::ends;
}





