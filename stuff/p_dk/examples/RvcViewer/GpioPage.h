#if !defined(AFX_GPIOPAGE_H__359456E6_0F2C_44D6_9D94_27ED4F6DD942__INCLUDED_)
#define AFX_GPIOPAGE_H__359456E6_0F2C_44D6_9D94_27ED4F6DD942__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GpioPage.h : header file
//

#include "ControlPageBase.h"

/////////////////////////////////////////////////////////////////////////////
// CGpioPage dialog

class CGpioPage : public CControlPageBase
{
// Construction
public:
	CGpioPage(CControlPanelDlg* pControlPanel);
	virtual ~CGpioPage();

// Dialog Data
	//{{AFX_DATA(CGpioPage)
	enum { IDD = IDD_GPIO_PAGE };
	CListCtrl	m_listGpio;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGpioPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGpioPage)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnItemChanged_ListGpio(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButton_Set();
	afx_msg void OnButton_Reset();
	afx_msg void OnButton_Input();
	afx_msg void OnButton_Output();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void ShowCurrentState();
	void SetOutputEnable(int iItem, bool bOutput);
	void SetData(int iItem, bool bSet);
	void UpdateControls();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GPIOPAGE_H__359456E6_0F2C_44D6_9D94_27ED4F6DD942__INCLUDED_)
