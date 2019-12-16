#pragma once

#include <Windows.h>

#define CSS_FADEIN		0x0001
#define CSS_FADEOUT		0x0002
#define CSS_FADE		CSS_FADEIN | CSS_FADEOUT
#define CSS_SHADOW		0x0004
#define CSS_CENTERSCREEN	0x0008
#define CSS_CENTERAPP		0x0010
#define CSS_HIDEONCLICK		0x0020

#define CSS_TEXT_NORMAL		0x0000
#define CSS_TEXT_BOLD		0x0001
#define CSS_TEXT_ITALIC		0x0002
#define CSS_TEXT_UNDERLINE	0x0004

typedef BOOL (WINAPI* FN_ANIMATE_WINDOW)(HWND,DWORD,DWORD);

// CSplashScreenEx

class CSplashScreenEx
{
public:
	CSplashScreenEx();
	virtual ~CSplashScreenEx();

	BOOL Create(LPCTSTR szText=NULL,DWORD dwTimeout=2000,DWORD dwStyle=CSS_FADE | CSS_CENTERSCREEN | CSS_SHADOW);
	BOOL SetBitmap(LPCTSTR szFileName,short red=-1,short green=-1,short blue=-1);

	void Show();
	void Hide();

    static CSplashScreenEx* Instance()
    {
        static CSplashScreenEx local;
        return &local;
    }
protected:	
	FN_ANIMATE_WINDOW m_fnAnimateWindow;

	HBITMAP m_bitmap;
	HRGN m_hRegion;
	
	DWORD m_dwStyle;
	DWORD m_dwTimeout;
	
	int m_nBitmapWidth;
	int m_nBitmapHeight;
	int m_nxPos;
	int m_nyPos;

   
	void DrawWindow(HDC pDC);

public:
    bool m_isEnd;
    HWND m_hWnd;

	void OnPaint();
	LRESULT OnPrintClient(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


