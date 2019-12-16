
#include "EXCEL9.H"
#include "Excel.h"


class CExcelImpl
{
public:
    CExcelImpl(void);

    ~CExcelImpl(void);

    bool Initialize();
    void DeInitialize();
    void Attach(const _variant_t& var);
    void Detach();
    void Release();
    bool IsValidSheetIndex(int sheetIdx);
public:
    //////////////////////////////////////////////////////////////////////////
    //remark:		��һ��EXCEL���
    //params:		1
    //par1:			���ļ�·��
    //ret:			�򿪳ɹ�:true ��ʧ��:false
    //////////////////////////////////////////////////////////////////////////
    bool Open(const char* filePath);

    //////////////////////////////////////////////////////////////////////////
    //remark:		�رյ�ǰ�򿪵�EXCEL���
    //params:		0
    //ret:			void
    //////////////////////////////////////////////////////////////////////////
    void Close();

    //////////////////////////////////////////////////////////////////////////
    //remark:		���浱ǰ�򿪵�EXCEL�ļ�
    //params:		1
    //par1:			�����ļ�·��
    //ret:			void
    //////////////////////////////////////////////////////////////////////////
    void SaveAs(const char* filePath);

    //////////////////////////////////////////////////////////////////////////
    //remark:		��յ�ǰ��EXCEL�ļ�������
    //params:		0
    //ret:			void
    //////////////////////////////////////////////////////////////////////////
    void Reset();

    //////////////////////////////////////////////////////////////////////////
    //remark:		EXCEL�Ƿ񱻳�ʼ��
    //params:		0
    //ret:			�Ѿ���ʼ��:true��δ��ʼ��:false
    //////////////////////////////////////////////////////////////////////////
    bool IsInitialized();

    //////////////////////////////////////////////////////////////////////////
    //remark:		���EXCELӦ����
    //params:		0
    //ret:			EXCELӦ����
    //////////////////////////////////////////////////////////////////////////
    std::string GetAppName();

    //////////////////////////////////////////////////////////////////////////
    //remark:		����EXCEL�ļ�����
    //params:		0
    //ret:			void
    //////////////////////////////////////////////////////////////////////////
    void SetCaption(const char* capName);

    //////////////////////////////////////////////////////////////////////////
    //remark:		���EXCEL�ļ�����
    //params:		0
    //ret:			EXCEL�ļ�����
    //////////////////////////////////////////////////////////////////////////
    std::string GetCaption();

    //////////////////////////////////////////////////////////////////////////
    //remark:		����ָ��sheet��Ԫ������
    //params:		4
    //par1:			sheet��ţ�������sheet�����ַ�����Ҳ������sheet index����1��ʼ
    //par2:			��Ԫ����index����1��ʼ
    //par3:			��Ԫ����index����1��ʼ
    //par4:			�����õ�ֵ
    //ret:			�ɹ�:true , ʧ��:false
    //////////////////////////////////////////////////////////////////////////
    bool SetItem(const _variant_t& sheetIndex, const _variant_t& rowIndex, const _variant_t& colIndex, const _variant_t& val);

    //////////////////////////////////////////////////////////////////////////
    //remark:		���õ�ǰsheet��Ԫ������
    //params:		3
    //par1:			��Ԫ����index����1��ʼ
    //par2:			��Ԫ����index����1��ʼ
    //par3:			�����õ�ֵ
    //ret:			void
    //////////////////////////////////////////////////////////////////////////
    void SetItem(const _variant_t& rowIndex, const _variant_t& colIndex, const _variant_t& val);

    //////////////////////////////////////////////////////////////////////////
    //remark:		��õ�ǰsheet��Ԫ������
    //params:		2
    //par1:			��Ԫ����index����1��ʼ
    //par2:			��Ԫ����index����1��ʼ
    //ret:			��Ԫ��ֵ
    //////////////////////////////////////////////////////////////////////////
    _variant_t GetItem(const _variant_t& rowIndex, const _variant_t& colIndex);

    //////////////////////////////////////////////////////////////////////////
    //remark:		����ָ��sheet��Ԫ������
    //params:		3
    //par1:			sheet��ţ�������sheet�����ַ�����Ҳ������sheet index����1��ʼ
    //par2:			��Ԫ����index����1��ʼ
    //par3:			��Ԫ����index����1��ʼ
    //ret:			��Ԫ��ֵ
    //////////////////////////////////////////////////////////////////////////
    _variant_t GetItem(const _variant_t& sheetIndex, const _variant_t& rowIndex, const _variant_t& colIndex);

