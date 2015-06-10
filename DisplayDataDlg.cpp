
// DisplayDataDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DisplayData.h"
#include "DisplayDataDlg.h"
#include "afxdialogex.h"
#include "Pegrpapi.h "

#include "math.h"
#include "SerialPort.h"
#include "stdlib.h"
#include "stdio.h"
#include "time.h"

#define GetRandom( min, max ) ((rand() % (int)(((max) + 1) - (min))) + (min))



#define X_AXIS_LEN 600

#define Y_AXIS_LEN 600


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
public:
//	virtual HRESULT accDoDefaultAction(VARIANT varChild);
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
	m_Edit_ReDisp_FilePath = _T("");
}

void CDisplayDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FILE_PATH, m_Edit_save_path);
	//  DDX_CBString(pDX, IDC_COMBO_PORTNUM, m_PortNum);
	DDX_Control(pDX, IDC_COMBO_BAUD, m_ComBaurate);
	DDX_Control(pDX, IDC_COMBO_PORTNUM, m_ComPortNum);
	DDX_Text(pDX, IDC_EDIT_REDISP_FILE_PATH, m_Edit_ReDisp_FilePath);
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
	ON_BN_CLICKED(IDC_BUTTON_DAT_REDISPLAY, &CDisplayDataDlg::OnBnClickedButtonDatRedisplay)
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
	nDispDataIndex= 0;
	for(int i=0;i<DISP_DATA_NUMS;i++)
		nDisplayData[i] = 0;


		//获取图像显示范围
	//RECT rect;
	CStatic *pST=(CStatic *)GetDlgItem(IDC_STATIC_DISP);
	pST->GetWindowRect(&m_rect);
	ScreenToClient(&m_rect);
	//UpdateData(TRUE);
   //GetClientRect( &rect );

   RealtimeDisplay_Init();
//	   StaticDisplay_Init();
  
  //D:\Material\Current Project\mathwork

//设置默认路径
   	m_Edit_save_path = _T("d:\\DataSample.txt"); 
	SetDlgItemText(IDC_EDIT_FILE_PATH,m_Edit_save_path);//采集数据默认存储路径	
	
//设置串口
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
//	//	//	//图像保存
//		float newy;
//		float newy2;
//		float newx;
//		
//数据存储
	CString str=_T(""); 
	CString str2=_T(""); 

//		// TODO: Add your message handler code here and/or call default
//		newy = (float)nDisplayData[0];
//		newy = (float)nDisplayData[0];
//		newy2 = (float)nDisplayData[1];
	 // New y value //
//	   newx = m_nRealTimeCounter;

     // Append new values  //
//	   PEvset(m_hPE, PEP_faAPPENDYDATA, &newy, 1);
//	   PEvset(m_hPE, PEP_faAPPENDXDATA, &newx, 1);
//	   PEvset(m_hPE, PEP_faAPPENDYDATA, &newy2, 2);
//	   PEvset(m_hPE, PEP_faAPPENDXDATA, &newx, 2);
//	
//	//		// Store new values at current index //
//	//		PEvsetcellEx(m_hPE, PEP_faXDATA, 0, m_nRealTimeCounter, &newx);
//	//		PEvsetcellEx(m_hPE, PEP_faYDATA, 0, m_nRealTimeCounter, &newy);
//	//		 
//	//		// Store new values at current index //
//	//		PEvsetcellEx(m_hPE, PEP_faXDATA, 1, m_nRealTimeCounter, &newx);
//	//		PEvsetcellEx(m_hPE, PEP_faYDATA, 1, m_nRealTimeCounter, &newy);
//	
//	   // Increment counter //
//	   m_nRealTimeCounter = m_nRealTimeCounter + 1;
//	
//	   // Switch to AutoScaling x axis after receiving 100 data points //
//	   if (m_nRealTimeCounter == X_AXIS_LEN)
//	       PEnset(m_hPE, PEP_nMANUALSCALECONTROLX, PEMSC_NONE);
//	
//	   // SinCounter is only to produce sin wave data //
//	   m_nSinCounter = m_nSinCounter + 1;
//	   if (m_nSinCounter > 30000) 
//	       m_nSinCounter = 0;

