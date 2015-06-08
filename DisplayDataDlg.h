
// DisplayDataDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "SerialPort.h"


// CDisplayDataDlg dialog
class CDisplayDataDlg : public CDialogEx
{
// Construction
public:
	//图像显示变量
	HWND m_hPE; //采集数据图像显示
	RECT m_rect;
	int m_nRealTimeCounter;//
	int m_nSinCounter;

	double StartTime;
	double ZoomStart;
	double ZoomEnd;
	double ZoomInterval;
	double ZoomSmallInterval; 

	//串口通信
	CSerialPort m_SerialComm;    //CSerailPort类对象
	int nPortOpened;
//		int nRecBuf[20];
//		int nRecIndex;
	int nDisplayData;

//数据存储
	int m_bStartSave;


	CDisplayDataDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_DISPLAYDATA_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg LONG OnComm(WPARAM ch, LPARAM port);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonFilePath();
	CString m_Edit_save_path;
//	CComboBox m_ComPortNum;
//	CComboBox m_ComBaudrate;
	afx_msg void OnCbnSelchangeComboPortnum();
//	CString m_PortNum;
	CComboBox m_ComBaurate;
	CComboBox m_ComPortNum;
	afx_msg void OnBnClickedButtonComOpen();
	afx_msg void OnBnClickedButtonDatSave();
	void RealtimeDisplay_Init(void);
	void StaticDisplay_Init(void);
	afx_msg void OnBnClickedButtonDatRedisplay();
//	CString m_Edit_ReDisp_FilePath;
	CString m_Edit_ReDisp_FilePath;
};