    //////////////////////////////////////////////////////////////////////////
    //remark:		��õ�ǰsheet������
    //params:		0
    //ret:			��ǰsheet������
    //////////////////////////////////////////////////////////////////////////
    std::string GetCurrentSheetName();

    //////////////////////////////////////////////////////////////////////////
    //remark:		���õ�ǰsheet������
    //params:		1
    //par1:			sheet����
    //ret:			void
    //////////////////////////////////////////////////////////////////////////
    void SetCurrentSheetName(const char* sheetName);

    //////////////////////////////////////////////////////////////////////////
    //remark:		ָ��sheetΪ��ǰsheet
    //params:		1
    //par1:			ָ��sheet�ı��,������sheet�����ַ�����Ҳ������sheet index����1��ʼ
    //ret:			�ɹ�:true , ʧ��:false
    //////////////////////////////////////////////////////////////////////////
    bool ActiveSheet(const _variant_t& sheet);

    //////////////////////////////////////////////////////////////////////////
    //remark:		���sheet����
    //params:		0
    //ret:			sheet����
    //////////////////////////////////////////////////////////////////////////
    long GetSheetsCount();

    //////////////////////////////////////////////////////////////////////////
    //remark:		��õ�ǰsheet�������
    //params:		0
    //ret:			sheet�������
    //////////////////////////////////////////////////////////////////////////
    long GetCurrentSheetIndex();

    //////////////////////////////////////////////////////////////////////////
    //remark:		����insertedSheetName��sheetǰ
    //params:		2
    //par1:			���뵽��sheet��ǰ��
    //par2:			����sheet������
    //ret:			�ɹ�:true , ʧ��:false
    //////////////////////////////////////////////////////////////////////////
    bool InsertSheetBefore(const _variant_t& sheet, const char* insertedSheetName);

    //////////////////////////////////////////////////////////////////////////
    //remark:		����insertedSheetName��sheet��
    //params:		2
    //par1:			���뵽��sheet�ĺ���
    //par2:			����sheet������
    //ret:			�ɹ�:true , ʧ��:false
    //////////////////////////////////////////////////////////////////////////
    bool InsertSheetAfter(const _variant_t& sheet, const char* insertedSheetName);

    //////////////////////////////////////////////////////////////////////////
    //remark:		��sheet�б�pushһ��sheet
    //params:		1
    //par1:			����sheet������
    //ret:			void
    //////////////////////////////////////////////////////////////////////////
    void PushSheet(const char* pushedSheetName);

    //////////////////////////////////////////////////////////////////////////
    //remark:		ɾ��һ��sheet
    //params:		1
    //par1:			��ɾ��sheet,������sheet�����ַ�����Ҳ������sheet index����1��ʼ
    //ret:			�ɹ�:true , ʧ��:false
    //////////////////////////////////////////////////////////////////////////
    bool DeleteSheet(const _variant_t& deletedSheet);

    //////////////////////////////////////////////////////////////////////////
    //remark:		����һ��sheet
    //params:		1
    //par1:			�����ҵ�sheet,������sheet�����ַ�����Ҳ������sheet index����1��ʼ
    //ret:			sheet�±�����
    //////////////////////////////////////////////////////////////////////////
    int  FindSheet(const _variant_t& sheet);

    //////////////////////////////////////////////////////////////////////////
    //remark:		��õ�ǰ����������
    //params:		0
    //ret:			��������
    //////////////////////////////////////////////////////////////////////////
    std::string GetCurrentWorkBookName();

    //////////////////////////////////////////////////////////////////////////
    //remark:		��õ�ǰ����������
    //params:		0
    //ret:			����������
    //////////////////////////////////////////////////////////////////////////
    long GetWorkBooksCount();

    std::string GetCurrentRangeName();
    long GetCurrentRangeCount();

    _Application m_xslApp;
    Workbooks m_xslBooks;
    _Workbook m_xslBook;
    Worksheets m_xslSheets;
    _Worksheet m_xslSheet;
    Range m_xslRange;
    bool m_bInitialized;
};

CExcelImpl::CExcelImpl(void):
    m_bInitialized(false)
{
	m_bInitialized = Initialize();
}

CExcelImpl::~CExcelImpl(void)
{
	if (m_bInitialized)
		DeInitialize();
}

//CExcelImpl& CExcelImpl::_GetInstance()
//{
//	static CExcelImpl app;
//	if (!app.IsInitialized())
//	{
//		app.Initialize();
//	}
//	return app;
//}

