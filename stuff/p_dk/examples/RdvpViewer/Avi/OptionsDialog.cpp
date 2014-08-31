// OptionsDialog.cpp : implementation file
//

#include "stdafx.h"

#include "OptionsDialog.h"
#include "Avi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsDialog dialog


COptionsDialog::COptionsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(COptionsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsDialog)
	//}}AFX_DATA_INIT
}


COptionsDialog::COptionsDialog(CAviApp* pParent /*=NULL*/)
	: CDialog(COptionsDialog::IDD, NULL)
{

	m_pParent = pParent;
	m_strName.Empty();
	m_dwCacheFrames = m_pParent->m_dwCacheFrames;
}


void COptionsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsDialog)
	DDX_Control(pDX, IDC_STATIC_AVINAME, m_stName);
	DDX_Check(pDX, IDC_CHECK_CACHE, m_chkUseCache);
	DDX_Text(pDX, IDC_EDIT_CACHE, m_dwCacheFrames);
	DDV_MinMaxDWord(pDX, m_dwCacheFrames, 0, 500);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsDialog, CDialog)
	//{{AFX_MSG_MAP(COptionsDialog)
	ON_BN_CLICKED(IDC_BUTTON_OPENAVI, OnButtonOpenavi)
	ON_BN_CLICKED(IDC_CHECK_CACHE, OnCheckCache)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsDialog message handlers

void COptionsDialog::OnButtonOpenavi() 
{

	char szFilter[] = "Avi Files (*.avi)|*.avi|Moving Picture (*.mov)|*.mov|All files (*.*)|*.*||";
 
	// TODO: Add your control notification handler code here
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CFileDialog	*dlgOpen = new CFileDialog(TRUE,NULL,NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter);

	if(dlgOpen->DoModal() == IDOK)
	{
		CString strMsg,strCap;

		m_strName = dlgOpen->GetPathName();

		//print avi Name on the dialogs
		m_stName.SetWindowText(m_strName);

	}
	
	delete dlgOpen;
}

//////////////////////////////////////////////////////////////
//
void COptionsDialog::OnOK() 
{

	m_pParent->Stop();


	if(m_strName.IsEmpty()==TRUE || m_pParent->OpenAvi(m_strName))
	{
		CString	strCap,strMsg;

		strCap.LoadString(IDS_ERROR);
		strMsg.LoadString(IDS_ERROR_AVI);
		MessageBox(strMsg,strCap);
		return;
	}

	// TODO: Add extra validation here

	if(m_strName.IsEmpty()==FALSE)
	{
		if(m_chkUseCache==TRUE)
		{
			m_pParent->MakePrecache(m_dwCacheFrames);
		}
		else
			m_pParent->MakePrecache(20);
	}

	m_pParent->m_chkUseCache = 	m_chkUseCache; 
	m_pParent->m_dwCacheFrames = m_dwCacheFrames;

	CDialog::OnOK();
}

void COptionsDialog::OnCheckCache() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);	

	CWnd *wndEditBox = GetDlgItem(IDC_EDIT_CACHE);

	if(m_chkUseCache) 
		wndEditBox->EnableWindow(TRUE);
	else
		wndEditBox->EnableWindow(FALSE);
}

BOOL COptionsDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_chkUseCache = m_pParent->m_chkUseCache;
	m_dwCacheFrames = m_pParent->m_dwCacheFrames;

	m_strName = m_pParent->m_strName;

	//print avi Name on the dialogs
	m_stName.SetWindowText(m_strName);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsDialog::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnClose();
}
