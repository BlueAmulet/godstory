// ServerManagerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ServerManager.h"
#include "ServerManagerDlg.h"
#include ".\servermanagerdlg.h"

#include ".\ConfigDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CServerManagerDlg �Ի���

HANDLE g_hWorldServer = 0;
HANDLE g_hAccountServer = 0;
HANDLE g_hNetGateServer = 0;
HANDLE g_hDataAgentServer = 0;

DWORD g_dwWorldServer = 0;
DWORD g_dwAccountServer = 0;
DWORD g_dwNetGateServer = 0;
DWORD g_dwDataAgentServer = 0;

BOOL g_bShowWindow;

CServerManagerDlg::CServerManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CServerManagerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CServerManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CServerManagerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_WM_CLOSE()
	ON_MESSAGE(WM_SHOWTASK,onShowTask)
	ON_COMMAND(ID_SYSTEM_32774, OnSystem32774)
	ON_COMMAND(ID_SYSTEM_32772, OnSystem32772)
	ON_COMMAND(ID_SYSTEM_32775, OnSystem32775)
	ON_COMMAND(ID_SYSTEM_32773, OnSystem32773)
	ON_COMMAND(ID_SYSTEM_32776, OnSystem32776)
	ON_COMMAND(ID_SYSTEM_32777, OnSystem32777)
	ON_WM_SHOWWINDOW()
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CServerManagerDlg ��Ϣ�������

BOOL CServerManagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bIsFirstShow = true;

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_bStarted = false;

	CString str;
	GetCurrentPath( str );
	
	return TRUE;  // ���������˿ؼ��Ľ��㣬���򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CServerManagerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		if( m_bIsFirstShow )
		{
			ToTray();
			m_bIsFirstShow = false;
		}
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
HCURSOR CServerManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CServerManagerDlg::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���

	CDialog::OnOK();
}

void CServerManagerDlg::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���

	CDialog::OnCancel();
}

void CServerManagerDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: �ڴ˴������Ϣ����������

	::Shell_NotifyIcon(NIM_DELETE,&m_nid);

	StopServer();
}

void CServerManagerDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	if( !m_bStarted )
	{
		SetDlgItemText( IDC_BUTTON1, "ֹͣ����" );
		StartServer();
	}
	else
	{
		SetDlgItemText( IDC_BUTTON1, "��������" );
		StopServer();
	}
}

void CServerManagerDlg::OnBnClickedButton3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	if( m_bStarted )
	{
		AfxMessageBox( "������������,��������ֹͣ����������÷���˳���" );\
		return ;
	}

	CConfigDlg dlg;
	dlg.DoModal();
}

void CServerManagerDlg::StartServer(void)
{

	m_bStarted = true;

	if( m_bStarted )
	{
		SetDlgItemText( IDC_BUTTON1, "ֹͣ����" );
	}
	else
	{
		SetDlgItemText( IDC_BUTTON1, "��������" );
	}

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	CString strPathName;
	CString strPath;

	GetCurrentPath( strPath );
	DWORD dwShowWindow = ::GetPrivateProfileInt( "Others", "ShowWindow", 0, strPath + "\\Config.ini" );
	
	g_bShowWindow = dwShowWindow;

	if( g_bShowWindow )
	{
		SetDlgItemText( IDC_BUTTON2, "��ʾ���񴰿�" );
	}
	else
	{
		SetDlgItemText( IDC_BUTTON2, "���ط��񴰿�" );
	}

	ZeroMemory( &pi, sizeof(PROCESS_INFORMATION) );
	ZeroMemory( &si, sizeof(STARTUPINFO) );
	strPathName = GetConfigPath( "WorldServer" );
	strPath = GetPath( strPathName );
	if( dwShowWindow == 0 )
	{
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;
	}
	CreateProcessA( strPathName, NULL, NULL, NULL, FALSE, NULL, NULL, strPath, &si, &pi );
	g_hWorldServer = pi.hProcess;
	g_dwWorldServer = pi.dwProcessId;

	ZeroMemory( &pi, sizeof(PROCESS_INFORMATION) );
	ZeroMemory( &si, sizeof(STARTUPINFO) );
	strPathName = GetConfigPath( "AccountServer" );
	strPath = GetPath( strPathName );
	if( dwShowWindow == 0 )
	{
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;
	}
	CreateProcessA( strPathName, NULL, NULL, NULL, FALSE, NULL, NULL, strPath, &si, &pi );
	g_hAccountServer = pi.hProcess;
	g_dwAccountServer = pi.dwProcessId;

	ZeroMemory( &pi, sizeof(PROCESS_INFORMATION) );
	ZeroMemory( &si, sizeof(STARTUPINFO) );
	strPathName = GetConfigPath( "NetGateServer" );
	strPath = GetPath( strPathName );
	if( dwShowWindow == 0 )
	{
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;
	}
	CreateProcessA( strPathName, NULL, NULL, NULL, FALSE, NULL, NULL, strPath, &si, &pi );
	g_hNetGateServer = pi.hProcess;
	g_dwNetGateServer = pi.dwProcessId;

	ZeroMemory( &pi, sizeof(PROCESS_INFORMATION) );
	ZeroMemory( &si, sizeof(STARTUPINFO) );
	strPathName = GetConfigPath( "DataAgentServer" );
	strPath = GetPath( strPathName );
	if( dwShowWindow == 0 )
	{
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;
	}
	CreateProcessA( strPathName, NULL, NULL, NULL, FALSE, NULL, NULL, strPath, &si, &pi );
	g_hDataAgentServer = pi.hProcess;
	g_dwDataAgentServer = pi.dwProcessId;
}