//	   // Update image and force paint //
//	   PEreinitialize( m_hPE );
//	   PEresetimage( m_hPE, 0, 0 );
//	   ::InvalidateRect(m_hPE, NULL, FALSE);

	float r1,r2,fNew[2];
	CTime t;
	CString ttext;

	t = CTime::GetCurrentTime();
	ttext = t.Format("%M:%S");

	// Graph Real Time Feed //
	PEvset(m_hPE, PEP_szaAPPENDPOINTLABELDATA, (void *) (LPCTSTR) ttext, 1);

	fNew[0] =  nDisplayData[0];
	fNew[1] =  nDisplayData[1];

	PEvset(m_hPE, PEP_faAPPENDYDATA, &fNew[0], 1);
   

   if(m_bStartSave)
   {
	  //格式转换
	  	nDisplayData[0]++;
		if((nDisplayData[0] >= 500)||(nDisplayData[0] < (-500)))
		nDisplayData[0] = 0;

		nDisplayData[1] = nDisplayData[0]+50;;
	
	   str.Format("%d",nDisplayData[0]);
	   str2.Format("%d",(nDisplayData[1]));

	   //保存到文件
	   if(m_Edit_save_path.Right(4) != ".TXT")
		   m_Edit_save_path += ".TXT";

		FILE *m_pSavefp = fopen(m_Edit_save_path,"a+t");
		//		   CTime time=CTime::GetCurrentTime(); 
		//		   CString tstr =time.Format("%m??%d?? %H:%M:%S   ");
		//		   tstr += str;
		fprintf(m_pSavefp,"%s\t",str);//第一列数据
		fprintf(m_pSavefp,"%s\n",str2);//第二列数据
		fclose(m_pSavefp);
	   	   
//			CStdioFile m_pSavefp;
//			if(m_pSavefp.Open(m_Edit_save_path, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
//			{
//	//				m_pSavefp.Write(&nDisplayData, 1);
//	//				m_pSavefp.Close();
//					char pbufWrite[100];
//					memset(pbufWrite, 'a', sizeof(pbufWrite));
//					m_pSavefp.Write(pbufWrite, 100);         
//					m_pSavefp.Flush();
//	
//			}

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
   	if(fileDlg.DoModal() == IDOK)						//判断是否按下"打开"按钮
	{
		m_Edit_save_path = fileDlg.GetPathName();				//获得文件路径
		UpdateData(FALSE);
	}
	
	// TODO: Add your control notification handler code here
}


void CDisplayDataDlg::OnBnClickedButtonComOpen()
{
	// TODO: Add your control notification handler code here
	int nPort= m_ComPortNum.GetCurSel()+1; //得到串口号，想想为什么要加1
//		int nBaudrate = atoi(m_ComBaurate.SelectString(m_ComBaurate.GetCurSel()));
//		TRACE("------%d----\n",nBaudrate);

	if(nPortOpened) //关闭串口
	{
		m_SerialComm.ClosePort();
		nPortOpened = FALSE;
		
		SetDlgItemText(IDC_BUTTON_COM_OPEN,_T("打开"));
	}
	else //打开串口
	{
		if(m_SerialComm.InitPort(this, nPort, 9600,'N',8,1,EV_RXFLAG | EV_RXCHAR,512))
		{
			m_SerialComm.StartMonitoring();
			nPortOpened = TRUE;
			   // Initialize Counters and Timer 
			SetDlgItemText(IDC_BUTTON_COM_OPEN,_T("关闭"));
		}
		else
		{
			AfxMessageBox(_T("没有发现此串口或被占用"));
			nPortOpened = FALSE;
			SetDlgItemText(IDC_BUTTON_COM_OPEN,_T("打开"));
		}
	}



}
LONG CDisplayDataDlg::OnComm(WPARAM ch, LPARAM port)
{
//		nRecBuf[nRecIndex++] = ch;
	nDisplayData[nDispDataIndex++] = ch;

	if(nDispDataIndex >= DISP_DATA_NUMS)
		nDispDataIndex = 0;
	
	return 0;
}

void CDisplayDataDlg::OnBnClickedButtonDatSave()
{
	// TODO: Add your control notification handler code here
	m_bStartSave = !m_bStartSave;
	if(m_bStartSave)
	{
	
		SetDlgItemText(IDC_BUTTON_DAT_SAVE,_T("停止存储"));
		RealtimeDisplay_Init();
	
		  
		SetTimer( 1, 5, NULL );
	}
	else
	{
		KillTimer(1);
		SetDlgItemText(IDC_BUTTON_DAT_SAVE,_T("开始存储"));
	}
}


