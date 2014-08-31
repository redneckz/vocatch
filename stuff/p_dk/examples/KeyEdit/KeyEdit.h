// KeyEdit.h : main header file for the KEYEDIT application
//

#if !defined(AFX_KEYEDIT_H__3396C683_EE3F_4E19_8DA0_2D3BBC69F95E__INCLUDED_)
#define AFX_KEYEDIT_H__3396C683_EE3F_4E19_8DA0_2D3BBC69F95E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"
#include "..\..\include\rdevkit.h"

#define ERROR_MESSAGE(dwError) _com_error(HRESULT_FROM_WIN32(dwError)).ErrorMessage()

/////////////////////////////////////////////////////////////////////////////
// CKeyEditApp:
// See KeyEdit.cpp for the implementation of this class
//

class CKeyEditApp : public CWinApp
{
public:
	CKeyEditApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKeyEditApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CKeyEditApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KEYEDIT_H__3396C683_EE3F_4E19_8DA0_2D3BBC69F95E__INCLUDED_)
