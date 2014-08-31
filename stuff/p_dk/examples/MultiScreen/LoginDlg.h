#if !defined(AFX_LOGINDLG_H__A982189A_BD37_4C12_B244_966DC154DAC8__INCLUDED_)
#define AFX_LOGINDLG_H__A982189A_BD37_4C12_B244_966DC154DAC8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LoginDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLoginDlg dialog

class CLoginDlg : public CDialog
{
// Construction
public:
	CLoginDlg(CWnd* pParent = NULL);   // standard constructor

protected:
// Dialog Data
	//{{AFX_DATA(CLoginDlg)
	enum { IDD = IDD_LOGIN };
	CString	m_strPassword1;
	CString	m_strPassword2;
	CString	m_strPassword3;
	CString	m_strPassword4;
	int		m_nRadioPasswords;
	//}}AFX_DATA

public:
	DWORD m_dwCustomerPassword[4];

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLoginDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLoginDlg)
	virtual void OnOK();
	afx_msg void OnRadioPasswords();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGINDLG_H__A982189A_BD37_4C12_B244_966DC154DAC8__INCLUDED_)
