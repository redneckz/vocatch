// Avi.h : main header file for the AVI DLL
//

#if !defined(AFX_AVI_H__78DC6730_0E12_40B3_AA30_06FE449E91E6__INCLUDED_)
#define AFX_AVI_H__78DC6730_0E12_40B3_AA30_06FE449E91E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "data2compress.h"
#include "OptionsDialog.h"
#include "AviMan.h"

#include "..\..\..\include\rvcl.h"

#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// CAviApp
// See Avi.cpp for the implementation of this class
//



/////////////////////////////////////////////////////////////////////////////
// Image Container


class CAviApp : public CWinApp
{
public:


	CAviApp();


	~CAviApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAviApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CAviApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	//call from .EXE to initialize library
	BOOL Initialize(PDATA2COMPRESS_ROUTINE pRoutineAddress,LPVOID	pParam);

	GetResolution(SIZE *psizeResolution);

	//call from .EXE to stop Frame Capture Thread
	Stop();

	Start();

	ExitCapture();

	StartCapture();

	DWORD	CAviApp::OpenAvi(CString strName);

	void RewindAvi();

	MakePrecache(DWORD	dwPrecacheSize);					//preload some frames (for example preload 20 megabyetes)

	CString	m_strName;
	BOOL m_chkUseCache;
	DWORD	m_dwCacheFrames;

	DWORD	m_dwPopIndex;
	DWORD	m_dwPushIndex;

private:

	static DWORD WINAPI SendThread(LPVOID pParam);		//threads for comperss/decompress
	SendEngine();											//Engine for ToCompressThread

	static DWORD WINAPI ReadThread(LPVOID pParam);			//threads for Read from Avi
	ReadEngine();											//Engine for Read from Avi thread


private:


	LPVOID	m_pParent;										//pointer to Codec class
	PDATA2COMPRESS_ROUTINE	m_pCompressRoutine;				//pointer to compress routine

    CArray <SData2Compress*,SData2Compress*>	m_arrFrames;//List of Frame ready to send


	HANDLE	m_htSendThread;
	HANDLE	m_htReadThread;

	HANDLE	m_hExitSendEvent;								//exit from send to compress event
	HANDLE	m_hMakeSendEvent;								//play or pause
	HANDLE	m_hExitReadEvent;								//exit from read event
	HANDLE	m_hMakeReadEvent;								//play or pause

	HANDLE m_hSemaphoreWrite;
	HANDLE m_hSemaphoreRead;

	CRITICAL_SECTION m_cs;
	CAviMan	m_aviFilm;										//avi file

};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AVI_H__78DC6730_0E12_40B3_AA30_06FE449E91E6__INCLUDED_)
