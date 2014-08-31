#if !defined(AFX_GENERALPAGE_H__3805A2A6_BE9D_449B_89BE_44F8EABE2500__INCLUDED_)
#define AFX_GENERALPAGE_H__3805A2A6_BE9D_449B_89BE_44F8EABE2500__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GeneralPage.h : header file
//

#include "ControlPageBase.h"

/////////////////////////////////////////////////////////////////////////////
// CGeneralPage dialog

class CGeneralPage : public CControlPageBase
{
// Construction
public:
	CGeneralPage(CControlPanelDlg* pControlPanel);
	virtual ~CGeneralPage();

// Dialog Data
	//{{AFX_DATA(CGeneralPage)
	enum { IDD = IDD_GENERAL_PAGE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	DWORD GetColorFormat();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGeneralPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Struct for video & color formats
	struct SFormat
	{
		DWORD	dwRvclFlag;		// RVCL value
		long	lStringID;		// resource string ID
	};

	const static SFormat s_aVideoFormats[];
public:
	typedef struct defVideoAdjustments
	{
		DWORD dwBrightness;
		DWORD dwContrast;
		DWORD dwSaturation;
		DWORD dwHue;
		DWORD dwCombRange;
	}SVideoAdjustments;

	const static SVideoAdjustments s_aVideoAdjustmentsDefault[];
	const static SVideoAdjustments s_aVideoAdjustmentsMax[];

	const static SFormat s_aColorFormats[];

	// Generated message map functions
	//{{AFX_MSG(CGeneralPage)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChange_ComboColorFormat();
	afx_msg void OnSelChange_ComboVideoFormat();
	afx_msg void OnSelChange_ComboVideoSource();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnButton_ResetAdjustments();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void ShowCurrentState();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GENERALPAGE_H__3805A2A6_BE9D_449B_89BE_44F8EABE2500__INCLUDED_)