void CServerManagerDlg::StopServer(void)
{
	m_bStarted = false;

	if( g_hWorldServer )
		TerminateProcess( g_hWorldServer, 0 );

	if( g_hAccountServer )
		TerminateProcess( g_hAccountServer, 0 );

	if( g_hNetGateServer )
		TerminateProcess( g_hNetGateServer, 0 );

	if( g_hDataAgentServer )
		TerminateProcess( g_hDataAgentServer, 0 );
}

BOOL CALLBACK EnumWindow_Show( HWND hWnd, LPARAM lParam )
{
	char buf[100];
	GetWindowText( hWnd, buf, 100 );
	CString strName = buf;
	DWORD dwId;
	GetWindowThreadProcessId( hWnd, &dwId );

	if( dwId == g_dwWorldServer 
		|| dwId == g_dwAccountServer 
		|| dwId == g_dwNetGateServer 
		|| dwId == g_dwDataAgentServer )
		ShowWindow( hWnd, SW_SHOW );
		
	return TRUE;
}

BOOL CALLBACK EnumWindow_Hide( HWND hWnd, LPARAM lParam )
{
	char buf[100];
	GetWindowText( hWnd, buf, 100 );
	CString strName = buf;
	DWORD dwId;
	GetWindowThreadProcessId( hWnd, &dwId );

	if( dwId == g_dwWorldServer 
		|| dwId == g_dwAccountServer 
		|| dwId == g_dwNetGateServer 
		|| dwId == g_dwDataAgentServer )
		ShowWindow( hWnd, SW_HIDE );

	return TRUE;
}

void CServerManagerDlg::ShowAllWindow(void)
{
	EnumWindows( EnumWindow_Show, NULL );
}

void CServerManagerDlg::HideAllWindow(void)
{
	EnumWindows( EnumWindow_Hide, NULL );
}

void CServerManagerDlg::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	if( g_bShowWindow )
	{
		HideAllWindow();
		g_bShowWindow = false;
		SetDlgItemText( IDC_BUTTON2, "��ʾ���񴰿�" );
	}
	else
	{
		ShowAllWindow();
		g_bShowWindow = true;
		SetDlgItemText( IDC_BUTTON2, "���ط��񴰿�" );
	}
}

void CServerManagerDlg::ToTray(void)
{
	ShowWindow( SW_HIDE );

	m_nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA); 
	m_nid.hWnd = this->m_hWnd; 
	m_nid.uID = IDR_MAINFRAME; 
	m_nid.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP ; 
	m_nid.uCallbackMessage = WM_SHOWTASK;

	//�Զ������Ϣ���� WM_SHOWTASK ͷ�����ж���ΪWM_USER+1
	m_nid.hIcon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME)); 
	strcpy(m_nid.szTip,"���ɴ�����˹���");//������������ʱ������ʾ������ 
	Shell_NotifyIcon(NIM_ADD,&m_nid);//�����������ͼ�� 

}

LRESULT CServerManagerDlg::onShowTask(WPARAM wParam,LPARAM lParam)
{
	if(wParam!=IDR_MAINFRAME) 
		return 1; 
	switch(lParam) 
	{ 
	case WM_RBUTTONUP://�Ҽ�����ʱ������ݲ˵�������ֻ��һ�����رա� 
		{   
			CMenu menu, *pSubMenu;
			//���ز˵�
			if(!menu.LoadMenu(IDR_MENU1))
			{
				return 1;
			}
			//���ز˵��еĵ�һ��
			if(!(pSubMenu = menu.GetSubMenu(0)))
			{
				return 1;
			}

			CPoint pos;
			::GetCursorPos(&pos);
			//::SetForegroundWindow(m_NotifyIconData.hWnd);
			//���ò˵�����ʾλ��
			::TrackPopupMenu(pSubMenu->m_hMenu,0,pos.x,pos.y,0,this->GetSafeHwnd(),NULL);
			menu.DestroyMenu();

		} 
		break; 
	case WM_LBUTTONDBLCLK://˫������Ĵ��� 
		{     
			this->ShowWindow(SW_SHOW);//�򵥵���ʾ������
		} 
		break; 
	} 
	return 0; 

}
void CServerManagerDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	ToTray();
}

void CServerManagerDlg::OnSystem32774()
{
	// TODO: �ڴ���������������
	// ����һ���˳���Ϣ


	::Shell_NotifyIcon(NIM_DELETE,&m_nid);

	this->DestroyWindow();
}

void CServerManagerDlg::OnSystem32772()
{
	// TODO: �ڴ���������������
	StartServer();
}

void CServerManagerDlg::OnSystem32775()
{
	// TODO: �ڴ���������������
	StopServer();
}

void CServerManagerDlg::OnSystem32773()
{
	// TODO: �ڴ���������������
	ShowAllWindow();
}

void CServerManagerDlg::OnSystem32776()
{
	// TODO: �ڴ���������������
	HideAllWindow();
}

void CServerManagerDlg::OnSystem32777()
{
	// TODO: �ڴ���������������
	::Shell_NotifyIcon(NIM_DELETE,&m_nid);

	ShowWindow( SW_SHOW );
}

void CServerManagerDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	// TODO: �ڴ˴������Ϣ����������
	CDialog::OnShowWindow(bShow, nStatus);


}

int CServerManagerDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������

	return 0;
}
