// Capture.h : main header file for the CAPTURE DLL
//

#if !defined(AFX_CAPTURE_H__111992B3_74BD_41EB_A661_956764E60C5F__INCLUDED_)
#define AFX_CAPTURE_H__111992B3_74BD_41EB_A661_956764E60C5F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include <afxtempl.h>

#include "resource.h"		// main symbols

#include "Image.h"
#include "data2compress.h"
/////////////////////////////////////////////////////////////////////////////
// CCaptureApp
// See Capture.cpp for the implementation of this class
//


#define NUM_BUFFERS 4

class CCaptureApp : public CWinApp
{
public:
	CCaptureApp();

	~CCaptureApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCaptureApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CCaptureApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


public:
	BOOL Initialize(PDATA2COMPRESS_ROUTINE pRoutineAddress,LPVOID	pParam);

	SetColorFormat(DWORD dwColorFormat);

	SetResolution(SIZE	szResolution);

	DWORD	SetVideoOptions(int nDevice,int nInput,DWORD dwFormat);

	CaptureEngine();

	ExitCapture();

private:

	static DWORD WINAPI  CaptureThread(LPVOID pParam)	;
//	 UINT  CaptureThread(LPVOID pParam)	;

private:

	DWORD	m_fInitRVCL;

	LPVOID	m_pParent;

	PDATA2COMPRESS_ROUTINE	m_pCompressRoutine;				//pointer to compress routine

	DWORD		m_dwFlags;									//capture flags

	DWORD		m_dwColorFormat;
	SIZE		m_szResolution;

	HANDLE		m_htCaptureThread;

	CRITICAL_SECTION	m_cs;

	HANDLE		m_hStopCaptureEvent;
	// Requests

	CArray <CImage*,CImage*> m_arrImages;
public:

	int			m_nDevice ;								//
	int			m_nInput ;
	DWORD		m_dwFormat ;

};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CAPTURE_H__111992B3_74BD_41EB_A661_956764E60C5F__INCLUDED_)
