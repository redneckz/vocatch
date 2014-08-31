// CaptureDevice.h: interface for the CCaptureDevice class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAPTUREDEVICE_H__DFA9FA6C_26F2_46F1_886D_580D9757F871__INCLUDED_)
#define AFX_CAPTUREDEVICE_H__DFA9FA6C_26F2_46F1_886D_580D9757F871__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>

//////////////////////////////////////////////////////////////////////

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
		CloseHandle(m_overlapped.hEvent), m_overlapped.hEvent = 0;
		CloseHandle(m_heReady), m_heReady = 0;
		CloseHandle(m_htCheckingTimer), m_htCheckingTimer = 0;

		delete [] m_rvclf.pBufferPtr, m_rvclf.pBufferPtr = 0;
		m_rvclf.dwBufferSize = 0;
	}
};

//////////////////////////////////////////////////////////////////////

class CMultiScreenView;

class CCaptureDevice  
{
public:
	CCaptureDevice(int nDeviceNumber,CMultiScreenView* pView);
	virtual ~CCaptureDevice();

	bool	Create();
	void	Destroy();
	bool	Start();
	void	Stop();
	void	SetColorFormat(DWORD dwColorFormat);
	void	SetVideoFormat(DWORD dwVideoFormat);
	void	SetMultiplexingMode(bool fMultiplexingMode);
	void	SetImageResolution(const SIZE &size);

private:
	static DWORD WINAPI CaptureThread(LPVOID lpParameter);
	
	DWORD	CaptureEngine();
	void	DestroyImages();

private:
	// Device #
	int m_nDeviceNumber;

	// Multiplexing mode flag
	bool m_fMultiplexingMode;

	// Capture thread
	HANDLE m_htCaptureThread;

	// "Stop" event for all threads
	HANDLE m_heStop;

	// Images
	CArray<CImage*,CImage*> m_arrImages;

	// View pointer
	CMultiScreenView* m_pView;

	// Current capture parameters:
	DWORD		m_dwColorFormat;			// current color format
	DWORD		m_dwVideoFormat;			// current video format
	CSize		m_sizeMinResolution,		// minimum image resolution
				m_sizeMaxResolution,		// maximum image resolution
				m_sizeAlignment,
				m_sizeImageResolution;

	CRITICAL_SECTION m_csCaptureParameters;
};

#endif // !defined(AFX_CAPTUREDEVICE_H__DFA9FA6C_26F2_46F1_886D_580D9757F871__INCLUDED_)
