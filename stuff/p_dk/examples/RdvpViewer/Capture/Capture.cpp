// Capture.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "Capture.h"
#include "OptionDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CCaptureApp

BEGIN_MESSAGE_MAP(CCaptureApp, CWinApp)
	//{{AFX_MSG_MAP(CCaptureApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCaptureApp construction

CCaptureApp::CCaptureApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

	m_dwFormat = RVCL_VF_PALBDGHI;
	m_nDevice = 0;
	m_nInput = 0;

	m_fInitRVCL = FALSE;

	m_htCaptureThread = 0;
	m_hStopCaptureEvent = 0;
}


/////////////////////////////////////////////////////////////////////////////
// CCaptureApp destruction

CCaptureApp::~CCaptureApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

	for(int i=0; i<m_arrImages.GetSize(); i++)
	{
		m_arrImages[i]->Destroy();
		delete m_arrImages[i];
	}

	m_arrImages.RemoveAll();

	if(m_hStopCaptureEvent) CloseHandle(m_hStopCaptureEvent);
	if(m_htCaptureThread) CloseHandle(m_htCaptureThread);

	DeleteCriticalSection(&m_cs);


}

////////////////////////////////////////////////////
//
////////////////////////////////////////////////////
CCaptureApp::ExitCapture()
{
	::ResumeThread(m_htCaptureThread);
	if(m_hStopCaptureEvent)  SetEvent(m_hStopCaptureEvent);
	if(m_htCaptureThread) WaitForSingleObject(m_htCaptureThread, INFINITE);
}


////////////////////////////////////////////////////
// Create Capture Thread, Image arrays etc...
////////////////////////////////////////////////////
BOOL CCaptureApp::InitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class

	for(int i=0; i<4; i++)
	{
		CImage* pImage = new CImage;
		ASSERT(pImage);

		pImage->m_nDevice = m_nDevice;
		pImage->m_dwVideoSource = 0;

		if(!pImage->Create())
		{
			ASSERT(false);
			return false;
		}

		m_arrImages.Add(pImage);
	}

	m_hStopCaptureEvent = CreateEvent(NULL,FALSE,FALSE,NULL);

	//create critical section
	InitializeCriticalSection(&m_cs);

	// Create Compress thread (in a suspended state)
	DWORD dwThreadID;		

	//thread for capture live video routine
	m_htCaptureThread = 
		::CreateThread(NULL,0,CaptureThread,this,CREATE_SUSPENDED,&dwThreadID);

	
	return CWinApp::InitInstance();
}


////////////////////////////////////////////////////
// Initialize RVCL Library and resume Capture thread
////////////////////////////////////////////////////
BOOL CCaptureApp::Initialize(PDATA2COMPRESS_ROUTINE pRoutineAddress,LPVOID	pParam)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pCompressRoutine = pRoutineAddress;

	m_pParent = pParam;

	if(m_fInitRVCL==FALSE)
	{
		SRvclInitialize rvcl_init;

		rvcl_init.dwSize = sizeof(rvcl_init);
	
		// Use Demo Key passwords
		rvcl_init.pdwCustomerPassword[0] = 0x9592CB4F;
		rvcl_init.pdwCustomerPassword[1] = 0xF3B62AC0;
		rvcl_init.pdwCustomerPassword[2] = 0xFCB8802F;
		rvcl_init.pdwCustomerPassword[3] = 0x9CE9CEF9;
	
		if(!RvclInitializeEx(&rvcl_init))
		{
			CString strMsg;
			CString	strCap;

			strMsg.LoadString(IDS_ERROR_RVCL_INIT);
			strCap.LoadString(IDS_ERROR);
			MessageBox(NULL,strMsg,strCap,MB_OK|MB_ICONSTOP);

			return TRUE;
		}

		m_fInitRVCL = TRUE;		//Init RVCL only at once
	}

	::ResumeThread(m_htCaptureThread);

	COptionDialog	* dlgOptions = new COptionDialog(this);

	if(dlgOptions->DoModal()!=IDOK)
	{
		delete dlgOptions;
		return TRUE;
	}



	delete	dlgOptions;
	return FALSE;


}

