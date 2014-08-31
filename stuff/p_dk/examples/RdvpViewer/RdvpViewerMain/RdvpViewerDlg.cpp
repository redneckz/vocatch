// RdvpViewerDlg.cpp : implementation file
//
/********************************************************************
	created:	2003/01/21
	created:	21:1:2003   18:08
	filename: 	D:\Projects\RDVPack\Examples\RdvpViewer\RdvpViewerMain\RdvpViewerDlg.cpp
	file path:	D:\Projects\RDVPack\Examples\RdvpViewer\RdvpViewerMain
	file base:	RdvpViewerDlg
	file ext:	cpp
	author:		Svitcov
	
	purpose:	
*********************************************************************/

#include "stdafx.h"
#include "MainApp.h"
#include "RdvpViewerDlg.h"
#include "SourceDialog.h"
#include "data2compress.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Video formats
const CRdvpViewerDlg::SVFormat CRdvpViewerDlg::s_aVideoFormats[] = 
{	
	{{192,144}},		 
	{{256,192}},
	{{384,288}},		 
	{{512,384}},
	{{768,288}},
	{{768,576}},
};

// Color formats
const CRdvpViewerDlg::SCFormat CRdvpViewerDlg::s_aColorFormats[] = 
{	
	{RDN_CF_RGB32,		    IDS_CF_RGB32},
	{RDN_CF_RGB24,		    IDS_CF_RGB24},
	{RDN_CF_YUV411_PACKED, IDS_CF_YUV},
	{RDN_CF_YUV411_PLANAR, IDS_CF_YUVPLAN},
	{RDN_CF_Y8,		    IDS_CF_Y8}
};


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRdvpViewerDlg dialog

CRdvpViewerDlg::CRdvpViewerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRdvpViewerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRdvpViewerDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);


	lpfInitialize = NULL;
	lpfSetColorFormat = NULL;
	lpfSetResolution = NULL;
	lpfStop = NULL;
	lpfStart = NULL;
	lpfExitCapture = NULL;
	lpfGetResolution = NULL;

	m_codecDVPack = NULL;
}

void CRdvpViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRdvpViewerDlg)
	DDX_Control(pDX, IDC_STATIC_FRAMESIZE, m_stFrameSize);
	DDX_Control(pDX, IDC_STATIC_DECTIME, m_stDecTime);
	DDX_Control(pDX, IDC_STATIC_COMPTIME, m_stCompTime);
	DDX_Control(pDX, IDC_STATIC_COMPRESSION, m_stCompression);
	DDX_Control(pDX, IDC_STATIC_SENSE, m_stSense);
	DDX_Control(pDX, IDC_STATIC_QUAL, m_stQual);
	DDX_Control(pDX, IDC_BUTTON_OPTIONS, m_btnOptions);
	DDX_Control(pDX, IDC_BUTTON_START, m_btnStart);
	DDX_Control(pDX, IDC_BUTTON_STOP, m_btnStop);
	DDX_Control(pDX, IDC_SLIDER_SENSE, m_slSense);
	DDX_Control(pDX, IDC_SLIDER_QUAL, m_slQual);
	DDX_Control(pDX, IDC_COMBO_DSTCF, m_cmbDstCF);
	DDX_Control(pDX, IDC_COMBO_SRCCF, m_cmbSrcCF);
	DDX_Control(pDX, IDC_COMBO_DSTREZ, m_cmbDstRez);
	DDX_Control(pDX, IDC_COMBO_SRCREZ, m_cmbSrcRez);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRdvpViewerDlg, CDialog)
	//{{AFX_MSG_MAP(CRdvpViewerDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_COMBO_SRCREZ, OnSelChange_ComboSrcRez)
	ON_CBN_SELCHANGE(IDC_COMBO_DSTREZ, OnSelChange_ComboDstRez)
	ON_CBN_SELCHANGE(IDC_COMBO_SRCCF, OnSelChange_ComboSrcCF)
	ON_CBN_SELCHANGE(IDC_COMBO_DSTCF, OnSelChange_ComboDstCF)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BUTTON_OPTIONS, OnButtonOptions)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_START, OnButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnButtonStop)
	ON_MESSAGE(WM_USER_PRINTDATA,OnPrintData)
	ON_MESSAGE(WM_USER_CODEC_ERROR,OnCodecError)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRdvpViewerDlg message handlers

