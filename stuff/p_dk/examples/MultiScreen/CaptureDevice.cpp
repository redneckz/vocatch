// CaptureDevice.cpp: implementation of the CCaptureDevice class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MultiScreen.h"
#include "CaptureDevice.h"
#include "MultiScreenView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCaptureDevice::CCaptureDevice(int nDeviceNumber,CMultiScreenView* pView)
{
	m_nDeviceNumber = nDeviceNumber;
	m_pView = pView;
	m_fMultiplexingMode = false;
	m_htCaptureThread = 0;
	m_heStop = 0;
	m_dwColorFormat = RDN_CF_RGB24;
	m_sizeMinResolution = CSize(48,36);
	m_sizeMaxResolution = CSize(768,576);
	m_sizeAlignment = CSize(16,1);
	m_sizeImageResolution = CSize(384,288);
	m_fMultiplexingMode = false;

	InitializeCriticalSection(&m_csCaptureParameters);
}

CCaptureDevice::~CCaptureDevice()
{
	Stop();
	Destroy();
	
	DeleteCriticalSection(&m_csCaptureParameters);
}

void CCaptureDevice::SetColorFormat(DWORD dwColorFormat)
{
	EnterCriticalSection(&m_csCaptureParameters);
	m_dwColorFormat = dwColorFormat;
	LeaveCriticalSection(&m_csCaptureParameters);
}

void CCaptureDevice::SetVideoFormat(DWORD dwVideoFormat)
{
	EnterCriticalSection(&m_csCaptureParameters);
	m_dwVideoFormat = dwVideoFormat;

	RvclSetVideoFormat(m_nDeviceNumber, m_dwVideoFormat);

	SRvclDeviceStatus rvclds;
	
	rvclds.dwSize = sizeof(rvclds);
	rvclds.dwMask = RVCL_STATUS_RESOLUTION;
	
	if(RvclGetDeviceStatus(m_nDeviceNumber,&rvclds))
	{
		m_sizeMaxResolution = rvclds.sizeMaxResolution;
		m_sizeMinResolution = rvclds.sizeMinResolution;
		m_sizeAlignment = rvclds.sizeResolutionAlignment;
	}

	LeaveCriticalSection(&m_csCaptureParameters);
}

void CCaptureDevice::SetMultiplexingMode(bool fMultiplexingMode)
{
	EnterCriticalSection(&m_csCaptureParameters);
	m_fMultiplexingMode = fMultiplexingMode;
	LeaveCriticalSection(&m_csCaptureParameters);
}

void CCaptureDevice::SetImageResolution(const SIZE &size)
{
	EnterCriticalSection(&m_csCaptureParameters);
	CSize sizeImageResolution = size;
	
	if((sizeImageResolution.cx > m_sizeMaxResolution.cx))
		sizeImageResolution.cx = m_sizeMaxResolution.cx;
	else
		if(sizeImageResolution.cx < m_sizeMinResolution.cx)
			sizeImageResolution.cx = m_sizeMinResolution.cx;
		
	if(sizeImageResolution.cy > m_sizeMaxResolution.cy)
		sizeImageResolution.cy = m_sizeMaxResolution.cy;
	else
		if(sizeImageResolution.cy < m_sizeMinResolution.cy)
			sizeImageResolution.cy = m_sizeMinResolution.cy;

	m_sizeImageResolution = sizeImageResolution;
	LeaveCriticalSection(&m_csCaptureParameters);
}

bool CCaptureDevice::Create()
{
	ASSERT(m_arrImages.GetSize() == 0);
	ASSERT(!m_htCaptureThread);
	ASSERT(!m_heStop);

	for(int i=0; i<4; i++)
	{
		CImage* pImage = new CImage;
		ASSERT(pImage);

		pImage->m_nDevice = m_nDeviceNumber;
		pImage->m_dwVideoSource = i;

		if(!pImage->Create())
		{
			Destroy();
			return false;
		}

		m_arrImages.Add(pImage);
	}
	
	m_heStop = CreateEvent(NULL,true,false,NULL);
	ASSERT(m_heStop);

	if(!m_heStop)
	{
		Destroy();
		return false;
	}

	m_htCaptureThread = CreateThread(NULL,0,CaptureThread,this,CREATE_SUSPENDED,NULL);
	ASSERT(m_htCaptureThread);

	if(!m_htCaptureThread)
	{
		Destroy();
		return false;
	}

	return true;
}

