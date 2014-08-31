// KeyEditDlg.h : header file
//

#if !defined(AFX_KEYEDITDLG_H__B50BA37C_AD92_4C35_A322_0A19EE1F6156__INCLUDED_)
#define AFX_KEYEDITDLG_H__B50BA37C_AD92_4C35_A322_0A19EE1F6156__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CKeyEditDlg dialog

class CKeyEditDlg : public CDialog
{
// Construction
public:
	CKeyEditDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CKeyEditDlg)
	enum { IDD = IDD_KEYEDIT_DIALOG };
	CListCtrl	m_listKeyData;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKeyEditDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CKeyEditDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnButtonRead();
	afx_msg void OnButtonWrite();
	afx_msg void OnButtonEdit();
	afx_msg void OnBeginLabelEdit_ListData(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndLabelEdit_ListData(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomDraw_ListData(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	bool GetPassword(DWORD dwPassword[4]);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KEYEDITDLG_H__B50BA37C_AD92_4C35_A322_0A19EE1F6156__INCLUDED_)
