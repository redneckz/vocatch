// RvcViewer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "RvcViewer.h"

#include "MainFrm.h"
#include "VideoDoc.h"
#include "VideoView.h"
#include "LoginDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRvcViewerApp

BEGIN_MESSAGE_MAP(CRvcViewerApp, CWinApp)
	//{{AFX_MSG_MAP(CRvcViewerApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRvcViewerApp construction

CRvcViewerApp::CRvcViewerApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CRvcViewerApp object

CRvcViewerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CRvcViewerApp initialization

BOOL CRvcViewerApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CString		strMsg,strCap;
	CLoginDlg	dlgLogin;

	// Show login dialog
	if(dlgLogin.DoModal() != IDOK)
		return FALSE;

	SRvclInitialize rvclInit;

	rvclInit.dwSize = sizeof(rvclInit);
	rvclInit.pdwCustomerPassword[0] = dlgLogin.m_dwCustomerPassword[0];
	rvclInit.pdwCustomerPassword[1] = dlgLogin.m_dwCustomerPassword[1];
	rvclInit.pdwCustomerPassword[2] = dlgLogin.m_dwCustomerPassword[2];
	rvclInit.pdwCustomerPassword[3] = dlgLogin.m_dwCustomerPassword[3];
	
	// Initialize RVCL library
	if(!RvclInitializeEx(&rvclInit))
	{
		DWORD dwError = GetLastError();

		strCap.LoadString(IDS_ERROR);
		strMsg.Format(IDS_ERR_RVCL_INIT_FAILED,ERROR_MESSAGE(dwError),dwError);
		MessageBox(NULL,strMsg,strCap,MB_OK|MB_ICONSTOP);
		return FALSE;
	}

	// Get devices count
	int nDevices;

	if(!RvclGetDevicesCount(&nDevices) || nDevices == 0)
	{
		strCap.LoadString(IDS_ERROR);
		strMsg.LoadString(IDS_ERR_NO_DEVICES);
		MessageBox(NULL,strMsg,strCap,MB_OK|MB_ICONSTOP);
		return FALSE;
	}

	// Register the application's document templates.  Document templates
	// serve as the connection between documents, frame windows and views.
	CSingleDocTemplate* pDocTemplate;

	// Создаём шаблон с видео-окном или для настройки мультимедиа.
	pDocTemplate = new CSingleDocTemplate(
							IDR_MAINFRAME,
							RUNTIME_CLASS(CVideoDoc),
							RUNTIME_CLASS(CMainFrame),
							RUNTIME_CLASS(CVideoView));

	AddDocTemplate(pDocTemplate);

	CCommandLineInfo cmdInfo;
	
	cmdInfo.m_nShellCommand = CCommandLineInfo::FileNew;
	cmdInfo.m_bShowSplash = TRUE;
	cmdInfo.m_bRunAutomated = FALSE;
	cmdInfo.m_bRunEmbedded = FALSE;

	// Dispatch commands specified on the command line
	if(!ProcessShellCommand(cmdInfo))
		return FALSE;
	
	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	return TRUE;
}


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
		// No message handlers
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

// App command to run the dialog
void CRvcViewerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CRvcViewerApp message handlers