void CCaptureDevice::Destroy()
{
	for(int i=0; i<m_arrImages.GetSize(); i++)
	{
		m_arrImages[i]->Destroy();
		delete m_arrImages[i];
	}

	m_arrImages.RemoveAll();

	CloseHandle(m_heStop), m_heStop = 0;
	CloseHandle(m_htCaptureThread), m_htCaptureThread = 0;
}

bool CCaptureDevice::Start()
{
	RvclSetVideoFormat(m_nDeviceNumber, RVCL_VF_PALBDGHI);
	RvclSetVideoSource(m_nDeviceNumber, 0);

	ASSERT(m_htCaptureThread);
	ResumeThread(m_htCaptureThread);
	return true;
}

void CCaptureDevice::Stop()
{
	if(m_htCaptureThread)
	{
		SetEvent(m_heStop);
		WaitForSingleObject(m_htCaptureThread,30000);
	}
}

// static 
DWORD WINAPI CCaptureDevice::CaptureThread(LPVOID lpParameter)
{
	ASSERT(lpParameter);
	return static_cast<CCaptureDevice*>(lpParameter)->CaptureEngine();
}

DWORD CCaptureDevice::CaptureEngine()
{
	// Handles to wait:
	// 0										- "stop" event,
	// 1 .. ImagesCount							- OVERLAPPED events,
	// [ImagesCount + 1] .. [2 * ImagesCount]	- waitable timers,
	// [2 * ImagesCount+1] .. [3 * ImagesCount]	- "ready" events
	
	int		nHandles = m_arrImages.GetSize() * 3 + 1;
	HANDLE*	hHandles = new HANDLE[nHandles];

	hHandles[0] = m_heStop;

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
			
				// Get capture result
				if(!RvclGetGrabFrameResult(
					m_nDeviceNumber,
					&pImage->m_overlapped,
					&dwBytesWritten,
					FALSE))
				{
					pImage->m_dwError = GetLastError();
				}
				else
					pImage->m_dwError = ERROR_SUCCESS;

				// Show image or error
				m_pView->DrawImage(pImage);

				if((pImage->m_dwError != ERROR_SUCCESS) && !m_fMultiplexingMode)
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
			RvclCancelIo(m_nDeviceNumber);

			// Wait for OVELAPPED event(s)...
		}
		else if(dwWait <= (WAIT_OBJECT_0 + 3*m_arrImages.GetSize()))
		{
			// "Ready" event

			nImage = dwWait - 2*m_arrImages.GetSize() - WAIT_OBJECT_0 - 1;
			pImage = m_arrImages[nImage];
			
			pImage->m_rvclf.dwSize = sizeof(pImage->m_rvclf);
			
			pImage->m_rvclf.dwFlags = RVCL_GRAB_FIELD_EVEN|RVCL_GRAB_AFTER_SWITCH;
			
			EnterCriticalSection(&m_csCaptureParameters);

			pImage->m_rvclf.dwColorFormat = m_dwColorFormat;
			pImage->m_rvclf.sizeResolution = m_sizeImageResolution;
			pImage->m_rvclf.dwVideoSource = m_fMultiplexingMode ? pImage->m_dwVideoSource : 0;

			LeaveCriticalSection(&m_csCaptureParameters);

			// Start grab operation
			if(RvclGrabFrame(m_nDeviceNumber,&pImage->m_rvclf,&pImage->m_overlapped))
			{
				pImage->m_dwError = ERROR_SUCCESS;
				m_pView->DrawImage(pImage);
			}
			else
			{
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
					m_pView->DrawImage(pImage);
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
			RvclCancelIo(m_nDeviceNumber);
			CancelWaitableTimer(m_arrImages[i]->m_htCheckingTimer);

			WaitForSingleObject(m_arrImages[i]->m_overlapped.hEvent,1000);
		}

	delete [] hHandles;
	return 0;
}
