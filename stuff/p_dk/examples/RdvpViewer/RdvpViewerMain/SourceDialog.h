#if !defined(AFX_SOURCEDIALOG_H__5280867B_AA80_49D8_AB87_49CB21E79C36__INCLUDED_)
#define AFX_SOURCEDIALOG_H__5280867B_AA80_49D8_AB87_49CB21E79C36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SourceDialog.h : header file
//
#include "resource.h"

#include "RdvpViewerDlg.h"
/////////////////////////////////////////////////////////////////////////////
// CSourceDialog dialog

class CSourceDialog : public CDialog
{
// Construction
public:
	CSourceDialog(CWnd* pParent = NULL);   // standard constructor

	CSourceDialog(CRdvpViewerDlg* pParent);   // constructor with paretn class

// Dialog Data
	//{{AFX_DATA(CSourceDialog)
	enum { IDD = IDD_DIALOG_SOURCE };
	int		m_nSelect;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSourceDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

public:

protected:

	// Generated message map functions
	//{{AFX_MSG(CSourceDialog)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CRdvpViewerDlg	*m_pParent;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOURCEDIALOG_H__5280867B_AA80_49D8_AB87_49CB21E79C36__INCLUDED_)
