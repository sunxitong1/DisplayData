
// DisplayDataDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DisplayData.h"
#include "DisplayDataDlg.h"
#include "afxdialogex.h"
#include "Pegrpapi.h "
#include "math.h"
#include "SerialPort.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDisplayDataDlg dialog




CDisplayDataDlg::CDisplayDataDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDisplayDataDlg::IDD, pParent)
	, m_Edit_save_path(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDisplayDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FILE_PATH, m_Edit_save_path);
	//  DDX_CBString(pDX, IDC_COMBO_PORTNUM, m_PortNum);
	DDX_Control(pDX, IDC_COMBO_BAUD, m_ComBaurate);
	DDX_Control(pDX, IDC_COMBO_PORTNUM, m_ComPortNum);
}

BEGIN_MESSAGE_MAP(CDisplayDataDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_FILE_PATH, &CDisplayDataDlg::OnBnClickedButtonFilePath)
	ON_BN_CLICKED(IDC_BUTTON_COM_OPEN, &CDisplayDataDlg::OnBnClickedButtonComOpen)
	ON_MESSAGE(WM_COMM_RXCHAR,OnComm)
	ON_BN_CLICKED(IDC_BUTTON_DAT_SAVE, &CDisplayDataDlg::OnBnClickedButtonDatSave)
END_MESSAGE_MAP()


// CDisplayDataDlg message handlers

BOOL CDisplayDataDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_hPE = NULL;

	RECT rect;

		//��ȡͼ����ʾ��Χ
	//RECT rect;
	CStatic *pST=(CStatic *)GetDlgItem(IDC_STATIC_DISP);
	pST->GetWindowRect(&rect);
	ScreenToClient(&rect);
	//UpdateData(TRUE);
   //GetClientRect( &rect );

    m_hPE = PEcreate(PECONTROL_SGRAPH, WS_VISIBLE, &rect, m_hWnd, 1001);

   PEnset(m_hPE, PEP_nSUBSETS, 1);
   PEnset(m_hPE, PEP_nPOINTS, 300);

   // Set Manual Y scale //
   PEnset(m_hPE, PEP_nMANUALSCALECONTROLY, PEMSC_MINMAX);
   double arg = 1.0F;
   PEvset(m_hPE, PEP_fMANUALMINY, &arg, 1);
   arg = 300.0F;
   PEvset(m_hPE, PEP_fMANUALMAXY, &arg, 1);

   // Set Manual X scale //
   PEnset(m_hPE, PEP_nMANUALSCALECONTROLX, PEMSC_MINMAX);
   arg = 1.0F;
   PEvset(m_hPE, PEP_fMANUALMINX, &arg, 1);
   arg = 300;
   PEvset(m_hPE, PEP_fMANUALMAXX, &arg, 1);

   // Clear out default data //
   float val = 0;
   PEvsetcellEx(m_hPE, PEP_faXDATA, 0, 0, &val);
   PEvsetcellEx(m_hPE, PEP_faXDATA, 0, 1, &val);
   PEvsetcellEx(m_hPE, PEP_faXDATA, 0, 2, &val);
   PEvsetcellEx(m_hPE, PEP_faXDATA, 0, 3, &val);

   PEvsetcellEx(m_hPE, PEP_faYDATA, 0, 0, &val);
   PEvsetcellEx(m_hPE, PEP_faYDATA, 0, 1, &val);
   PEvsetcellEx(m_hPE, PEP_faYDATA, 0, 2, &val);
   PEvsetcellEx(m_hPE, PEP_faYDATA, 0, 3, &val);

   // Set Various Other Properties ///
   PEnset(m_hPE, PEP_bBITMAPGRADIENTMODE, FALSE);
   PEnset(m_hPE, PEP_nQUICKSTYLE, PEQS_MEDIUM_INSET);

   PEszset(m_hPE, PEP_szMAINTITLE, TEXT("����ʵʱ���"));

   PEszset(m_hPE, PEP_szSUBTITLE, TEXT(""));
   PEnset(m_hPE, PEP_bNORANDOMPOINTSTOEXPORT, TRUE);
   PEnset(m_hPE, PEP_bFOCALRECT, FALSE);
   PEnset(m_hPE, PEP_bALLOWBAR, FALSE);
   PEnset(m_hPE, PEP_bALLOWPOPUP, FALSE);
   PEnset(m_hPE, PEP_bPREPAREIMAGES, TRUE);
   PEnset(m_hPE, PEP_bCACHEBMP, TRUE);
   PEnset(m_hPE, PEP_bFIXEDFONTS, TRUE);

   DWORD col = PERGB( 255,0, 198, 0);
   PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, 0, &col);

   PEnset(m_hPE, PEP_nGRADIENTBARS, 8);
   PEnset(m_hPE, PEP_nTEXTSHADOWS, PETS_BOLD_TEXT);
   PEnset(m_hPE, PEP_bMAINTITLEBOLD, TRUE);
   PEnset(m_hPE, PEP_bSUBTITLEBOLD, TRUE);
   PEnset(m_hPE, PEP_bLABELBOLD, TRUE);
   PEnset(m_hPE, PEP_bLINESHADOWS, TRUE);
   PEnset(m_hPE, PEP_nFONTSIZE, PEFS_MEDIUM);

   // Improves metafile export //
   PEnset(m_hPE, PEP_nDPIX, 600);
   PEnset(m_hPE, PEP_nDPIY, 600);

   PEreinitialize(m_hPE);
   PEresetimage(m_hPE, 0, 0);

   // Demo's Logic controlling RenderEngine // 
   CMDIFrameWnd* pWnd = (CMDIFrameWnd*) AfxGetApp()->GetMainWnd();
   //pWnd->SendMessage(WM_CHANGE_METAFILE, PEPLAYMETAFILE );
   	m_nRealTimeCounter = 1;
	m_nSinCounter = 1;
   


