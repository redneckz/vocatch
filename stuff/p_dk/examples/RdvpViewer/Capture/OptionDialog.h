#if !defined(AFX_OPTIONDIALOG_H__1C3BBE7C_C53C_47EC_8AEC_81202549E1D4__INCLUDED_)
#define AFX_OPTIONDIALOG_H__1C3BBE7C_C53C_47EC_8AEC_81202549E1D4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionDialog.h : header file
//
#include "resource.h"
#include "OptionDialog.h"


/////////////////////////////////////////////////////////////////////////////
// COptionDialog dialog

class COptionDialog : public CDialog
{
// Construction
public:
	COptionDialog(CWnd* pParent = NULL);   // standard constructor

	COptionDialog(CCaptureApp *pParent);   // my contructor

// Dialog Data
	//{{AFX_DATA(COptionDialog)
	enum { IDD = IDD_DIALOG_OPT };
	CComboBox	m_cmbFormat;
	CComboBox	m_cmbInput;
	CComboBox	m_cmbDevice;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionDialog)
	afx_msg void OnSelchange_ComboDevice();
	afx_msg void OnSelchange_ComboInput();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchange_ComboFormat();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Struct for video & color formats
	struct SFormat
	{
		DWORD	dwRvclFlag;		// RVCL value
		long	lStringID;		// resource string ID
	};

	const static SFormat s_aVideoFormats[];

	int	m_nDevice;
	int	m_nInput;
	DWORD	m_dwFormat;

	CCaptureApp *m_pParent;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONDIALOG_H__1C3BBE7C_C53C_47EC_8AEC_81202549E1D4__INCLUDED_)