BOOL CRdvpViewerDlg::OnInitDialog()
{

	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
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
	CString		strRes;

	//fill the possible source resolution
	//
	//  

	int i,j;
	for( i = 0; i<sizeof(s_aVideoFormats)/sizeof(SVFormat);i++)
	{
		strRes.Format(IDS_REZ,s_aVideoFormats[i].szRes.cx,s_aVideoFormats[i].szRes.cy);
		
		j = m_cmbSrcRez.AddString(strRes);
		m_cmbSrcRez.SetItemData(j,i);
		
		j = m_cmbDstRez.AddString(strRes);
		m_cmbDstRez.SetItemData(j,i);
	}
	
	for( i = 0; i<sizeof(s_aColorFormats)/sizeof(SCFormat);i++)
	{
		strRes.LoadString(s_aColorFormats[i].lStringID);

		//do not include RGB32 to input coor formats
		if(i)
		{
			j = m_cmbSrcCF.AddString(strRes);
			m_cmbSrcCF.SetItemData(j,i);
		}

		// Destination Format must be only YUV411 Packed or Y8 GrayScale 
		//(because video adapter does not support this format for output video)
		j = m_cmbDstCF.AddString(strRes);
		m_cmbDstCF.SetItemData(j,i);				
	}

	//Set Source and Destination Color Formats and Resolutions
		m_cmbSrcCF.SetCurSel(0);		//set RG24 as default
		m_cmbDstCF.SetCurSel(0);
		m_cmbSrcRez.SetCurSel(0);		//set 192x144 as default
		m_cmbDstRez.SetCurSel(0);

		CHAR str[20];

		m_bQuality = 3;
		m_bSense = 6;

		//set Slider Parameters
		m_slQual.SetRangeMin(1);
		m_slQual.SetRangeMax(10);
		m_slQual.SetPos(m_bQuality);
		sprintf(str,"Quality %2d0", m_slQual.GetPos());
		m_stQual.SetWindowText(str);

		//set Slider Parameters
		m_slSense.SetRangeMin(1);
		m_slSense.SetRangeMax(10);
		m_slSense.SetPos(m_bSense);
		sprintf(str,"Sensitivity %2d", m_slSense.GetPos());
		m_stSense.SetWindowText(str);


		m_dwIntraFrame = 100;			//every 100 frame set Intra Frame


		///////////////////////////////////////////////////////
		// now we must select video source (bt878 or avi file)
		// 
		CSourceDialog	*dlgSource = new CSourceDialog(this);

		if(dlgSource->DoModal() != IDOK)
		{

			delete dlgSource;

			::PostMessage(m_hWnd, WM_CLOSE, 0, 0);

			return TRUE;
		}


		delete dlgSource;
		//Send to Dll Compress Routine Adress

		m_codecDVPack = new CCodec(GetSafeHwnd());

		//read resolution
		m_sSourceParams.szResolution = s_aVideoFormats[m_cmbSrcRez.GetItemData(m_cmbSrcRez.GetCurSel())].szRes;
		m_sDestParams.szResolution = s_aVideoFormats[m_cmbDstRez.GetItemData(0)].szRes;

		//read Color Format
		m_sSourceParams.dwColorFormat = s_aColorFormats[m_cmbSrcCF.GetItemData(m_cmbSrcCF.GetCurSel())].dwRvclFlag;
		m_sDestParams.dwColorFormat = s_aColorFormats[m_cmbDstCF.GetItemData(0)].dwRvclFlag;

		m_codecDVPack->SetParams(m_bQuality,m_bSense,m_dwIntraFrame,
							m_sSourceParams.szResolution,
							m_sSourceParams.dwColorFormat,
							m_sDestParams.szResolution,
							m_sDestParams.dwColorFormat);


		lpfSetColorFormat(m_sSourceParams.dwColorFormat);
		lpfSetResolution(m_sSourceParams.szResolution);

		if(lpfInitialize(CCodec::ProcessImage,m_codecDVPack)) 
		{
			::PostMessage(m_hWnd, WM_CLOSE, 0, 0);
			return TRUE;
		}


		SetAviResolution();
	
		m_lastCmbDstRez = 0;

	return TRUE;  // return TRUE  unless you set the focus to a control
}