//����Ĭ��·��
   	m_Edit_save_path = _T("d:\\DataSample.txt"); 
	SetDlgItemText(IDC_EDIT_FILE_PATH,m_Edit_save_path);//�ɼ�����Ĭ�ϴ洢·��	
	
//���ô���
  	m_ComPortNum.AddString(_T("COM1")); 
	m_ComPortNum.AddString(_T("COM2")) ;//COM2
	m_ComPortNum.AddString(_T("COM3")) ;//COM3
	m_ComPortNum.AddString(_T("COM4")); 
	m_ComPortNum.AddString(_T("COM5")) ;//COM2
	m_ComPortNum.AddString(_T("COM6")) ;//COM3
	
	m_ComBaurate.AddString(_T("115200")) ;//COM1
	m_ComBaurate.AddString(_T("9600")) ;//COM2
	m_ComBaurate.AddString(_T("2400")) ;//COM3

	

	m_ComPortNum.SetCurSel(2);//COM3
	m_ComBaurate.SetCurSel(1);//baud rate = 9600
	nPortOpened = FALSE;
	m_bStartSave = FALSE;
//		nRecIndex = 0;
	UpdateData(TRUE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDisplayDataDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDisplayDataDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDisplayDataDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDisplayDataDlg::OnTimer(UINT_PTR nIDEvent)
{
//ͼ�񱣴�
	float newy;
	float newx;

//���ݴ洢
	CString str=_T(""); 


	// TODO: Add your message handler code here and/or call default
		newy = (float)nDisplayData;
	 // New y value //
//	   newy = (float)(50.0F + (sin((double) m_nSinCounter * 0.075F) * 30.0F));// + GetRandom(1, 15);

  
   //newy = 50.0F + (sin((double) m_nSinCounter * 0.075F) * 30.0F) + GetRandom(1, 15);
   newx = m_nRealTimeCounter;

     // Append new values  //
   PEvset(m_hPE, PEP_faAPPENDYDATA, &newy, 1);
   PEvset(m_hPE, PEP_faAPPENDXDATA, &newx, 1);

   // Increment counter //
   m_nRealTimeCounter = m_nRealTimeCounter + 1;

   // Switch to AutoScaling x axis after receiving 100 data points //
   if (m_nRealTimeCounter == 300)
       PEnset(m_hPE, PEP_nMANUALSCALECONTROLX, PEMSC_NONE);

   // SinCounter is only to produce sin wave data //
   m_nSinCounter = m_nSinCounter + 1;
   if (m_nSinCounter > 30000) 
       m_nSinCounter = 1;

   // Update image and force paint //
   PEreinitialize( m_hPE );
   PEresetimage( m_hPE, 0, 0 );
   ::InvalidateRect(m_hPE, NULL, FALSE);

   if(m_bStartSave)
   {
	  //��ʽת��
	   str.Format("%d",nDisplayData);

	   //���浽�ļ�
	   if(m_Edit_save_path.Right(4) != ".TXT")
		   m_Edit_save_path += ".TXT";
	   FILE *m_pSavefp = fopen(m_Edit_save_path,"a+t");
//		   CTime time=CTime::GetCurrentTime(); 
//		   CString tstr =time.Format("%m��%d�� %H:%M:%S   ");
//		   tstr += str;
	   fprintf(m_pSavefp,"%s\n",str);
	   fclose(m_pSavefp);
   }



	CDialogEx::OnTimer(nIDEvent);
}


void CDisplayDataDlg::OnBnClickedButtonFilePath()
{
   // szFilters is a text string that includes two file name filters: 
   // "*.my" for "MyType Files" and "*.*' for "All Files."
   TCHAR szFilters[]= _T("Txt Files (*.txt)|*.TXT|All Files (*.*)|*.*||");

   // Create an Open dialog; the default file name extension is ".my".
   CFileDialog fileDlg(TRUE, _T("*"), _T("*.TXT"),
      OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

   // Display the file dialog. When user clicks OK, fileDlg.DoModal()  
   // returns IDOK. 
   	if(fileDlg.DoModal() == IDOK)						//�ж��Ƿ���"��"��ť
	{
		m_Edit_save_path = fileDlg.GetPathName();				//����ļ�·��
		UpdateData(FALSE);
	}
	
	// TODO: Add your control notification handler code here
}


void CDisplayDataDlg::OnBnClickedButtonComOpen()
{
	// TODO: Add your control notification handler code here
	int nPort= m_ComPortNum.GetCurSel()+1; //�õ����ںţ�����ΪʲôҪ��1
//		int nBaudrate = atoi(m_ComBaurate.SelectString(m_ComBaurate.GetCurSel()));
//		TRACE("------%d----\n",nBaudrate);

	if(nPortOpened) //�رմ���
	{
		m_SerialComm.ClosePort();
		nPortOpened = FALSE;
		KillTimer(1);
		SetDlgItemText(IDC_BUTTON_COM_OPEN,_T("��"));
	}
	else //�򿪴���
	{
		if(m_SerialComm.InitPort(this, nPort, 9600,'N',8,1,EV_RXFLAG | EV_RXCHAR,512))
		{
			m_SerialComm.StartMonitoring();
			nPortOpened = TRUE;
			   // Initialize Counters and Timer 
			SetTimer( 1, 1, NULL );

			SetDlgItemText(IDC_BUTTON_COM_OPEN,_T("�ر�"));
		}
		else
		{
			AfxMessageBox(_T("û�з��ִ˴��ڻ�ռ��"));
			nPortOpened = FALSE;
			SetDlgItemText(IDC_BUTTON_COM_OPEN,_T("��"));
		}
		
	}



}
LONG CDisplayDataDlg::OnComm(WPARAM ch, LPARAM port)
{
//		nRecBuf[nRecIndex++] = ch;
	nDisplayData = ch;
//	TRACE("------%d----\n",ch);
//		nRecIndex++;
	
//		if(nRecIndex >=10)
//			nRecIndex = 0;
	return 0;
}

void CDisplayDataDlg::OnBnClickedButtonDatSave()
{
	// TODO: Add your control notification handler code here
	m_bStartSave = !m_bStartSave;
	if(m_bStartSave)
		SetDlgItemText(IDC_BUTTON_DAT_SAVE,_T("ֹͣ�洢"));
	else
		SetDlgItemText(IDC_BUTTON_DAT_SAVE,_T("��ʼ�洢"));
}
