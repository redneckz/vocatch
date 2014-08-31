#if !defined(AFX_OPTIONSDIALOG_H__1997D7E5_5899_4577_A0B7_EC852916185B__INCLUDED_)
#define AFX_OPTIONSDIALOG_H__1997D7E5_5899_4577_A0B7_EC852916185B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsDialog.h : header file
//


#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// COptionsDialog dialog

class CAviApp;

class COptionsDialog : public CDialog
{
// Construction
public:
	COptionsDialog(CWnd* pParent = NULL);   // standard constructor
	COptionsDialog(CAviApp* pParent);   // nonstandard constructor

// Dialog Data
	//{{AFX_DATA(COptionsDialog)
	enum { IDD = IDD_DIALOG_AVI };
	CStatic	m_stName;
	BOOL	m_chkUseCache;
	DWORD	m_dwCacheFrames;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionsDialog)
	afx_msg void OnButtonOpenavi();
	virtual void OnOK();
	afx_msg void OnCheckCache();
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


private:
	CAviApp* m_pParent;
	CString m_strName;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSDIALOG_H__1997D7E5_5899_4577_A0B7_EC852916185B__INCLUDED_)
