// MultiScreen.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "MultiScreen.h"
#include "MainFrm.h"
#include "LoginDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMultiScreenApp

BEGIN_MESSAGE_MAP(CMultiScreenApp, CWinApp)
	//{{AFX_MSG_MAP(CMultiScreenApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMultiScreenApp construction

CMultiScreenApp::CMultiScreenApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMultiScreenApp object

CMultiScreenApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMultiScreenApp initialization

BOOL CMultiScreenApp::InitInstance()
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

	CString		strMsg;
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

		strMsg.Format(IDS_ERR_RVCL_INIT_FAILED,ERROR_MESSAGE(dwError),dwError);
		MessageBox(NULL,strMsg,NULL,MB_OK|MB_ICONSTOP);
		return FALSE;
	}

	// Get devices count
	int nDevices;
	
	if(!RvclGetDevicesCount(&nDevices) || nDevices == 0)
	{
		strMsg.LoadString(IDS_ERR_NO_DEVICES);
		MessageBox(NULL,strMsg,NULL,MB_OK|MB_ICONSTOP);
		return FALSE;
	}

	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object.

	CMainFrame* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;

	// Create and load the frame with its resources

	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);

	// The one and only window has been initialized, so show and update it.
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMultiScreenApp message handlers





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
void CMultiScreenApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CMultiScreenApp message handlers

