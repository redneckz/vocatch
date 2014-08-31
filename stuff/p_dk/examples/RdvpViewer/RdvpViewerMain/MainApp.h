// RdvpViewer.h : main header file for the RdvpViewer application
//

#if !defined(AFX_RdvpViewer_H__B62D70B7_7F1F_4167_BF9F_F679E80D0DDA__INCLUDED_)
#define AFX_RdvpViewer_H__B62D70B7_7F1F_4167_BF9F_F679E80D0DDA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


/////////////////////////////////////////////////////////////////////////////
// User Message for print auxulary data
#define	WM_USER_PRINTDATA	(WM_USER+500)
#define WM_USER_CODEC_ERROR (WM_USER+501)

/////////////////////////////////////////////////////////////////////////////
// CMainApp:
// See RdvpViewer.cpp for the implementation of this class
//

class CMainApp : public CWinApp
{
public:
	CMainApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMainApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RdvpViewer_H__B62D70B7_7F1F_4167_BF9F_F679E80D0DDA__INCLUDED_)