bool CExcelImpl::Initialize()
{
	if (CoInitialize(NULL) != 0)
		return false;

	//COleException *e = new COleException;

	//try
	//{
		if(!m_xslApp.CreateDispatch("Excel.Application",NULL))
		{
			//throw e;
			return false;
		}
	//}
	//catch (COleDispatchException * e) 
	//{
	//	CString cStr;

	//	if (!e->m_strSource.IsEmpty())
	//		cStr = e->m_strSource + " - ";
	//	if (!e->m_strDescription.IsEmpty())
	//		cStr += e->m_strDescription;
	//	else
	//		cStr += "unknown error";

	//	//AfxMessageBox(cStr, MB_OK, 
	//	//	(e->m_strHelpFile.IsEmpty())? 0:e->m_dwHelpContext);

	//	e->Delete();
	//}

	m_xslApp.SetDisplayAlerts(false);			//��Ҫ������Ϣ�Ի���
	m_xslApp.SetAlertBeforeOverwriting(false);

	Attach(vtMissing);

	return true;
}

bool CExcelImpl::IsInitialized()
{
	return m_bInitialized;
}

void CExcelImpl::DeInitialize()
{
	Release();
	m_xslApp.SetUserControl(FALSE);
	m_xslApp.Quit();
	m_xslApp.ReleaseDispatch(); 
	CoUninitialize();
}

bool CExcelImpl::Open(const char* filePath)
{
	CFileFind finder;
	if (!finder.FindFile(filePath))
		return false;

	Attach(_variant_t(filePath));
	return true;
}

void CExcelImpl::Close()
{
	m_xslBook.Close(_variant_t(bool(false)), vtMissing, vtMissing);
	m_xslBooks.Close();
	Attach(vtMissing);
}

void CExcelImpl::SaveAs(const char* filePath)
{
	m_xslBook.SaveCopyAs(_variant_t(filePath));
}

void CExcelImpl::Reset()
{
	Detach();
	Attach(vtMissing);
}

void CExcelImpl::Attach(const _variant_t& var)
{
	m_xslBooks.AttachDispatch(m_xslApp.GetWorkbooks());
	m_xslBook.AttachDispatch(m_xslBooks.Add(var));
	m_xslSheets.AttachDispatch(m_xslBook.GetWorksheets());
	m_xslSheet.AttachDispatch(m_xslSheets.GetItem(_variant_t(long(1))));
	m_xslRange.AttachDispatch(m_xslSheet.GetCells());
}

void CExcelImpl::Detach()
{
	m_xslBooks.DetachDispatch();
	m_xslBook.DetachDispatch();
	m_xslSheets.DetachDispatch();
	m_xslSheet.DetachDispatch();
	m_xslRange.DetachDispatch();
}

void CExcelImpl::Release()
{
	m_xslBooks.ReleaseDispatch();
	m_xslBook.ReleaseDispatch();
	m_xslSheets.ReleaseDispatch();
	m_xslSheet.ReleaseDispatch();
	m_xslRange.ReleaseDispatch();
}


void CExcelImpl::SetCaption(const char* capName)
{
	m_xslApp.SetCaption(_T(capName));
}

std::string CExcelImpl::GetCaption()
{
    CString tmp = m_xslApp.GetCaption();
    std::string  rslt = tmp.GetBuffer(0);
    tmp.ReleaseBuffer();

    return rslt;
}

void CExcelImpl::SetItem(const _variant_t& rowIndex, const _variant_t& colIndex, const _variant_t& val)
{
	m_xslRange.SetItem(rowIndex, colIndex, val);
}

bool CExcelImpl::SetItem(const _variant_t& sheetIndex, const _variant_t& rowIndex, const _variant_t& colIndex, const _variant_t& val)
{
	int idx = FindSheet(sheetIndex);
	if (IsValidSheetIndex(idx))
	{
		_Worksheet tmpSheet;
		Range	   tmpRange;
		tmpSheet.AttachDispatch(m_xslSheets.GetItem(_variant_t(idx)));
		tmpRange.AttachDispatch(tmpSheet.GetCells());
		tmpRange.SetItem(rowIndex, colIndex, val);
		return true;
	}
	return false;
}

_variant_t CExcelImpl::GetItem(const _variant_t& rowIndex, const _variant_t& colIndex)
{
	return m_xslRange.GetItem(rowIndex, colIndex);
}

