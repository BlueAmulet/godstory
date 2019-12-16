// ServerManagerDlg.h : 头文件
//

#pragma once

#define WM_SHOWTASK WM_USER+100

// CServerManagerDlg 对话框
class CServerManagerDlg : public CDialog
{
// 构造
public:
	CServerManagerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SERVERMANAGER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	bool m_bStarted;
	bool m_bIsFirstShow;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton3();
	void StartServer(void);
	void StopServer(void);
	void ShowAllWindow(void);
	void HideAllWindow(void);
	afx_msg void OnBnClickedButton2();
	void ToTray(void);

	LRESULT onShowTask(WPARAM wParam,LPARAM lParam);
	NOTIFYICONDATA m_nid;
	afx_msg void OnClose();
	afx_msg void OnSystem32774();
	afx_msg void OnSystem32772();
	afx_msg void OnSystem32775();
	afx_msg void OnSystem32773();
	afx_msg void OnSystem32776();
	afx_msg void OnSystem32777();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