//HRESULT CAboutDlg::accDoDefaultAction(VARIANT varChild)
//{
//	// TODO: Add your specialized code here and/or call the base class
//
//	return CDialogEx::accDoDefaultAction(varChild);
//}


void CDisplayDataDlg::RealtimeDisplay_Init(void)
{
//			m_hPE = PEcreate(PECONTROL_SGRAPH, WS_VISIBLE, &m_rect, m_hWnd, 1001);
//			PEnset(m_hPE, PEP_nSUBSETS, 2);
//			PEnset(m_hPE, PEP_nPOINTS, X_AXIS_LEN);
//	
//			// Set Manual Y scale //
//			PEnset(m_hPE, PEP_nMANUALSCALECONTROLY, PEMSC_MINMAX);
//			double arg = 1.0F;
//			PEvset(m_hPE, PEP_fMANUALMINY, &arg, 1);
//			arg = Y_AXIS_LEN;
//			PEvset(m_hPE, PEP_fMANUALMAXY, &arg, 1);
//	
//			// Set Manual X scale //
//			PEnset(m_hPE, PEP_nMANUALSCALECONTROLX, PEMSC_MINMAX);
//			arg = 0.0F;
//			PEvset(m_hPE, PEP_fMANUALMINX, &arg, 1);
//			arg = X_AXIS_LEN;
//			PEvset(m_hPE, PEP_fMANUALMAXX, &arg, 1);
//	
//			// Clear out default data //
//			float val = 0;
//			PEvsetcellEx(m_hPE, PEP_faXDATA, 0, 0, &val);
//			PEvsetcellEx(m_hPE, PEP_faXDATA, 0, 1, &val);
//			PEvsetcellEx(m_hPE, PEP_faXDATA, 0, 2, &val);
//			PEvsetcellEx(m_hPE, PEP_faXDATA, 0, 3, &val);
//	
//			PEvsetcellEx(m_hPE, PEP_faYDATA, 0, 0, &val);
//			PEvsetcellEx(m_hPE, PEP_faYDATA, 0, 1, &val);
//			PEvsetcellEx(m_hPE, PEP_faYDATA, 0, 2, &val);
//			PEvsetcellEx(m_hPE, PEP_faYDATA, 0, 3, &val);
//	
//			// Set Various Other Properties ///
//			PEnset(m_hPE, PEP_bBITMAPGRADIENTMODE, FALSE);
//			PEnset(m_hPE, PEP_nQUICKSTYLE, PEQS_MEDIUM_INSET);
//	
//			PEszset(m_hPE, PEP_szMAINTITLE, TEXT("数据实时监控"));
//	
//			PEszset(m_hPE, PEP_szSUBTITLE, TEXT(""));
//			PEnset(m_hPE, PEP_bNORANDOMPOINTSTOEXPORT, TRUE);
//			PEnset(m_hPE, PEP_bFOCALRECT, FALSE);
//			PEnset(m_hPE, PEP_bALLOWBAR, FALSE);
//			PEnset(m_hPE, PEP_bALLOWPOPUP, FALSE);
//			PEnset(m_hPE, PEP_bPREPAREIMAGES, TRUE);
//			PEnset(m_hPE, PEP_bCACHEBMP, TRUE);
//			PEnset(m_hPE, PEP_bFIXEDFONTS, TRUE);
//	
//			DWORD col = PERGB( 255,0, 198, 0);
//			PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, 0, &col);
//	
//			PEnset(m_hPE, PEP_nGRADIENTBARS, 8);
//			PEnset(m_hPE, PEP_nTEXTSHADOWS, PETS_BOLD_TEXT);
//			PEnset(m_hPE, PEP_bMAINTITLEBOLD, TRUE);
//			PEnset(m_hPE, PEP_bSUBTITLEBOLD, TRUE);
//			PEnset(m_hPE, PEP_bLABELBOLD, TRUE);
//			PEnset(m_hPE, PEP_bLINESHADOWS, TRUE);
//			PEnset(m_hPE, PEP_nFONTSIZE, PEFS_MEDIUM);
//	
//	//			// Improves metafile export //
//	//			PEnset(m_hPE, PEP_nDPIX, 600);
//	//			PEnset(m_hPE, PEP_nDPIY, 600);
//	//	
//	//			PEreinitialize(m_hPE);
//	//			PEresetimage(m_hPE, 0, 0);
//	
//			int dwSC[] = { PERGB(128,0,198,0), PERGB( 128,255,255,255) };
//			PEvset(m_hPE, PEP_dwaSUBSETCOLORS, dwSC, 2);
//			
//			PEnset(m_hPE, PEP_nRENDERENGINE, PERE_HYBRID);
//			


		
		m_hPE = PEcreate(PECONTROL_GRAPH, WS_VISIBLE, &m_rect, m_hWnd, 1001);
		
		// No Shadows //
		PEnset(m_hPE, PEP_nDATASHADOWS, PEDS_NONE);
		
		// No Flicker //
		PEnset(m_hPE, PEP_bPREPAREIMAGES, TRUE);
		PEnset(m_hPE, PEP_bCACHEBMP, TRUE);
		
		// Set Subsets and Points //
		PEnset(m_hPE, PEP_nSUBSETS, 2); 	 // set number of subsets
		// Set number data points
		PEnset(m_hPE, PEP_nPOINTS, 600); // number of data points	 
		
		PEnset(m_hPE, PEP_nPOINTSTOGRAPH, 100);
		PEnset(m_hPE, PEP_nPOINTSTOGRAPHINIT, PEPTGI_LASTPOINTS);			 
//			PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_AREA);    
		PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_POINTSPLUSLINE);

		
		// main title
		PEszset(m_hPE, PEP_szMAINTITLE, TEXT("Graph Real Time"));
		PEszset(m_hPE, PEP_szSUBTITLE, TEXT("")); // no subtitle
		
		// Manually configure scales //
		PEnset(m_hPE, PEP_nMANUALSCALECONTROLY, PEMSC_MINMAX);
		
		double manminY = 0.0F;
		PEvset(m_hPE, PEP_fMANUALMINY, &manminY, 1);
		double manmaxY = 600.0F;
		PEvset(m_hPE, PEP_fMANUALMAXY, &manmaxY, 1);
		
		PEszset(m_hPE, PEP_szMANUALMAXPOINTLABEL, TEXT("000.000" ));
		PEszset(m_hPE, PEP_szMANUALMAXDATASTRING, TEXT("00:00:00xx"));
		
		PEnset(m_hPE, PEP_bNOSTACKEDDATA, TRUE); 
		PEnset(m_hPE, PEP_bNORANDOMPOINTSTOGRAPH, TRUE);
		PEnset(m_hPE, PEP_bALLOWHISTOGRAM, FALSE);	 
		PEnset(m_hPE, PEP_bFOCALRECT, FALSE);
		PEnset(m_hPE, PEP_nGRIDLINECONTROL, FALSE);
		PEnset(m_hPE, PEP_nDATAPRECISION, 1);									   
		
		// Needed to allocate point labels so append logic works //
		// Set last point label, Points - 1 //
		PEszset(m_hPE, PEP_szaPOINTLABELS, TEXT(""));
		
		float f1 = 0.0F;
		PEvsetcellEx(m_hPE, PEP_faYDATA, 0, 0, &f1);
		PEvsetcellEx(m_hPE, PEP_faYDATA, 0, 1, &f1);
		PEvsetcellEx(m_hPE, PEP_faYDATA, 0, 2, &f1);
		PEvsetcellEx(m_hPE, PEP_faYDATA, 0, 3, &f1);
		
		PEnset(m_hPE, PEP_bBITMAPGRADIENTMODE, FALSE);
		PEnset(m_hPE, PEP_nQUICKSTYLE, PEQS_DARK_NO_BORDER);
		PEnset(m_hPE, PEP_bFIXEDFONTS, TRUE);
		
		PEnset(m_hPE, PEP_nTEXTSHADOWS, PETS_BOLD_TEXT);
		PEnset(m_hPE, PEP_bMAINTITLEBOLD, TRUE);
		PEnset(m_hPE, PEP_bSUBTITLEBOLD, TRUE);
		PEnset(m_hPE, PEP_bLABELBOLD, TRUE);
		PEnset(m_hPE, PEP_nFONTSIZE, PEFS_MEDIUM);
		
		int dwSC[] = { PERGB(128,0,198,0), PERGB( 128,255,255,255) };
		PEvset(m_hPE, PEP_dwaSUBSETCOLORS, dwSC, 2);
		
		PEnset(m_hPE, PEP_nRENDERENGINE, PERE_HYBRID);
		


		// Demo's Logic controlling RenderEngine // 
		//CMDIFrameWnd* pWnd = (CMDIFrameWnd*) AfxGetApp()->GetMainWnd();
		//pWnd->SendMessage(WM_CHANGE_METAFILE, PEPLAYMETAFILE );
		m_nRealTimeCounter = 1;
		m_nSinCounter = 1;
}


