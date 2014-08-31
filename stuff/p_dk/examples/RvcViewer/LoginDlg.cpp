// LoginDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RvcViewer.h"
#include "LoginDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLoginDlg dialog


CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLoginDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLoginDlg)
	m_strPassword1 = _T("");
	m_strPassword2 = _T("");
	m_strPassword3 = _T("");
	m_strPassword4 = _T("");
	m_nRadioPasswords = 0;
	//}}AFX_DATA_INIT
}


void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoginDlg)
	DDX_Text(pDX, IDC_EDIT_PASSWORD1, m_strPassword1);
	DDV_MaxChars(pDX, m_strPassword1, 8);
	DDX_Text(pDX, IDC_EDIT_PASSWORD2, m_strPassword2);
	DDV_MaxChars(pDX, m_strPassword2, 8);
	DDX_Text(pDX, IDC_EDIT_PASSWORD3, m_strPassword3);
	DDV_MaxChars(pDX, m_strPassword3, 8);
	DDX_Text(pDX, IDC_EDIT_PASSWORD4, m_strPassword4);
	DDV_MaxChars(pDX, m_strPassword4, 8);
	DDX_Radio(pDX, IDC_RADIO_PASSWORDS1, m_nRadioPasswords);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialog)
	//{{AFX_MSG_MAP(CLoginDlg)
	ON_BN_CLICKED(IDC_RADIO_PASSWORDS1, OnRadioPasswords)
	ON_BN_CLICKED(IDC_RADIO_PASSWORDS2, OnRadioPasswords)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLoginDlg message handlers

void CLoginDlg::OnOK() 
{
	UpdateData(true);

	switch(m_nRadioPasswords)
	{
		case 0:
		{
			// Use Demo Key customer passwords
			m_dwCustomerPassword[0] = 0x9592CB4F;
			m_dwCustomerPassword[1] = 0xF3B62AC0;
			m_dwCustomerPassword[2] = 0xFCB8802F;
			m_dwCustomerPassword[3] = 0x9CE9CEF9;
			break;
		}
		case 1:
		{
			// Use specified passwords
			LPTSTR endptr;

			m_dwCustomerPassword[0] = _tcstoul(m_strPassword1, &endptr, 16);
			m_dwCustomerPassword[1] = _tcstoul(m_strPassword2, &endptr, 16);
			m_dwCustomerPassword[2] = _tcstoul(m_strPassword3, &endptr, 16);
			m_dwCustomerPassword[3] = _tcstoul(m_strPassword4, &endptr, 16);

			for(int i=0; i<3; i++)
				if(	m_dwCustomerPassword[i] == ULONG_MAX ||
					m_dwCustomerPassword[i] == 0 )
				{
					CString strMsg;

					strMsg.Format(_T("Invalid password #%u"),i+1);
					MessageBox(strMsg,_T("Error"),MB_OK|MB_ICONSTOP);
					return;
				}
			
			break;
		}

		default:
			ASSERT(false);
	}
	
	CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////

void CLoginDlg::OnRadioPasswords() 
{
	UpdateData(true);

	bool fEnable = (m_nRadioPasswords == 1);
	
	GetDlgItem(IDC_EDIT_PASSWORD1)->EnableWindow(fEnable);
	GetDlgItem(IDC_EDIT_PASSWORD2)->EnableWindow(fEnable);
	GetDlgItem(IDC_EDIT_PASSWORD3)->EnableWindow(fEnable);
	GetDlgItem(IDC_EDIT_PASSWORD4)->EnableWindow(fEnable);
}

/////////////////////////////////////////////////////////////////////////////

BOOL CLoginDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	OnRadioPasswords();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