_variant_t CExcelImpl::GetItem(const _variant_t& sheetIndex, const _variant_t& rowIndex, const _variant_t& colIndex)
{
	int idx = FindSheet(sheetIndex);
	if (IsValidSheetIndex(idx))
	{
		_Worksheet tmpSheet;
		Range	   tmpRange;
		tmpSheet.AttachDispatch(m_xslSheets.GetItem(_variant_t(idx)));
		tmpRange.AttachDispatch(tmpSheet.GetCells());
		return tmpRange.GetItem(rowIndex, colIndex);
	}
	return vtMissing;
}

std::string CExcelImpl::GetAppName()
{
    CString tmp = m_xslApp.GetName();
	std::string  rslt = tmp.GetBuffer(0);
    tmp.ReleaseBuffer();

    return rslt;
}

std::string CExcelImpl::GetCurrentSheetName()
{
    CString tmp = m_xslSheet.GetName();
    std::string  rslt = tmp.GetBuffer(0);
    tmp.ReleaseBuffer();

    return rslt;
}

void CExcelImpl::SetCurrentSheetName(const char* sheetName)
{
	m_xslSheet.SetName(sheetName);
}

std::string CExcelImpl::GetCurrentWorkBookName()
{
    CString tmp = m_xslBook.GetName();
    std::string  rslt = tmp.GetBuffer(0);
    tmp.ReleaseBuffer();

    return rslt;
}

std::string CExcelImpl::GetCurrentRangeName()
{
    CString tmp = m_xslRange.GetName();
    std::string  rslt = tmp.GetBuffer(0);
    tmp.ReleaseBuffer();

    return rslt;
}

long CExcelImpl::GetSheetsCount()
{
	return m_xslSheets.GetCount();
}

long CExcelImpl::GetWorkBooksCount()
{
	return m_xslBooks.GetCount();
}

long CExcelImpl::GetCurrentRangeCount()
{
	return m_xslRange.GetCount();
}

long CExcelImpl::GetCurrentSheetIndex()
{
	return m_xslSheet.GetIndex();
}

bool CExcelImpl::ActiveSheet(const _variant_t& sheet)
{
	int idx = FindSheet(sheet);
	if (IsValidSheetIndex(idx))
	{
		m_xslSheet.AttachDispatch(m_xslSheets.GetItem(_variant_t(idx)));
		m_xslRange.AttachDispatch(m_xslSheet.GetCells());
		return true;
	}
	return false;
}

bool CExcelImpl::InsertSheetBefore(const _variant_t& sheet, const char* insertedSheetName)
{
	int idx = FindSheet(sheet);
	if (IsValidSheetIndex(idx))
	{
		_Worksheet tmpSheet;
		tmpSheet.AttachDispatch(m_xslSheets.GetItem(_variant_t(idx)));
		m_xslSheets.Add(_variant_t(tmpSheet), vtMissing, vtMissing, vtMissing);
		tmpSheet.AttachDispatch(m_xslSheets.GetItem(_variant_t(idx)));
		tmpSheet.SetName(insertedSheetName);
		return true;
	}
	return false;
}

bool CExcelImpl::InsertSheetAfter(const _variant_t& sheet, const char* insertedSheetName)
{
	int idx = FindSheet(sheet);
	if (IsValidSheetIndex(idx))
	{
		_Worksheet tmpSheet;
		tmpSheet.AttachDispatch(m_xslSheets.GetItem(_variant_t(idx)));
		m_xslSheets.Add(vtMissing, _variant_t(tmpSheet), vtMissing, vtMissing);
		tmpSheet.AttachDispatch(m_xslSheets.GetItem(_variant_t(idx+1)));
		tmpSheet.SetName(insertedSheetName);
		return true;
	}
	return false;
}

bool CExcelImpl::IsValidSheetIndex(int sheetIdx)
{
	if (sheetIdx >= 1 && sheetIdx <= GetSheetsCount())
		return true;
	return false;
}
void CExcelImpl::PushSheet(const char* pushedSheetName)
{
	int count = m_xslSheets.GetCount();
	_Worksheet tmpSheet;
	tmpSheet.AttachDispatch(m_xslSheets.GetItem(_variant_t(count)));
	m_xslSheets.Add(vtMissing, _variant_t(tmpSheet), vtMissing, vtMissing);
	tmpSheet.AttachDispatch(m_xslSheets.GetItem(_variant_t(count+1)));
	tmpSheet.SetName(pushedSheetName);
}