void CDisplayDataDlg::StaticDisplay_Init(void)
{
	m_hPE = PEcreate(PECONTROL_SGRAPH, WS_VISIBLE, &m_rect, m_hWnd, 1001);
	if( m_hPE )
   {
       float fY;
       float fX;
		
       // Enable middle mouse dragging //
       PEnset(m_hPE, PEP_bMOUSEDRAGGINGX, TRUE);
       PEnset(m_hPE, PEP_bMOUSEDRAGGINGY, TRUE);

       // Enable Bar Glass Effect //
       PEnset(m_hPE, PEP_bBARGLASSEFFECT, TRUE);

       // Enable Plotting style gradient and bevel features //
       PEnset(m_hPE, PEP_nAREAGRADIENTSTYLE,  PEPGS_RADIAL_BOTTOM_RIGHT);
       PEnset(m_hPE, PEP_nAREABEVELSTYLE, PEBS_MEDIUM_SMOOTH);
       PEnset(m_hPE, PEP_nSPLINEGRADIENTSTYLE,  PEPGS_RADIAL_BOTTOM_RIGHT);
       PEnset(m_hPE, PEP_nSPLINEBEVELSTYLE, PESBS_MEDIUM_SMOOTH);

       // Set number of Subsets and Points //
       PEnset(m_hPE, PEP_nSUBSETS, 4);	
       PEnset(m_hPE, PEP_nPOINTS, 12);	

       for( int s=0; s<=3; s++ )
       {
           for( int p=0; p<=11; p++ )
           {										  
               fX = ((float) (p+1)) * 100.0F;// + ((float) GetRandom(2, 250)) / 50.0F;
               PEvsetcellEx (m_hPE, PEP_faXDATA, s, p, &fX);
               fY = ((float) (p+1) * 1.0F) + 700.0F - ((float) (s * 140.0F));// + ((float) GetRandom(2, 250)) / 250.0F;
               PEvsetcellEx (m_hPE, PEP_faYDATA, s, p, &fY);
           }
       }

       // Set DataShadows to show shadows
       PEnset(m_hPE, PEP_nDATASHADOWS, PEDS_SHADOWS);

       PEszset(m_hPE, PEP_szMAINTITLE, TEXT("Example Data"));
       PEszset(m_hPE, PEP_szSUBTITLE, TEXT("")); // no subtitle
       PEszset(m_hPE, PEP_szYAXISLABEL, TEXT("Units Sold"));
       PEszset(m_hPE, PEP_szXAXISLABEL, TEXT("Month"));
       PEnset(m_hPE, PEP_bFOCALRECT, FALSE);
       PEnset(m_hPE, PEP_bPREPAREIMAGES, TRUE);
       PEnset(m_hPE, PEP_bCACHEBMP, TRUE);
       PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_SPLINE);
       PEnset(m_hPE, PEP_nGRIDLINECONTROL, PEGLC_BOTH);
       PEnset(m_hPE, PEP_nGRIDSTYLE, PEGS_DOT);
       PEnset(m_hPE, PEP_nALLOWZOOMING, PEAZ_HORZANDVERT);
       PEnset(m_hPE, PEP_nZOOMSTYLE, PEZS_RO2_NOT);

       // subset labels
       PEvsetcell( m_hPE, PEP_szaSUBSETLABELS, 0, TEXT("Texas"));
       PEvsetcell( m_hPE, PEP_szaSUBSETLABELS, 1, TEXT("Florida" ));
       PEvsetcell( m_hPE, PEP_szaSUBSETLABELS, 2, TEXT("Washington" ));
       PEvsetcell( m_hPE, PEP_szaSUBSETLABELS, 3, TEXT("California" ));

       // subset colors
       DWORD dwArray[4] = { PERGB(128,198,0,0), PERGB(128, 0, 198, 198 ), PERGB(128, 198,198,0 ), PERGB(128, 0,198,0 ) };
       PEvsetEx( m_hPE, PEP_dwaSUBSETCOLORS, 0, 4, dwArray, 0 );

       // subset line types
       int nLineTypes[] = { PELT_MEDIUMSOLID, PELT_MEDIUMSOLID, PELT_MEDIUMSOLID, PELT_MEDIUMSOLID };
       PEvset(m_hPE, PEP_naSUBSETLINETYPES, nLineTypes, 4);

       // subset point types
       int nPointTypes[] = { PEPT_DOTSOLID, PEPT_UPTRIANGLESOLID, PEPT_SQUARESOLID, PEPT_DOWNTRIANGLESOLID };
       PEvset(m_hPE, PEP_naSUBSETPOINTTYPES, nPointTypes, 4);

       // Version 4.0 Features //
       PEnset(m_hPE, PEP_bFIXEDFONTS, TRUE);
       PEnset(m_hPE, PEP_bSIMPLEPOINTLEGEND, TRUE);
       PEnset(m_hPE, PEP_bSIMPLELINELEGEND, TRUE);
       PEnset(m_hPE, PEP_nLEGENDSTYLE, PELS_1_LINE);
       PEnset(m_hPE, PEP_nMULTIAXISSTYLE, PEMAS_SEPARATE_AXES);

       // Set Various Other Properties //
       PEnset(m_hPE, PEP_bBITMAPGRADIENTMODE, TRUE);
       PEnset(m_hPE, PEP_nQUICKSTYLE, PEQS_MEDIUM_NO_BORDER);

       PEnset(m_hPE, PEP_nGRADIENTBARS, 8);
       PEnset(m_hPE, PEP_nTEXTSHADOWS, PETS_BOLD_TEXT);
       PEnset(m_hPE, PEP_bMAINTITLEBOLD, TRUE);
       PEnset(m_hPE, PEP_bSUBTITLEBOLD, TRUE);
       PEnset(m_hPE, PEP_bLABELBOLD, TRUE);
       PEnset(m_hPE, PEP_bLINESHADOWS, TRUE);
       PEnset(m_hPE, PEP_nFONTSIZE, PEFS_LARGE);
       PEnset(m_hPE, PEP_bSCROLLINGHORZZOOM, TRUE);
       PEnset(m_hPE, PEP_nDATAPRECISION, 1);

       // Improves metafile export //
       PEnset(m_hPE, PEP_nDPIX, 600);
       PEnset(m_hPE, PEP_nDPIY, 600);

       // Set Demo's RenderEngine to Gdi Plus // 
       //CMDIFrameWnd* pWnd = (CMDIFrameWnd*) AfxGetApp()->GetMainWnd();
      // pWnd->SendMessage(WM_CHANGE_METAFILE, PEPLAYMETAFILEGDIPLUS );
   }

		
}

