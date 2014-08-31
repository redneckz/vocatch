#if !defined(AFX_CONTROLPAGEBASE_H__1BAF8C0F_27B1_4F0E_8F00_BB1EFFF4A3A5__INCLUDED_)
#define AFX_CONTROLPAGEBASE_H__1BAF8C0F_27B1_4F0E_8F00_BB1EFFF4A3A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ControlPageBase.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CControlPageBase window

class CControlPanelDlg;

class CControlPageBase : public CDialog
{
// Construction
public:
	CControlPageBase(CControlPanelDlg* pControlPanel, UINT nStatusUpdateInterval);

// Attributes
public:

// Operations
public:
	virtual void ShowCurrentState() = 0;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CControlPageBase)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CControlPageBase();

	// Generated message map functions
protected:
	//{{AFX_MSG(CControlPageBase)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	int		GetCurrentDevice() const;
	CString	GetCurrentDeviceName() const;
	void	UpdateView() const;
	
protected:
	CControlPanelDlg* m_pControlPanel;

	UINT m_nStatusTimer;			// status update timer
	UINT m_nStatusUpdateInterval;	// status update interval (ms)
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTROLPAGEBASE_H__1BAF8C0F_27B1_4F0E_8F00_BB1EFFF4A3A5__INCLUDED_)