////////////////////////////////////////////////////
// Set Captured Color Format
////////////////////////////////////////////////////
CCaptureApp::SetColorFormat(DWORD	dwColorFormat)
{
	EnterCriticalSection(&m_cs);

	m_dwColorFormat = dwColorFormat;

	LeaveCriticalSection(&m_cs);
}

////////////////////////////////////////////////////
// Set Resolution (cx,cy)
////////////////////////////////////////////////////
CCaptureApp::SetResolution(SIZE		szResolution)
{
	EnterCriticalSection(&m_cs);

	m_szResolution = szResolution;

	LeaveCriticalSection(&m_cs);
}



////////////////////////////////////////////////////////////////////
// Video capture thread
////////////////////////////////////////////////////////////////////
DWORD WINAPI  CCaptureApp::CaptureThread(LPVOID pParam)	
{
	CCaptureApp *pThis = (CCaptureApp*)pParam;		//get this of class
	pThis->CaptureEngine();

	return 0;
}
////////////////////////////////////////////////////////////////////
// Video capture engine
////////////////////////////////////////////////////////////////////
CCaptureApp::CaptureEngine()
{
	// Handles to wait:
	// 0										- "stop" event,
	// 1 .. ImagesCount							- OVERLAPPED events,
	// [ImagesCount + 1] .. [2 * ImagesCount]	- waitable timers,
	// [2 * ImagesCount+1] .. [3 * ImagesCount]	- "ready" events

	int		nHandles = m_arrImages.GetSize() * 3 + 1;
	HANDLE*	hHandles = new HANDLE[nHandles];

	hHandles[0] = m_hStopCaptureEvent;

	for(int i=0; i<m_arrImages.GetSize(); i++)
	{
		// Add OVELAPPED event
		hHandles[i+1] = m_arrImages[i]->m_overlapped.hEvent;

		// Add waitable timer
		hHandles[i+m_arrImages.GetSize()+1] = m_arrImages[i]->m_htCheckingTimer;

		// Add "ready" event
		hHandles[i+2*m_arrImages.GetSize()+1] = m_arrImages[i]->m_heReady;

		// Set the state of the OVERLAPPED event object to signaled
		// (to start capture process immediately).
		//m_arrImages[i]->m_dwError = ERROR_SUCCESS;
		//SetEvent(m_arrImages[i]->overlapped.m_hEvent);
	}

	while(true)
	{
		DWORD dwWait = WaitForMultipleObjects(nHandles,hHandles,false,INFINITE);

		if(dwWait == WAIT_OBJECT_0)
		{
			// "Stop" event
			break;
		}
		
		int		nImage = -1;
		CImage*	pImage = NULL;

		if(dwWait <= (WAIT_OBJECT_0 + m_arrImages.GetSize()))
		{
			// OVERLAPPED event	(async operation result)

			nImage = dwWait - WAIT_OBJECT_0 - 1;
			pImage = m_arrImages[nImage];
			
			if(pImage->m_dwError == ERROR_IO_PENDING)
			{
				ASSERT(HasOverlappedIoCompleted(&pImage->m_overlapped));
				
				DWORD dwBytesWritten;
			
				EnterCriticalSection(&m_cs);
				// Get capture result
				if(!RvclGetGrabFrameResult(
					m_nDevice,
					&pImage->m_overlapped,
					&dwBytesWritten,
					FALSE))
				{
					pImage->m_dwError = GetLastError();
				}
				else
					pImage->m_dwError = ERROR_SUCCESS;

				LeaveCriticalSection(&m_cs);

				// Here we must call Codec routine
				// Fill SData2Compress structure
				SData2Compress	sData;

				sData.dwSize = sizeof(sData);

				sData.dwColorFromat = pImage->m_rvclf.dwColorFormat;
				sData.sizeResolution = pImage->m_rvclf.sizeResolution;

				sData.dwDataSize = pImage->m_rvclf.dwBufferSize;
				sData.hReadyEvent = pImage->m_heReady;
				sData.pDataPtr = (LPBYTE) pImage->m_rvclf.pBufferPtr;
				sData.hSemaphore = NULL;

				//call Compress/Decompress Routine
				m_pCompressRoutine(&sData,m_pParent);

				if(pImage->m_dwError != ERROR_SUCCESS)
				{
					Sleep(20);
				}
			}
		}
		else if(dwWait <= (WAIT_OBJECT_0 + 2*m_arrImages.GetSize()))
		{
			// Waitable timer (async operation timeout)

			nImage = dwWait - m_arrImages.GetSize() - WAIT_OBJECT_0 - 1;
			
			// Cancel all pending operations
			EnterCriticalSection(&m_cs);

			RvclCancelIo(m_nDevice);

			LeaveCriticalSection(&m_cs);

			// Wait for OVELAPPED event(s)...
		}
		else if(dwWait <= (WAIT_OBJECT_0 + 3*m_arrImages.GetSize()))
		{
			// "Ready" event

			nImage = dwWait - 2*m_arrImages.GetSize() - WAIT_OBJECT_0 - 1;
			pImage = m_arrImages[nImage];
			
			pImage->m_rvclf.dwSize = sizeof(pImage->m_rvclf);
			pImage->m_rvclf.dwFlags = RVCL_GRAB_FIELD_EVEN;
			
			EnterCriticalSection(&m_cs);

			pImage->m_rvclf.dwColorFormat = m_dwColorFormat;
			pImage->m_rvclf.sizeResolution =m_szResolution;
			//pImage->m_rvclf.dwVideoSource = m_nInput;

			// Start grab operation
			if(RvclGrabFrame(m_nDevice,&pImage->m_rvclf,&pImage->m_overlapped))
			{
				LeaveCriticalSection(&m_cs);

				pImage->m_dwError = ERROR_SUCCESS;
			}
			else
			{
				LeaveCriticalSection(&m_cs);

				pImage->m_dwError = GetLastError();

				if(pImage->m_dwError == ERROR_IO_PENDING)
				{
					// Pending operation - set up waitable timer (1 second).
					LARGE_INTEGER nDueTime;
					nDueTime.QuadPart = Int32x32To64(-10000,1000);
					
					SetWaitableTimer(pImage->m_htCheckingTimer,&nDueTime,0,NULL,NULL,FALSE);
				}
				else
				{
					// Other error
					Sleep(20);
				}
			}


		}
		else
		{
			ASSERT(false);
		}
	}	

	// Cancel all pending requests
	for(i=0; i<m_arrImages.GetSize(); i++)
		if(m_arrImages[i]->m_dwError == ERROR_IO_PENDING)
		{
			EnterCriticalSection(&m_cs);

			RvclCancelIo(m_nDevice);
			CancelWaitableTimer(m_arrImages[i]->m_htCheckingTimer);

			WaitForSingleObject(m_arrImages[i]->m_overlapped.hEvent,1000);

			LeaveCriticalSection(&m_cs);
		}

	delete [] hHandles;

	return 0;
}


//////////////////////////////////////////////////////////////////////////
// function set new source device and new Format of Video(PAL,NTSC ,etc)
//////////////////////////////////////////////////////////////////////////
DWORD	CCaptureApp::SetVideoOptions(int nDevice,int nInput,DWORD dwFormat)
{

	EnterCriticalSection(&m_cs);
	m_nDevice = nDevice;
	m_nInput = nInput;
	m_dwFormat = dwFormat;

	// Set new source
	if(!RvclSetVideoSource(m_nDevice,m_nInput))
	{
		ASSERT(false);
//		return 1;
	}

	// Set new format
	if(!RvclSetVideoFormat(m_nDevice,m_dwFormat))
	{
		ASSERT(false);
//		return 1;

	}

	LeaveCriticalSection(&m_cs);
	return 0;
}



/////////////////////////////////////////////////////////////////////////////
// The one and only CCaptureApp object

CCaptureApp theApp;
