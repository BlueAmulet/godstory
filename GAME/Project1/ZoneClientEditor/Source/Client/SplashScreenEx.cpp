#include "SplashScreenEx.h"

#ifndef AW_HIDE
	#define AW_HIDE 0x00010000
	#define AW_BLEND 0x00080000
#endif

#ifndef CS_DROPSHADOW
	#define CS_DROPSHADOW   0x00020000
#endif

LRESULT CALLBACK SplashWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    CSplashScreenEx* pSplashWnd = CSplashScreenEx::Instance();

    if (0 == pSplashWnd)
        return TRUE;

    MSG msg;
    ZeroMemory(&msg,sizeof(msg));

    msg.message = message;
    msg.hwnd    = hWnd;
    msg.wParam  = wParam;
    msg.lParam  = lParam;

    pSplashWnd->PreTranslateMessage(&msg);

    switch(message)
    {
    case WM_PAINT:
        {
            pSplashWnd->OnPaint();
        }
        break;
    case WM_PRINTCLIENT:
        {
            pSplashWnd->OnPrintClient(wParam,lParam);
        }
        break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

CSplashScreenEx::CSplashScreenEx()
{
	m_hRegion=0;
	m_nBitmapWidth=0;
	m_nBitmapHeight=0;
	m_nxPos=0;
	m_nyPos=0;
	m_dwTimeout=2000;
	m_dwStyle=0;
    m_isEnd = false;
	

	HMODULE hUser32 = GetModuleHandleA("USER32.DLL");

	if (hUser32!=NULL)
		m_fnAnimateWindow = (FN_ANIMATE_WINDOW)GetProcAddress(hUser32, "AnimateWindow");
	else
		m_fnAnimateWindow = NULL;
}

CSplashScreenEx::~CSplashScreenEx()
{
    m_isEnd = true;
}

BOOL CSplashScreenEx::Create(LPCTSTR szText,DWORD dwTimeout,DWORD dwStyle)
{
	m_dwTimeout = dwTimeout;
	m_dwStyle = dwStyle;
	
	WNDCLASSEX wcx; 

	wcx.cbSize = sizeof(wcx);
	wcx.lpfnWndProc = SplashWinProc;
	wcx.style = CS_DBLCLKS|CS_SAVEBITS;
	wcx.cbClsExtra = 0;
	wcx.cbWndExtra = 0;
	wcx.hInstance = 0;
	wcx.hIcon = NULL;
	wcx.hCursor = LoadCursor(NULL,IDC_ARROW);
	wcx.hbrBackground=::GetSysColorBrush(COLOR_WINDOW);
	wcx.lpszMenuName = NULL;
	wcx.lpszClassName = TEXT("SplashScreenExClass");
	wcx.hIconSm = NULL;

	if (m_dwStyle & CSS_SHADOW)
		wcx.style|=CS_DROPSHADOW;

	ATOM classAtom = RegisterClassEx(&wcx);
      
	// didn't work? try not using dropshadow (may not be supported)

	if (classAtom==NULL)
	{
		if (m_dwStyle & CSS_SHADOW)
		{
			wcx.style &= ~CS_DROPSHADOW;
			classAtom = RegisterClassEx(&wcx);
		}
		else
			return FALSE;
	}

    DWORD extStyle = WS_EX_TOOLWINDOW;

#ifdef RELEASE
    extStyle |= WS_EX_TOPMOST;
#endif

    m_hWnd = CreateWindowEx(
        extStyle,
        TEXT("SplashScreenExClass"),              //class name
        L"",                                //window title
        WS_POPUP | WS_VISIBLE,              //style - need clip siblings/children for opengl
        0,
        0,
        0,
        0,
        0,                                 //parent window
        NULL,                              //menu? No.
        NULL,                              //the hInstance
        NULL );                            //no funky params

	return TRUE;
}

BOOL CSplashScreenEx::SetBitmap(LPCTSTR szFileName,short red,short green,short blue)
{
	BITMAP bm;
	
	m_bitmap = (HBITMAP)::LoadImage(0,szFileName,IMAGE_BITMAP,0,0, LR_LOADFROMFILE);

	if (!m_bitmap)
		return FALSE;
	
	GetObject(m_bitmap, sizeof(bm), &bm);
	m_nBitmapWidth  = bm.bmWidth;
	m_nBitmapHeight = bm.bmHeight;
	
	if (m_dwStyle & CSS_CENTERSCREEN)
	{
		m_nxPos=(GetSystemMetrics(SM_CXFULLSCREEN)-bm.bmWidth)/2;
		m_nyPos=(GetSystemMetrics(SM_CYFULLSCREEN)-bm.bmHeight)/2;
	}
	else if (m_dwStyle & CSS_CENTERAPP)
	{
		/*CRect rcParentWindow;
		ASSERT(m_pWndParent!=NULL);
		m_pWndParent->GetWindowRect(&rcParentWindow);
		m_nxPos=rcParentWindow.left+(rcParentWindow.right-rcParentWindow.left-bm.bmWidth)/2;
		m_nyPos=rcParentWindow.top+(rcParentWindow.bottom-rcParentWindow.top-bm.bmHeight)/2;*/
	}

	return TRUE;
}

void CSplashScreenEx::Show()
{
	SetWindowPos(m_hWnd,NULL,m_nxPos,m_nyPos,m_nBitmapWidth,m_nBitmapHeight,SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOACTIVATE);

	if ((m_dwStyle & CSS_FADEIN) && (m_fnAnimateWindow!=NULL))
	{
		m_fnAnimateWindow(m_hWnd,500,AW_BLEND);
	}
	else
		ShowWindow(m_hWnd,SW_SHOW);
	
	if (m_dwTimeout!=0)
		SetTimer(m_hWnd,0,m_dwTimeout,NULL);

    OnPaint();
}

void CSplashScreenEx::Hide()
{
	if ((m_dwStyle & CSS_FADEOUT) && (m_fnAnimateWindow!=NULL))
		m_fnAnimateWindow(m_hWnd,200,AW_HIDE | AW_BLEND);
	else
		ShowWindow(m_hWnd,SW_HIDE);

	DestroyWindow(m_hWnd);
    m_hWnd = 0;
    m_isEnd = true;
}

void CSplashScreenEx::DrawWindow(HDC hdc)
{
    if (!m_bitmap)
        return;

    HDC memHDC = CreateCompatibleDC(hdc);
    HBITMAP pOldBitmap=(HBITMAP)SelectObject(memHDC,m_bitmap);

    BitBlt(hdc,0,0,m_nBitmapWidth,m_nBitmapHeight,memHDC,0,0,SRCCOPY);
    SelectObject(memHDC,pOldBitmap);
	DeleteDC(memHDC);
}

void CSplashScreenEx::OnPaint()
{
	DrawWindow(GetDC(m_hWnd));
}

LRESULT CSplashScreenEx::OnPrintClient(WPARAM wParam, LPARAM lParam)
{
	DrawWindow((HDC)wParam);
	return 1;
}

BOOL CSplashScreenEx::PreTranslateMessage(MSG* pMsg)
{
	// If a key is pressed, Hide the Splash Screen and destroy it
	if (m_dwStyle & CSS_HIDEONCLICK)
	{
		if (pMsg->message == WM_KEYDOWN ||
			pMsg->message == WM_SYSKEYDOWN ||
			pMsg->message == WM_LBUTTONDOWN ||
			pMsg->message == WM_RBUTTONDOWN ||
			pMsg->message == WM_MBUTTONDOWN ||
			pMsg->message == WM_NCLBUTTONDOWN ||
			pMsg->message == WM_NCRBUTTONDOWN ||
			pMsg->message == WM_NCMBUTTONDOWN)
		{
			Hide();
		}
	}

    return TRUE;
}