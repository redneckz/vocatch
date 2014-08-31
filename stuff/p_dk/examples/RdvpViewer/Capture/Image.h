#ifndef	_CIMAGE_H
#define _CIMAGE_H

#include "..\..\..\include\rvcl.h"
#include <afxtempl.h>

class CImage
{
public:
	int			m_nDevice;			// Zero-based device index
	DWORD		m_dwVideoSource;	// Zero-based video source
	DWORD		m_dwError;			// Last Win32 API error
	SRvclFrame	m_rvclf;			// RVCL frame settings
	OVERLAPPED	m_overlapped;		// Win32 OVERLAPPED
	HANDLE		m_heReady;			// Ready to capture
	HANDLE		m_htCheckingTimer;	// Waitable timer

	CImage()
	{
		m_nDevice = -1;
		m_dwVideoSource = 0;
		m_dwError = 0;
		memset(&m_rvclf,0,sizeof(m_rvclf));
		memset(&m_overlapped,0,sizeof(m_overlapped));
		m_heReady = 0;
		m_htCheckingTimer = 0;
	}
	bool Create()
	{
		// Create "overlapped" auto-reset event
		m_overlapped.hEvent = CreateEvent(NULL,false,false,NULL);
		
		if(!m_overlapped.hEvent)
		{
			ASSERT(false);
			Destroy();
			return false;
		}

		// Create "ready" auto-reset event
		m_heReady = CreateEvent(NULL,false,true,NULL);

		if(!m_heReady)
		{
			ASSERT(false);
			Destroy();
			return false;
		}

		// Create checking auto-reset timer
		m_htCheckingTimer = CreateWaitableTimer(NULL,false,NULL);
		
		if(!m_htCheckingTimer)
		{
			ASSERT(false);
			Destroy();
			return false;
		}

		// Allocate image buffer
		static DWORD dwMaximumBufferSize = 768*576*4;

		m_rvclf.pBufferPtr = new BYTE[dwMaximumBufferSize];
		m_rvclf.dwBufferSize = dwMaximumBufferSize;
		
		if(!m_rvclf.pBufferPtr)
		{
			ASSERT(false);
			Destroy();
			return false;
		}

		return true;
	}
	void Destroy()
	{

		CloseHandle(m_overlapped.hEvent);//, m_overlapped.hEvent = 0;
		CloseHandle(m_heReady);//, m_heReady = 0;
		CloseHandle(m_htCheckingTimer);//, m_htCheckingTimer = 0;

		delete [] m_rvclf.pBufferPtr, m_rvclf.pBufferPtr = 0;
		m_rvclf.dwBufferSize = 0;
	}
};


#endif