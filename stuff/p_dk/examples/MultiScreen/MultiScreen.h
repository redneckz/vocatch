// MultiScreen.h : main header file for the MULTISCREEN application
//

#if !defined(AFX_MULTISCREEN_H__052ADC9D_F4BE_49F9_989C_9BA7C7BD4BAF__INCLUDED_)
#define AFX_MULTISCREEN_H__052ADC9D_F4BE_49F9_989C_9BA7C7BD4BAF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"               // main symbols
#include "..\..\include\rvcl.h"		// PENTACON Video Capture Library

#define ERROR_MESSAGE(dwError) _com_error(HRESULT_FROM_WIN32(dwError)).ErrorMessage()

/////////////////////////////////////////////////////////////////////////////
// CMultiScreenApp:
// See MultiScreen.cpp for the implementation of this class
//

class CMultiScreenApp : public CWinApp
{
public:
	CMultiScreenApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMultiScreenApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

public:
	//{{AFX_MSG(CMultiScreenApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MULTISCREEN_H__052ADC9D_F4BE_49F9_989C_9BA7C7BD4BAF__INCLUDED_)