void	CRdvpViewerDlg::SetAviResolution()
{
	SIZE	szRes;
	if(lpfGetResolution)
	{

		m_cmbSrcRez.ResetContent();

		lpfGetResolution(&szRes);
		m_sSourceParams.szResolution = szRes;

		CString strAvi;

		strAvi.Format(IDS_REZ,szRes.cx,szRes.cy);

		m_sDestParams.szResolution = s_aVideoFormats[0].szRes;			//set 192x144

		int j = m_cmbSrcRez.AddString(strAvi);
		m_cmbSrcRez.SetItemData(j,0);
		m_cmbSrcRez.SetCurSel(0);

		DWORD dwItemData = 0xffffffff;

		for(int i = 0; i < m_cmbDstRez.GetCount(); i++)
			if(m_cmbDstRez.GetItemData(i) == dwItemData)
				break;

		m_cmbDstRez.DeleteString(i);
		m_cmbDstRez.SetCurSel(0);

		for(i = 0, j = 0; i < sizeof(s_aVideoFormats)/sizeof(SVFormat); i++)
			if(!memcmp(&s_aVideoFormats[i].szRes, &szRes, sizeof(SIZE) ) )
				break;

		if( !(i < sizeof(s_aVideoFormats)/sizeof(SVFormat)))
		{
			j = m_cmbDstRez.AddString(strAvi);
			m_cmbDstRez.SetItemData(j,dwItemData);
			m_cmbDstRez.SetCurSel(j);
			m_lastCmbDstRez = j;
			OnSelChange_ComboDstRez();
		}
	}

}


void CRdvpViewerDlg::DisableControls()
{

	m_cmbSrcRez.EnableWindow(FALSE);
	m_cmbSrcCF.EnableWindow(FALSE);

	m_dwFlagSource = AVI;
}

void CRdvpViewerDlg::EnableControls()
{
	m_cmbSrcRez.EnableWindow(TRUE);
	m_cmbSrcCF.EnableWindow(TRUE);

	m_dwFlagSource = LIVE;

}

void CRdvpViewerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRdvpViewerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRdvpViewerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CRdvpViewerDlg::OnSelChange_ComboSrcRez() 
{
	// TODO: Add your control notification handler code here
	int i = m_cmbSrcRez.GetCurSel();

	if(i == CB_ERR)
		return;

	// get index from array
	DWORD iRes = m_cmbSrcRez.GetItemData(i);

	//read resolution
	m_sSourceParams.szResolution = s_aVideoFormats[iRes].szRes;

	if(ControlResolution()==TRUE)
	{
		CString str, strCap;
		str.LoadString(IDS_ERROR_DESTINATIONSIZE);
		strCap.LoadString(IDS_WARNING);
		MessageBox(str, strCap, MB_OK | MB_ICONWARNING);

		int iSrc = m_cmbSrcRez.GetCurSel();

		m_lastCmbDstRez = iSrc;
		m_cmbDstRez.SetCurSel(iSrc);

		m_sDestParams.szResolution = s_aVideoFormats[iRes].szRes;
	}

	m_codecDVPack->SetParams(m_bQuality,m_bSense,m_dwIntraFrame,
							m_sSourceParams.szResolution,
							m_sSourceParams.dwColorFormat,
							m_sDestParams.szResolution,
							m_sDestParams.dwColorFormat);

	lpfSetColorFormat(m_sSourceParams.dwColorFormat);
	lpfSetResolution(m_sSourceParams.szResolution);
	
}


