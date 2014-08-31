// SourceDialog.cpp : implementation file
//

#include "stdafx.h"
#include "MainApp.h"
#include "SourceDialog.h"
#include "data2compress.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CSourceDialog dialog


CSourceDialog::CSourceDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSourceDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSourceDialog)
	m_nSelect = -1;
	//}}AFX_DATA_INIT
}

CSourceDialog::CSourceDialog(CRdvpViewerDlg* pParent)   // constructor with paretn class
	: CDialog(CSourceDialog::IDD, NULL)
{
	m_pParent = pParent;

}

void CSourceDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSourceDialog)
	DDX_Radio(pDX, IDC_RADIO_LIVE, m_nSelect);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSourceDialog, CDialog)
	//{{AFX_MSG_MAP(CSourceDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSourceDialog message handlers

void CSourceDialog::OnOK() 
{
	// TODO: Add extra validation here

	UpdateData(TRUE);
	
	if(m_nSelect==0)					//User select the Live Video - Load Capture DLL
	{

 	   // Load(check) capture.dll
       m_pParent->m_hCaptureDll = LoadLibrary(_T("Capture.dll"));
                                  
       if(m_pParent->m_hCaptureDll == NULL)		//Error to Load "capture.dll"
	   {
		   CString	strMsg,strCap;

		   strCap.LoadString(IDS_ERROR);
		   strMsg.LoadString(IDS_ERROR_DLLLOAD_CAPTURE);

		   MessageBox(strMsg,strCap,MB_OK);
		   return;
	   }

      m_pParent->EnableControls();

	  m_pParent->lpfGetResolution = NULL;
	}
	else if(m_nSelect==1)				//User Select ideo form Avi File - Load Avi DLL
	{

 	   // Load(check) capture.dll
       m_pParent->m_hCaptureDll = LoadLibrary(_T("Avi.dll"));
                                  
       if(m_pParent->m_hCaptureDll == NULL)		//Error to Load "capture.dll"
	   {
		   CString	strMsg,strCap;

		   strCap.LoadString(IDS_ERROR);
		   strMsg.LoadString(IDS_ERROR_DLLLOAD_AVI);

		   MessageBox(strMsg,strCap,MB_OK);
		   return;
	   }

	   m_pParent->DisableControls();

	   //get resolution only for avi files
	   m_pParent->lpfGetResolution = (BOOL(__stdcall*)(SIZE*))GetProcAddress(m_pParent->m_hCaptureDll, "D2CGetResolution");

	}
	else
	{
		ASSERT(false);
		return;
	}


	
	//get Function and set internal pointers
   
	//Get Function from DLL
	m_pParent->lpfInitialize = (BOOL(__stdcall*)(	PDATA2COMPRESS_ROUTINE,LPVOID))GetProcAddress(m_pParent->m_hCaptureDll,
																				 "D2CInitialize");

	m_pParent->lpfSetColorFormat = (BOOL(__stdcall*)(DWORD))GetProcAddress(m_pParent->m_hCaptureDll, "D2CSetColorFormat");

	m_pParent->lpfSetResolution = (BOOL(__stdcall*)(SIZE))GetProcAddress(m_pParent->m_hCaptureDll, "D2CSetResolution");

	m_pParent->lpfExitCapture = (BOOL(__stdcall*)())GetProcAddress(m_pParent->m_hCaptureDll, "D2CExitCapture");

	m_pParent->lpfStart = (BOOL(__stdcall*)())GetProcAddress(m_pParent->m_hCaptureDll, "D2CStart");
	m_pParent->lpfStop = (BOOL(__stdcall*)())GetProcAddress(m_pParent->m_hCaptureDll, "D2CStop");

				
     //check to Load all functions                      
	if(	m_pParent->lpfInitialize == NULL || 
		m_pParent->lpfSetColorFormat == NULL || 
		m_pParent->lpfSetResolution == NULL  ||
		m_pParent->lpfStart == NULL ||
		m_pParent->lpfStop == NULL ||
		m_pParent->lpfExitCapture == NULL)
	{
	   CString	strMsg,strCap;

	   strCap.LoadString(IDS_ERROR);
	   strMsg.LoadString(IDS_ERROR_DLLLOADEXPORT);

	   MessageBox(strMsg,strCap,MB_OK);
	   return;
	}
                                  		                                  		
	
	CDialog::OnOK();
}


BOOL CSourceDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_nSelect=0;
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