bool CExcelImpl::DeleteSheet(const _variant_t& deletedSheet)
{
	if (m_xslSheets.GetCount() == 1)
		return false;
	int idx = FindSheet(deletedSheet);
	if (IsValidSheetIndex(idx))
	{
		int tmpIdx = m_xslSheet.GetIndex();
		_Worksheet tmpSheet;
		tmpSheet.AttachDispatch(m_xslSheets.GetItem(_variant_t(idx)));
		tmpSheet.Delete();
		if (tmpIdx == idx)
		{
			m_xslSheet.AttachDispatch(m_xslSheets.GetItem(_variant_t(1)));
			m_xslRange.AttachDispatch(m_xslSheet.GetCells());
		}
		return true;
	}
	return false;
}

int CExcelImpl::FindSheet(const _variant_t& sheet)
{
	int count = m_xslSheets.GetCount();
	for (int i = 1; i <= count; ++i)
	{
		_Worksheet tmpSheet;
		tmpSheet.AttachDispatch(m_xslSheets.GetItem(_variant_t(i)));
		if (_variant_t(i) == sheet || _variant_t(tmpSheet.GetName()) == sheet)
			return i;
	}
	return 0;
}

//---------------------------------------------------------------------------------------------------------------
CExcel::CExcel(void):
    m_impl(new CExcelImpl)
{
}

CExcel::~CExcel(void)
{
    if (NULL != m_impl)
        delete m_impl;

    m_impl = NULL;
}

bool CExcel::IsInitialized()
{
    return m_impl->IsInitialized();
}

bool CExcel::Open(const char* filePath)
{
    return m_impl->Open(filePath);
}

void CExcel::Close()
{
    return m_impl->Close();
}

void CExcel::SaveAs(const char* filePath)
{
    return m_impl->SaveAs(filePath);
}

void CExcel::Reset()
{
    return m_impl->Reset();
}

void CExcel::SetCaption(const char* capName)
{
    return m_impl->SetCaption(capName);
}

std::string CExcel::GetCaption()
{
    return m_impl->GetCaption();
}

void CExcel::SetItem(const _variant_t& rowIndex, const _variant_t& colIndex, const _variant_t& val)
{
    return m_impl->SetItem(rowIndex, colIndex, val);
}

bool CExcel::SetItem(const _variant_t& sheetIndex, const _variant_t& rowIndex, const _variant_t& colIndex, const _variant_t& val)
{
    return m_impl->SetItem(sheetIndex, rowIndex, colIndex,val);
}

_variant_t CExcel::GetItem(const _variant_t& rowIndex, const _variant_t& colIndex)
{
    return m_impl->GetItem(rowIndex, colIndex);
}

_variant_t CExcel::GetItem(const _variant_t& sheetIndex, const _variant_t& rowIndex, const _variant_t& colIndex)
{
    return m_impl->GetItem(sheetIndex, rowIndex,colIndex);
}

std::string CExcel::GetAppName()
{
    return m_impl->GetAppName();
}

std::string CExcel::GetCurrentSheetName()
{
    return m_impl->GetCurrentSheetName();
}

void CExcel::SetCurrentSheetName(const char* sheetName)
{
    return m_impl->SetCurrentSheetName(sheetName);
}

std::string CExcel::GetCurrentWorkBookName()
{
    return m_impl->GetCurrentWorkBookName();
}

std::string CExcel::GetCurrentRangeName()
{
    return m_impl->GetCurrentRangeName();
}

long CExcel::GetSheetsCount()
{
    return m_impl->GetSheetsCount();
}

long CExcel::GetWorkBooksCount()
{
    return m_impl->GetWorkBooksCount();
}

long CExcel::GetCurrentRangeCount()
{
    return m_impl->GetCurrentRangeCount();
}

long CExcel::GetCurrentSheetIndex()
{
    return m_impl->GetCurrentSheetIndex();
}

bool CExcel::ActiveSheet(const _variant_t& sheet)
{
    return m_impl->ActiveSheet(sheet);
}

bool CExcel::InsertSheetBefore(const _variant_t& sheet, const char* insertedSheetName)
{
    return m_impl->InsertSheetBefore(sheet,insertedSheetName);
}

bool CExcel::InsertSheetAfter(const _variant_t& sheet, const char* insertedSheetName)
{
    return m_impl->InsertSheetAfter(sheet,insertedSheetName);
}

void CExcel::PushSheet(const char* pushedSheetName)
{
    return m_impl->PushSheet(pushedSheetName);
}

bool CExcel::DeleteSheet(const _variant_t& deletedSheet)
{
    return m_impl->DeleteSheet(deletedSheet);
}

int CExcel::FindSheet(const _variant_t& sheet)
{
    return m_impl->FindSheet(sheet);
}