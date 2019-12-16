// ServerManagerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ServerManager.h"
#include "ServerManagerDlg.h"
#include ".\servermanagerdlg.h"

#include ".\ConfigDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CServerManagerDlg 对话框

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


// CServerManagerDlg 消息处理程序

BOOL CServerManagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bIsFirstShow = true;

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_bStarted = false;

	CString str;
	GetCurrentPath( str );
	
	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CServerManagerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
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

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CServerManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CServerManagerDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialog::OnOK();
}

void CServerManagerDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialog::OnCancel();
}

void CServerManagerDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码

	::Shell_NotifyIcon(NIM_DELETE,&m_nid);

	StopServer();
}

void CServerManagerDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码

	if( !m_bStarted )
	{
		SetDlgItemText( IDC_BUTTON1, "停止服务" );
		StartServer();
	}
	else
	{
		SetDlgItemText( IDC_BUTTON1, "启动服务" );
		StopServer();
	}
}

void CServerManagerDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码

	if( m_bStarted )
	{
		AfxMessageBox( "服务器已启动,您必须先停止服务才能配置服务端程序" );\
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
		SetDlgItemText( IDC_BUTTON1, "停止服务" );
	}
	else
	{
		SetDlgItemText( IDC_BUTTON1, "启动服务" );
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
		SetDlgItemText( IDC_BUTTON2, "显示服务窗口" );
	}
	else
	{
		SetDlgItemText( IDC_BUTTON2, "隐藏服务窗口" );
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
	// TODO: 在此添加控件通知处理程序代码

	if( g_bShowWindow )
	{
		HideAllWindow();
		g_bShowWindow = false;
		SetDlgItemText( IDC_BUTTON2, "显示服务窗口" );
	}
	else
	{
		ShowAllWindow();
		g_bShowWindow = true;
		SetDlgItemText( IDC_BUTTON2, "隐藏服务窗口" );
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

	//自定义的消息名称 WM_SHOWTASK 头函数中定义为WM_USER+1
	m_nid.hIcon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME)); 
	strcpy(m_nid.szTip,"神仙传服务端管理");//当鼠标放在上面时，所显示的内容 
	Shell_NotifyIcon(NIM_ADD,&m_nid);//在托盘区添加图标 

}

LRESULT CServerManagerDlg::onShowTask(WPARAM wParam,LPARAM lParam)
{
	if(wParam!=IDR_MAINFRAME) 
		return 1; 
	switch(lParam) 
	{ 
	case WM_RBUTTONUP://右键起来时弹出快捷菜单，这里只有一个“关闭” 
		{   
			CMenu menu, *pSubMenu;
			//加载菜单
			if(!menu.LoadMenu(IDR_MENU1))
			{
				return 1;
			}
			//加载菜单中的第一项
			if(!(pSubMenu = menu.GetSubMenu(0)))
			{
				return 1;
			}

			CPoint pos;
			::GetCursorPos(&pos);
			//::SetForegroundWindow(m_NotifyIconData.hWnd);
			//设置菜单的显示位置
			::TrackPopupMenu(pSubMenu->m_hMenu,0,pos.x,pos.y,0,this->GetSafeHwnd(),NULL);
			menu.DestroyMenu();

		} 
		break; 
	case WM_LBUTTONDBLCLK://双击左键的处理 
		{     
			this->ShowWindow(SW_SHOW);//简单的显示主窗口
		} 
		break; 
	} 
	return 0; 

}
void CServerManagerDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	ToTray();
}

void CServerManagerDlg::OnSystem32774()
{
	// TODO: 在此添加命令处理程序代码
	// 这是一个退出信息


	::Shell_NotifyIcon(NIM_DELETE,&m_nid);

	this->DestroyWindow();
}

void CServerManagerDlg::OnSystem32772()
{
	// TODO: 在此添加命令处理程序代码
	StartServer();
}

void CServerManagerDlg::OnSystem32775()
{
	// TODO: 在此添加命令处理程序代码
	StopServer();
}

void CServerManagerDlg::OnSystem32773()
{
	// TODO: 在此添加命令处理程序代码
	ShowAllWindow();
}

void CServerManagerDlg::OnSystem32776()
{
	// TODO: 在此添加命令处理程序代码
	HideAllWindow();
}

void CServerManagerDlg::OnSystem32777()
{
	// TODO: 在此添加命令处理程序代码
	::Shell_NotifyIcon(NIM_DELETE,&m_nid);

	ShowWindow( SW_SHOW );
}

void CServerManagerDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	// TODO: 在此处添加消息处理程序代码
	CDialog::OnShowWindow(bShow, nStatus);


}

int CServerManagerDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码

	return 0;
}