void CDisplayDataDlg::OnBnClickedButtonDatRedisplay()
{
	// TODO: Add your control notification handler code here
	
	if(!m_bStartSave)//不在实时显示状态
	{
		//图像框图初始化
		StaticDisplay_Init();
		//读取文件路径
		TCHAR szFilters[]= _T("Txt Files (*.txt)|*.TXT|All Files (*.*)|*.*||");
		
		// Create an Open dialog; the default file name extension is ".my".
		CFileDialog fileDlg(TRUE, _T("*"), _T("*.TXT"),
		   OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);
		
		// Display the file dialog. When user clicks OK, fileDlg.DoModal()	
		// returns IDOK. 
		 if(fileDlg.DoModal() == IDOK)						 //判断是否按下"打开"按钮
		 {
			 m_Edit_ReDisp_FilePath = fileDlg.GetPathName();				 //获得文件路径
			 UpdateData(FALSE);
		 }


		 
		//文件读取
		CStdioFile fileWave;
		int m_iWavePointCnt;//
		int m_iData = 0;
		if(fileWave.Open(m_Edit_ReDisp_FilePath,CFile::typeText|CFile::modeReadWrite))
		{
			m_iWavePointCnt = fileWave.GetLength();
			m_iWavePointCnt = 10;
			fileWave.SeekToBegin();
			for(int i=0;i<m_iWavePointCnt;i++)
			{
				fileWave.Read(&m_iData, 1);
//					TRACE("------%d----\n",m_iWavePointCnt);
				TRACE("------%d----\n",m_iData);
				
			}
				
			
		}

		
//	
//				CString strText ="";
//	
//				CString szLine = "";
//	
//				//打开文件
//				CStdioFile file;
//	
//				file.Open("ts.txt",CFile::modeRead);
//	
//				//逐行读取字符串
//	
//				while( file.ReadString( szLine ) )
//	
//				{
//					strText += szLine;
//				}
//	
//				MessageBox(strText);
//	
//				//关闭文件
//	
//				file.Close();

	}

	else
		AfxMessageBox(_T("请关闭实时显示"));
		
}