BOOL	CRdvpViewerDlg::ControlResolution()
{

	if( m_sSourceParams.szResolution.cx < m_sDestParams.szResolution.cx ||
		m_sSourceParams.szResolution.cy < m_sDestParams.szResolution.cy)
		{
//			m_sDestParams.szResolution = m_sSourceParams.szResolution;
		return TRUE;
	}
	
	
	return FALSE;
}


void CRdvpViewerDlg::OnSelChange_ComboDstRez() 
{
	// TODO: Add your control notification handler code here
	int i = m_cmbDstRez.GetCurSel();
	
	if(i == CB_ERR)
		return;
	
	// get index from array
	DWORD iRes = m_cmbDstRez.GetItemData(i);
	
	//read resolution
	if(iRes == 0xffffffff)
	{
		if(lpfGetResolution)
			lpfGetResolution(&m_sDestParams.szResolution);
	}
	else
		m_sDestParams.szResolution = s_aVideoFormats[iRes].szRes;

	if(ControlResolution()==TRUE)
	{
		CString str, strCap;
		str.LoadString(IDS_ERROR_DESTINATIONSIZE);
		strCap.LoadString(IDS_WARNING);
		MessageBox(str, strCap, MB_OK | MB_ICONWARNING);

		m_cmbDstRez.SetCurSel(m_lastCmbDstRez);
		OnSelChange_ComboDstRez();
	}

	m_lastCmbDstRez = m_cmbDstRez.GetCurSel();

	//Set Parameters  of DV Pack(r)
	m_codecDVPack->SetParams(m_bQuality,m_bSense,m_dwIntraFrame,
							m_sSourceParams.szResolution,
							m_sSourceParams.dwColorFormat,
							m_sDestParams.szResolution,
							m_sDestParams.dwColorFormat);

}

void CRdvpViewerDlg::OnSelChange_ComboSrcCF() 
{
	// TODO: Add your control notification handler code here
	int i = m_cmbSrcCF.GetCurSel();

	if(i == CB_ERR)
		return;

	// get index from array
	DWORD iColor = m_cmbSrcCF.GetItemData(i);

	m_sSourceParams.dwColorFormat = s_aColorFormats[iColor].dwRvclFlag;

	m_codecDVPack->SetParams(m_bQuality,m_bSense,m_dwIntraFrame,
							m_sSourceParams.szResolution,
							m_sSourceParams.dwColorFormat,
							m_sDestParams.szResolution,
							m_sDestParams.dwColorFormat);

	lpfSetColorFormat(m_sSourceParams.dwColorFormat);
	lpfSetResolution(m_sSourceParams.szResolution);
	
}

void CRdvpViewerDlg::OnSelChange_ComboDstCF() 
{
	// TODO: Add your control notification handler code here
	int i = m_cmbDstCF.GetCurSel();

	if(i == CB_ERR)
		return;

	// get index from array
	DWORD iColor = m_cmbDstCF.GetItemData(i);

	//read resolution
	m_sDestParams.dwColorFormat = s_aColorFormats[iColor].dwRvclFlag;

	//Set Parameters  of DV Pack(r)
	m_codecDVPack->SetParams(m_bQuality,m_bSense,m_dwIntraFrame,
							m_sSourceParams.szResolution,
							m_sSourceParams.dwColorFormat,
							m_sDestParams.szResolution,
							m_sDestParams.dwColorFormat);
	
}

void CRdvpViewerDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default

	CHAR	str[20];

	//if it is Quality Scroll Basr - Get Position of Slider Quality
	if((CSliderCtrl *)pScrollBar == (CSliderCtrl *)GetDlgItem(IDC_SLIDER_QUAL))
	{
		sprintf(str,"Quality %2d0", m_slQual.GetPos());
		m_stQual.SetWindowText(str);
		m_bQuality = m_slQual.GetPos();
	}

	//if it is sense scroll bar - get sense
	if((CSliderCtrl *)pScrollBar == (CSliderCtrl *)GetDlgItem(IDC_SLIDER_SENSE))
	{
		sprintf(str,"Sensitivity %2d", m_slSense.GetPos());
		m_stSense.SetWindowText(str);
		m_bSense = m_slSense.GetPos();
	}

	//Set Parameters  of DV Pack(r)
	m_codecDVPack->SetParams(m_bQuality,m_bSense,m_dwIntraFrame,
							m_sSourceParams.szResolution,
							m_sSourceParams.dwColorFormat,
							m_sDestParams.szResolution,
							m_sDestParams.dwColorFormat);

	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CRdvpViewerDlg::OnButtonOptions() 
{
	// TODO: Add your control notification handler code here

	//stop compress/decompress routine
	OnButtonStop();

	m_codecDVPack->Reset();
	//Send to Dll Compress Routine Adress
	lpfInitialize(CCodec::ProcessImage,m_codecDVPack);

	SetAviResolution();

	m_lastCmbDstRez = 0;

}

void CRdvpViewerDlg::OnDestroy() 
{

	OnButtonStop();

	if(m_codecDVPack) m_codecDVPack->Destroy();

	if(lpfExitCapture) lpfExitCapture();

	if(m_hCaptureDll) FreeLibrary(m_hCaptureDll);

	if(m_codecDVPack) delete m_codecDVPack;

	// TODO: Add your message handler code here
	CDialog::OnDestroy();	
}

void CRdvpViewerDlg::OnButtonStart() 
{

	if(lpfSetColorFormat) lpfSetColorFormat(m_sSourceParams.dwColorFormat);
	if(lpfSetResolution) lpfSetResolution(m_sSourceParams.szResolution);

	lpfStart();

	m_codecDVPack->SetParams(m_bQuality,m_bSense,m_dwIntraFrame,
							m_sSourceParams.szResolution,
							m_sSourceParams.dwColorFormat,
							m_sDestParams.szResolution,
							m_sDestParams.dwColorFormat);

	 m_codecDVPack->Start();

	 m_btnStart.EnableWindow(FALSE);
	 m_btnStop.EnableWindow(TRUE);
	 m_btnOptions.EnableWindow(FALSE);

	// TODO: Add your control notification handler code here
	
}


void CRdvpViewerDlg::OnButtonStop() 
{
	if(lpfStop) lpfStop();

	// TODO: Add your control notification handler code here
	 if(m_codecDVPack) m_codecDVPack->Stop();

	 m_btnStart.EnableWindow(TRUE);
	 m_btnStop.EnableWindow(FALSE);
	 m_btnOptions.EnableWindow(TRUE);
}

LRESULT CRdvpViewerDlg::OnPrintData(WPARAM wParam, LPARAM lParam)
{
	CHAR	str[512];

	m_codecDVPack->GetCharacteristics(&m_sChar);

	sprintf(str,"%i.%i ms   ",m_sChar.dwCompressTime/10,m_sChar.dwCompressTime % 10);
	m_stCompTime.SetWindowText(str);
	
	sprintf(str,"%i.%i ms   ",m_sChar.dwDecompressTime/10,m_sChar.dwDecompressTime % 10);
	m_stDecTime.SetWindowText(str);

	sprintf(str,"%.1f Kb   ",(float)m_sChar.dwAvgFrameSize/1000);
	m_stFrameSize.SetWindowText(str);

	sprintf(str,"%.0f : 1    ",m_sChar.fCompression);
	m_stCompression.SetWindowText(str);

	return 0L ;
}

LRESULT CRdvpViewerDlg::OnCodecError(WPARAM wParam, LPARAM lParam)
{
	OnButtonStop();

	CString str, strCap;
	str.LoadString(IDS_ERROR_CODECERROR);
	strCap.LoadString(IDS_ERROR);
	MessageBox(str, strCap, MB_OK | MB_ICONERROR);

	return 0L;
}

