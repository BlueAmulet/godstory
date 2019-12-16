// ConfigDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ServerManager.h"
#include "ConfigDlg.h"
#include ".\configdlg.h"


// CConfigDlg 对话框



IMPLEMENT_DYNCREATE(CConfigDlg, CDialog)

CConfigDlg::CConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigDlg::IDD, pParent)
{
}

CConfigDlg::~CConfigDlg()
{
}

void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK1, m_bCheckedCtrl);
}

BOOL CConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString strCurrentPath;
	GetCurrentPath( strCurrentPath );
	strCurrentPath += "\\Config.ini";

	CString strPath;
	char szPath[100] = { 0 };
	
	// WorldServer
	this->SetDlgItemText( IDC_EDIT1, GetConfigPath("WorldServer") );

	// AccountServer
	::GetPrivateProfileString( "Paths", "AccountServer", NULL, szPath, 100, strCurrentPath );
	this->SetDlgItemText( IDC_EDIT2, szPath );

	// NetGateServer
	::GetPrivateProfileString( "Paths", "NetGateServer", NULL, szPath, 100, strCurrentPath );
	this->SetDlgItemText( IDC_EDIT3, szPath );

	// DataAgentServer
	::GetPrivateProfileString( "Paths", "DataAgentServer", NULL, szPath, 100,strCurrentPath );
	this->SetDlgItemText( IDC_EDIT4, szPath );

	DWORD dwChecked = ::GetPrivateProfileInt( "Others", "ShowWindow", 0, strCurrentPath );
	m_bCheckedCtrl.SetCheck( dwChecked );

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

BEGIN_MESSAGE_MAP(CConfigDlg, CDialog)

	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, OnBnClickedButton5)
END_MESSAGE_MAP()




void CConfigDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码

	CString strCurrentPath;
	GetCurrentPath( strCurrentPath );
	strCurrentPath += "\\Config.ini";

	CString strPath;

	GetDlgItemText( IDC_EDIT1, strPath );
	::WritePrivateProfileString( "Paths", "WorldServer", strPath, strCurrentPath );

	GetDlgItemText( IDC_EDIT2, strPath );
	::WritePrivateProfileString( "Paths", "AccountServer", strPath, strCurrentPath );

	GetDlgItemText( IDC_EDIT3, strPath );
	::WritePrivateProfileString( "Paths", "NetGateServer", strPath, strCurrentPath );

	GetDlgItemText( IDC_EDIT4, strPath );
	::WritePrivateProfileString( "Paths", "DataAgentServer", strPath, strCurrentPath );

	int nChecked = m_bCheckedCtrl.GetCheck();
	WritePrivateProfileString( "Others", "ShowWindow", nChecked?"1":"0", strCurrentPath );
	
	OnOK();
}

void CConfigDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码

	CFileDialog dlg( TRUE, NULL, NULL, OFN_READONLY, "WorldServer可执行文件|*.exe" );

	if( dlg.DoModal() == IDOK )
	{
		CString strPath = dlg.GetPathName();

		SetDlgItemText( IDC_EDIT1, strPath );
	}
}

void CConfigDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog dlg( TRUE, NULL, NULL, OFN_READONLY, "AccountServer可执行文件|*.exe" );

	if( dlg.DoModal() == IDOK )
	{
		CString strPath = dlg.GetPathName();

		SetDlgItemText( IDC_EDIT2, strPath );
	}
}

void CConfigDlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog dlg( TRUE, NULL, NULL, OFN_READONLY, "NetGateServer可执行文件|*.exe" );

	if( dlg.DoModal() == IDOK )
	{
		CString strPath = dlg.GetPathName();

		SetDlgItemText( IDC_EDIT3, strPath );
	}
}

void CConfigDlg::OnBnClickedButton5()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog dlg( TRUE, NULL, NULL, OFN_READONLY, "DataAgentServer可执行文件|*.exe" );

	if( dlg.DoModal() == IDOK )
	{
		CString strPath = dlg.GetPathName();

		SetDlgItemText( IDC_EDIT4, strPath );
	}
}
