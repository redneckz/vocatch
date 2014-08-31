// Avi.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "Avi.h"

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
// CAviApp

BEGIN_MESSAGE_MAP(CAviApp, CWinApp)
	//{{AFX_MSG_MAP(CAviApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAviApp construction

CAviApp::CAviApp()
{

	InitializeCriticalSection(&m_cs);

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_hExitSendEvent = CreateEvent(NULL,FALSE,FALSE,NULL);			//exit from	Stop
	m_hMakeSendEvent  = CreateEvent(NULL,TRUE,FALSE,NULL);			//Make Send- Manual Reset Event

	m_hExitReadEvent = CreateEvent(NULL,FALSE,FALSE,NULL);			//exit from read thread
	m_hMakeReadEvent  = CreateEvent(NULL,TRUE,FALSE,NULL);			//Make Read - Manual Reset Event

	//read semaphore initial by dwFrames
	m_hSemaphoreRead = CreateSemaphore(NULL,0,1,NULL);
	//write semaphore now initial to zero - becouse buffer is full
	m_hSemaphoreWrite = CreateSemaphore(NULL,0,1,NULL);

	m_htSendThread = NULL;
	m_htReadThread = NULL;
	
	m_dwCacheFrames = 100;

	m_chkUseCache = TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAviApp Destruction

CAviApp::~CAviApp()
{
	CloseHandle(m_hExitSendEvent);
	CloseHandle(m_hMakeSendEvent);
	CloseHandle(m_hExitReadEvent);
	CloseHandle(m_hMakeReadEvent);

	if(m_hSemaphoreRead) CloseHandle(m_hSemaphoreRead);
	if(m_hSemaphoreWrite) CloseHandle(m_hSemaphoreWrite);

	DeleteCriticalSection(&m_cs);

	//clear old Precache
	int nSize = m_arrFrames.GetSize();
	if(nSize>0)
	{
		for(int i=0;i<nSize;i++)
		{
			delete  m_arrFrames[i]->pDataPtr;
			delete  m_arrFrames[i];
		}

		m_arrFrames.RemoveAll();

	}

	m_aviFilm.CloseAviFile();
}

/////////////////////////////////////////////////////////////////////////////
//
//
CAviApp::GetResolution(SIZE *psizeResolution)
{
	BITMAPINFO bmpHeader;

	EnterCriticalSection(&m_cs);

	m_aviFilm.GetBmpHeader(&bmpHeader);

	LeaveCriticalSection(&m_cs);

	psizeResolution->cx = bmpHeader.bmiHeader.biWidth;
	psizeResolution->cy = bmpHeader.bmiHeader.biHeight;
}

/////////////////////////////////////////////////////////////////////////////
//
//
CAviApp::Initialize(PDATA2COMPRESS_ROUTINE pRoutineAddress,LPVOID	pParam)
{
	//Stop read and send thread
	if(!m_htReadThread)
		StartCapture();

	Stop();

	m_pCompressRoutine = pRoutineAddress;					//set pointer to compress routine
	m_pParent = pParam;										//set sender pointer

	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	COptionsDialog	* dlgOptions = new COptionsDialog(this);

	if(dlgOptions->DoModal()!=IDOK)
	{
		delete dlgOptions;
		return TRUE;
	}

	delete dlgOptions;
	return FALSE;
}


////////////////////////////////////////////////////
// Stop Capture Thread
////////////////////////////////////////////////////
CAviApp::Stop()
{
	//set event to Read Avi Frame
	ResetEvent(m_hMakeReadEvent);

	ResetEvent(m_hMakeSendEvent);

}

////////////////////////////////////////////////////
// Start Capture Thread
////////////////////////////////////////////////////
CAviApp::Start()
{
	//set event to Read Avi Frame
	SetEvent(m_hMakeReadEvent);

	//set event to send result to compressor
	SetEvent(m_hMakeSendEvent);


}


////////////////////////////////////////////////////
//
CAviApp::StartCapture()
{
	DWORD	dwThreadID;
	//thread for send to compressor
	m_htSendThread = ::CreateThread(NULL,0,SendThread,this,0,&dwThreadID);
	//thread for read from avi file
	m_htReadThread = ::CreateThread(NULL,0,ReadThread,this,0,&dwThreadID);
}

////////////////////////////////////////////////////
//
CAviApp::ExitCapture()
{
	//end of capturing frame from Avi
	SetEvent(m_hExitReadEvent);
	WaitForSingleObject(m_htReadThread,INFINITE);

	//end of draw frame
	SetEvent(m_hExitSendEvent);
	WaitForSingleObject(m_htSendThread,INFINITE);

	CloseHandle(m_htSendThread);
	CloseHandle(m_htReadThread);

	m_htSendThread = NULL;
	m_htReadThread = NULL;

}

DWORD	CAviApp::OpenAvi(CString strName)
{
	m_strName = strName;


	m_aviFilm.CloseAviFile();

	if(m_aviFilm.OpenAviFile(strName.GetBuffer(256)))
		return 1;

	BITMAPINFO	bmpInfo;

	m_aviFilm.GetBmpHeader(&bmpInfo);

	EnterCriticalSection(&m_cs);

	m_dwPopIndex = 0;
	m_dwPushIndex = 0;

	LeaveCriticalSection(&m_cs);

	return 0;
}

void	CAviApp::RewindAvi()
{

	m_aviFilm.ToStart();

}


////////////////////////////////////////////////////
//threads for comperss/decompress
////////////////////////////////////////////////////
DWORD WINAPI CAviApp::SendThread(LPVOID pParam)
{
	CAviApp *pThis = (CAviApp*)pParam;		//get this of class
	pThis->SendEngine();

	return 0;
}




////////////////////////////////////////////////////
//Engine for ToCompressThread
////////////////////////////////////////////////////
CAviApp::SendEngine()											
{
	HANDLE	hWorkEvents[2];
	HANDLE	hSemaphoreEvents[2];


	hWorkEvents[0] = m_hExitSendEvent;
	hWorkEvents[1] = m_hMakeSendEvent;				//manual reset event

	hSemaphoreEvents[0] = m_hExitSendEvent;
	hSemaphoreEvents[1] = m_hSemaphoreRead;				//semaphore for compress

	while(true)
	{
		//wait for 2 events - 'Exit' or 'Make Send' event
		DWORD dwWait = WaitForMultipleObjects(2,hWorkEvents,FALSE,INFINITE);		

		if(dwWait == WAIT_OBJECT_0)									//we get Exit Event -> Terminate thread
		{
			break;
		}
		else if(dwWait == WAIT_OBJECT_0+1)							//we get Make Send
		{


			DWORD	dwSemaphoreWait = WaitForMultipleObjects(2,hSemaphoreEvents,FALSE,INFINITE);

			if(dwSemaphoreWait == WAIT_OBJECT_0)					//we get Exit Event -> Terminate thread
			{
				break;
			}
			else if(dwSemaphoreWait == WAIT_OBJECT_0+1)				//we get ReleaseSemaphore
			{
				Sleep(30);

				EnterCriticalSection(&m_cs);

				m_arrFrames[m_dwPopIndex]->hSemaphore = m_hSemaphoreWrite;
				//the codec after compress must call ReleaseSemaphore(m_hSemaphoreWrite) 
				m_pCompressRoutine(m_arrFrames[m_dwPopIndex],m_pParent);		//process frame

				m_dwPopIndex++;

				if(m_dwPopIndex >= (DWORD)m_arrFrames.GetSize()) m_dwPopIndex = 0;

				LeaveCriticalSection(&m_cs);
			}

		}

	}//while true

	return 0;
}


////////////////////////////////////////////////////
//threads for Read from Avi
////////////////////////////////////////////////////
DWORD WINAPI CAviApp::ReadThread(LPVOID pParam)
{
	CAviApp *pThis = (CAviApp*)pParam;		//get this of class
	pThis->ReadEngine();

	return 0;
}


////////////////////////////////////////////////////
//
////////////////////////////////////////////////////
CAviApp::MakePrecache(DWORD	dwPrecacheFrames)
{
	DWORD	dwFrame = 0;
	
	SData2Compress	*pImage;

	BITMAPINFO	bmpInfo;
	m_aviFilm.GetBmpHeader(&bmpInfo);

	DWORD	dwImageSize = m_aviFilm.GetImageSize();

	//clear old Precache
	int nSize = m_arrFrames.GetSize();

	if(nSize>0)
	{
		for(int i=0;i<nSize;i++)
		{
			delete  m_arrFrames[i]->pDataPtr;
			delete  m_arrFrames[i];
		}

		m_arrFrames.RemoveAll();

	}


	//Create current Precache Load
	while(true)
	{
		pImage = new	SData2Compress;
		pImage->pDataPtr = new BYTE[m_aviFilm.GetImageSize()];

		//get next frame from avi
		if(!m_aviFilm.GetFrame(pImage->pDataPtr))
		{

			//set ReadyEvent to NULL
			pImage->hReadyEvent = NULL;

			//support only RGB24 Avi files
			pImage->dwColorFromat = RVCL_CF_RGB24;

			//set image resolution
			pImage->sizeResolution.cx = bmpInfo.bmiHeader.biWidth;
			pImage->sizeResolution.cy = bmpInfo.bmiHeader.biHeight;

			//set size of structute and size of data
			pImage->dwSize = sizeof(SData2Compress);
			pImage->dwDataSize = dwImageSize;

			//add to the array of images
			m_arrFrames.Add(pImage);

		}
		else
		{
			delete pImage->pDataPtr;
			delete pImage;
			break;
		}

		dwFrame++;
		//check number frames for precache
		if(dwFrame >= dwPrecacheFrames) break;

	}

	ExitCapture();

	if(m_hSemaphoreRead) CloseHandle(m_hSemaphoreRead);
	if(m_hSemaphoreWrite) CloseHandle(m_hSemaphoreWrite);


	//read semaphore initial by dwFrames
	m_hSemaphoreRead = CreateSemaphore(NULL,dwFrame,dwFrame,NULL);
	//write semaphore now initial to zero - becouse buffer is full
	m_hSemaphoreWrite = CreateSemaphore(NULL,0,dwFrame,NULL);

	StartCapture();
}

////////////////////////////////////////////////////
//Engine for Read from Avi thread
////////////////////////////////////////////////////
CAviApp::ReadEngine()											
{
	HANDLE	hSemaphoreEvents[2];								//events - stop,sendtocompress.

	HANDLE	hWorkEvents[2];


	hWorkEvents[0] = m_hExitReadEvent;
	hWorkEvents[1] = m_hMakeReadEvent;				//manual reset event


	hSemaphoreEvents[0] = m_hExitReadEvent;
	hSemaphoreEvents[1] = m_hSemaphoreWrite;				//semaphore for compress


	while(true)
	{
		//wait for 2 events - 'Exit' or 'Make Send' event
		DWORD dwWait = WaitForMultipleObjects(2,hWorkEvents,FALSE,INFINITE);		

		if(dwWait == WAIT_OBJECT_0)									//we get Exit Event -> Terminate thread
		{
			break;
		}
		else if(dwWait == WAIT_OBJECT_0+1)							//we get Make Send
		{

			//wait for exit or semaphore for write to stream
			DWORD	dwSemaphoreWait = WaitForMultipleObjects(2,hSemaphoreEvents,FALSE,INFINITE);

			if(dwSemaphoreWait == WAIT_OBJECT_0)					//we get Exit Event -> Terminate thread
			{
				break;
			}
			else if(dwSemaphoreWait == WAIT_OBJECT_0+1)				//we get ReleaseSemaphore
			{

				EnterCriticalSection(&m_cs);

				if(!m_aviFilm.GetFrame(m_arrFrames[m_dwPushIndex]->pDataPtr))
				{

					m_dwPushIndex++;
					if(m_dwPushIndex >= (DWORD)m_arrFrames.GetSize()) m_dwPushIndex = 0;


					ReleaseSemaphore(m_hSemaphoreRead,1,NULL);		//set semaphore for Read
				}
				else		//end of Avi file - stop thread
				{
					ResetEvent(m_hMakeReadEvent);
				}


				LeaveCriticalSection(&m_cs);

			}

		}//get semaphore

	}//while (true)

	return 0;

}


/////////////////////////////////////////////////////////////////////////////
// The one and only CAviApp object

CAviApp theApp;
