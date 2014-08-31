// VideoView.cpp : implementation of the CVideoView class
//

#include "stdafx.h"
#include "RvcViewer.h"

#include "VideoDoc.h"
#include "VideoView.h"
#include "MainFrm.h"

#include <math.h>
#include <comdef.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

// Maximum image buffer size = 768 * 576 * 4 bytes pre pixel (RGB32)
#define IMAGE_BUFFER_SIZE		768*576*4

// FPS update period (ms)
#define FPS_UPDATE_TIMEOUT		2000

/////////////////////////////////////////////////////////////////////////////
// CVideoView

IMPLEMENT_DYNCREATE(CVideoView, CView)

BEGIN_MESSAGE_MAP(CVideoView, CView)
	//{{AFX_MSG_MAP(CVideoView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_COMMAND(ID_WINDOW_FULL_FRAME, OnWindowFullFrame)
	ON_COMMAND(ID_WINDOW_QCIF, OnWindowQcif)
	ON_COMMAND(ID_WINDOW_CIF, OnWindowCif)
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_SHOW_IMAGE, OnShowImage)
	ON_WM_TIMER()
	ON_COMMAND(ID_IMAGE_TOP_DOWN, OnImageTopDown)
	ON_COMMAND(ID_IMAGE_BOTTOM_UP, OnImageBottomUp)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_TOP_DOWN, OnUpdateImageTopDown)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_BOTTOM_UP, OnUpdateImageBottomUp)
	ON_COMMAND(ID_WINDOW_DEINTERLACINGFILTER, OnWindowDeinterlacingfilter)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_DEINTERLACINGFILTER, OnUpdateWindowDeinterlacingfilter)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVideoView construction/destruction

CVideoView::CVideoView()
{
	// Create contol panel object
	m_pControlPanel = new CControlPanelDlg;
	ASSERT(m_pControlPanel);

	m_nDevice = 0;
	m_dwColorFormat = RDN_CF_RGB24;
	m_sizeMin = CSize(48,36);
	m_sizeMax = CSize(768,576);
	m_sizeAlignment = CSize(1,1);
	m_dwFlags = RVCL_GRAB_FIELD_ODD;
	m_dwFrameCount = 0;
	m_nReadyRequest = 0;
	m_hCaptureThread = NULL;
	m_hStopCaptureEvent = NULL;
	m_pFont = NULL;
	m_fDeinterlacing = FALSE;

	// Create requests
	for(int i=0; i<REQUESTS_NUMBER; i++)
	{
		m_aRequests[i].nDevice = -1;
		
		memset(&m_aRequests[i].rvclf,0,sizeof(m_aRequests[i].rvclf));
		memset(&m_aRequests[i].overlapped,0,sizeof(m_aRequests[i].overlapped));

		m_aRequests[i].hTimer = NULL;

		// Allocate image buffer
		m_aRequests[i].rvclf.pBufferPtr = new BYTE[IMAGE_BUFFER_SIZE];
		m_aRequests[i].rvclf.dwBufferSize = IMAGE_BUFFER_SIZE;
		ASSERT(m_aRequests[i].rvclf.pBufferPtr != NULL);
	}

	// Create BITMAP
	int nSize = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*256;
	
	m_pBMI = (BITMAPINFO*) new char[nSize];
	ASSERT(m_pBMI);

	m_pBMI->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_pBMI->bmiHeader.biWidth = 0;
	m_pBMI->bmiHeader.biHeight = 0;
	m_pBMI->bmiHeader.biPlanes = 1;
	m_pBMI->bmiHeader.biBitCount = 24;
	m_pBMI->bmiHeader.biCompression = BI_RGB;
	m_pBMI->bmiHeader.biSizeImage   = 0;
	m_pBMI->bmiHeader.biXPelsPerMeter = 0;
	m_pBMI->bmiHeader.biYPelsPerMeter = 0;
	m_pBMI->bmiHeader.biClrUsed	= 0;
	m_pBMI->bmiHeader.biClrImportant = 0;

	// Grayscale palette
	for(i=0; i<256; i++) 
	{
		m_pBMI->bmiColors[i].rgbRed = i ;
		m_pBMI->bmiColors[i].rgbGreen = i ;
		m_pBMI->bmiColors[i].rgbBlue = i ;
		m_pBMI->bmiColors[i].rgbReserved = 0;
	}

	InitializeCriticalSection(&m_cs);
}

/////////////////////////////////////////////////////////////////////////////

CVideoView::~CVideoView()
{
	for(int i=0; i<REQUESTS_NUMBER; i++)
		delete [] m_aRequests[i].rvclf.pBufferPtr;

	delete [] m_pBMI;
	delete m_pControlPanel;
	
	if(m_pFont)
		delete m_pFont;

	DeleteCriticalSection(&m_cs);
}

/////////////////////////////////////////////////////////////////////////////

BOOL CVideoView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CVideoView drawing

void CVideoView::OnDraw(CDC* pDC)
{
	CRect rectClient;
	GetClientRect(&rectClient);
	
	ASSERT(m_nReadyRequest < REQUESTS_NUMBER);
	
	switch(m_aRequests[m_nReadyRequest].dwError)
	{
		case ERROR_SUCCESS:
		{
			// Image Ok
			
			// Top-Down DIB - set negative height
			m_pBMI->bmiHeader.biWidth = m_aRequests[m_nReadyRequest].rvclf.sizeResolution.cx;
			m_pBMI->bmiHeader.biHeight = -m_aRequests[m_nReadyRequest].rvclf.sizeResolution.cy;
			
			switch(m_aRequests[m_nReadyRequest].rvclf.dwColorFormat)
			{
				case RDN_CF_RGB32:	m_pBMI->bmiHeader.biBitCount = 32;
									break;
				case RDN_CF_RGB24:	m_pBMI->bmiHeader.biBitCount = 24;
									break;
				case RDN_CF_RGB15:	m_pBMI->bmiHeader.biBitCount = 16;
									break;
				case RDN_CF_Y8:		m_pBMI->bmiHeader.biBitCount = 8;
									break;

				default:			return;
			}

			// Use StretchDIBits in 1:1 mode
			if(m_aRequests[m_nReadyRequest].rvclf.pBufferPtr && m_pBMI)
			{
				StretchDIBits(
					pDC->GetSafeHdc(),
					0,0,
					m_aRequests[m_nReadyRequest].rvclf.sizeResolution.cx,
					m_aRequests[m_nReadyRequest].rvclf.sizeResolution.cy,
					0,0,
					m_aRequests[m_nReadyRequest].rvclf.sizeResolution.cx,
					m_aRequests[m_nReadyRequest].rvclf.sizeResolution.cy,
					m_aRequests[m_nReadyRequest].rvclf.pBufferPtr,
					m_pBMI,
					DIB_RGB_COLORS,
					SRCCOPY);
			}
			
			return;
		}
		case ERROR_IO_PENDING:
		{
			return;
		}
		default:
		{
			// Error
			CString		str;
			COLORREF	colorText,colorBack;
			CFont*		pOldFont = NULL;

			if(m_aRequests[m_nReadyRequest].dwError == ERROR_NOT_READY)
			{
				// Show "NO SIGNAL" text
				colorText = RGB(0xff,0xff,0xff);
				colorBack = RGB(0,0,0);
				str.LoadString(IDS_NO_SIGNAL);
			}
			else
			{
				// Show Win32 API error description
				colorText = RGB(0xff,0xff,0xff);
				colorBack = RGB(0xff,0,0);
				str = _com_error(HRESULT_FROM_WIN32(m_aRequests[m_nReadyRequest].dwError)).ErrorMessage();
			}

			if(m_pFont)
				pOldFont = pDC->SelectObject(m_pFont);

			CSize size = pDC->GetTextExtent(str);
			CRect rect;

			// The code below is "smart" DrawText
			rect.SetRect(0,0,rectClient.right,(rectClient.bottom-size.cy)/2);
			pDC->FillSolidRect(&rect,colorBack);
			
			rect.SetRect(0,(rectClient.bottom+size.cy)/2,rectClient.right,rectClient.bottom);
			pDC->FillSolidRect(&rect,colorBack);

			rect.SetRect(0,(rectClient.bottom-size.cy)/2,
						 (rectClient.right-size.cx)/2,(rectClient.bottom+size.cy)/2);
			pDC->FillSolidRect(&rect,colorBack);

			rect.SetRect((rectClient.right+size.cx)/2,(rectClient.bottom-size.cy)/2,
						 rectClient.right,(rectClient.bottom+size.cy)/2);
			pDC->FillSolidRect(&rect,colorBack);

			rect.SetRect((rectClient.right-size.cx)/2,(rectClient.bottom-size.cy)/2,
						 (rectClient.right+size.cx)/2,(rectClient.bottom+size.cy)/2);
			
			colorText = pDC->SetTextColor(colorText);
			colorBack = pDC->SetBkColor(colorBack);
			
			int nBkMode = pDC->SetBkMode(OPAQUE);

			pDC->DrawText(str,&rect,DT_TOP|DT_LEFT);

			// Return old GDI settings
			pDC->SetTextColor(colorText);
			pDC->SetBkColor(colorBack);
			pDC->SetBkMode(nBkMode);
			
			if(pOldFont)
				pDC->SelectObject(pOldFont);

			break;
		}
	} // switch
}

/////////////////////////////////////////////////////////////////////////////
// CVideoView diagnostics

#ifdef _DEBUG
void CVideoView::AssertValid() const
{
	CView::AssertValid();
}

void CVideoView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CVideoDoc* CVideoView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVideoDoc)));
	return (CVideoDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CVideoView message handlers

int CVideoView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if(CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// Create contol panel window
	if(!m_pControlPanel->Create(this))
	{
		TRACE(_T("Cannot create control panel window.\n"));
		return -1;
	}

	// Create "stop capture" auto-reset event
	m_hStopCaptureEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	ASSERT(m_hStopCaptureEvent != NULL);

	for(int i=0; i<REQUESTS_NUMBER; i++)
	{
		// Create "overlapped" manual-reset event
		m_aRequests[i].overlapped.hEvent = CreateEvent(NULL,TRUE,TRUE,NULL);
		ASSERT(m_aRequests[i].overlapped.hEvent != NULL);

		// Create synchronization (auto-reset) timer
		m_aRequests[i].hTimer = CreateWaitableTimer(NULL,FALSE,NULL);
		ASSERT(m_aRequests[i].hTimer != NULL);
	}
	
	// Create capture thread (in a suspended state)
	DWORD dwThreadID;
	
	m_hCaptureThread = CreateThread(NULL,0,CaptureThread,this,CREATE_SUSPENDED,&dwThreadID);
	ASSERT(m_hCaptureThread != NULL);

	// Set up FPS timer
	m_nTimer = SetTimer(1,FPS_UPDATE_TIMEOUT,NULL);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////

void CVideoView::OnDestroy() 
{
	// Kill FPS timer
	if(m_nTimer)
		KillTimer(m_nTimer), m_nTimer = 0;
	
	// Stop capture thread
	SetEvent(m_hStopCaptureEvent);
	WaitForSingleObject(m_hCaptureThread,1000);

	CloseHandle(m_hStopCaptureEvent);
	CloseHandle(m_hCaptureThread);
	
	for(int i=0; i<REQUESTS_NUMBER; i++)
	{
		CloseHandle(m_aRequests[i].overlapped.hEvent);
		CloseHandle(m_aRequests[i].hTimer);
	}

	CView::OnDestroy();
}

/////////////////////////////////////////////////////////////////////////////

void CVideoView::OnSize(UINT nType,int cx,int cy) 
{
	if(!cx && !cy)
		return;

	if((cx < m_sizeMin.cx) || (cy < m_sizeMin.cy))
		SetSize(m_sizeMin);

	if((cx > m_sizeMax.cx) || (cy > m_sizeMax.cy))
		SetSize(m_sizeMax);

	// Show current image size
	CMainFrame*	pFrame = static_cast<CMainFrame*>(AfxGetApp()->m_pMainWnd);
	CSize		size;
	
	size.cx = m_sizeAlignment.cx*int(cx/m_sizeAlignment.cx);
	size.cy = m_sizeAlignment.cy*int(cy/m_sizeAlignment.cy);

	if(pFrame && ::IsWindow(pFrame->GetSafeHwnd()))
		pFrame->ShowPreviewSize(size);

	// Change font height
	if(m_pFont)
		delete m_pFont, m_pFont = NULL;
		
	m_pFont = new CFont;
	ASSERT(m_pFont);

	if(!m_pFont->CreateFont(-((cy/14 > 20) ? cy/14 : 20),
							0,0,0,FW_NORMAL,false,false,false,
							RUSSIAN_CHARSET,OUT_DEFAULT_PRECIS,
							CLIP_DEFAULT_PRECIS,PROOF_QUALITY,
							DEFAULT_PITCH|FF_SWISS,
							_T("Arial")))
	{
		delete m_pFont, m_pFont = NULL;
	}

}

/////////////////////////////////////////////////////////////////////////////

void CVideoView::OnWindowFullFrame() 
{
	SetSize(m_sizeMax);
}

/////////////////////////////////////////////////////////////////////////////

void CVideoView::OnWindowCif() 
{
	if(	(m_sizeMax.cx/2 >= m_sizeMin.cx) && 
		(m_sizeMax.cy/2 >= m_sizeMin.cy))
		SetSize(CSize(m_sizeMax.cx/2,m_sizeMax.cy/2));
}

/////////////////////////////////////////////////////////////////////////////

void CVideoView::OnWindowQcif() 
{
	if(	(m_sizeMax.cx/4 >= m_sizeMin.cx) && 
		(m_sizeMax.cy/4 >= m_sizeMin.cy))
		SetSize(CSize(m_sizeMax.cx/4,m_sizeMax.cy/4));
}

/////////////////////////////////////////////////////////////////////////////

// Resizes parent Frame
void CVideoView::SetSize(CSize &size)
{
	CSize		sizeFrame;
	CRect		rectFrame,rectMine;
	CFrameWnd*	pFrame = GetParentFrame();

	ASSERT(pFrame && ::IsWindow(pFrame->GetSafeHwnd()));
	
	pFrame->GetWindowRect(&rectFrame);
	GetClientRect(&rectMine);

	sizeFrame.cx = rectFrame.right - rectFrame.left - rectMine.right;
	sizeFrame.cy = rectFrame.bottom - rectFrame.top - rectMine.bottom;
	
	if((sizeFrame.cx > 0) && (sizeFrame.cy > 0))
	{
		sizeFrame += size;

		pFrame->SetWindowPos(NULL,0,0,sizeFrame.cx,sizeFrame.cy,
							 SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
	}
}

/////////////////////////////////////////////////////////////////////////////

BOOL CVideoView::OnEraseBkgnd(CDC* pDC) 
{
	return true;
	//return CView::OnEraseBkgnd(pDC);
}

/////////////////////////////////////////////////////////////////////////////

void CVideoView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();

	OnWindowCif();
	
	// Resume video capture thread
	ResumeThread(m_hCaptureThread);
}

/////////////////////////////////////////////////////////////////////////////

// Update capture settings
bool CVideoView::Update(int nDevice,DWORD dwColorFormat)
{
	EnterCriticalSection(&m_cs);
	
	m_nDevice = nDevice;
	m_dwColorFormat = dwColorFormat;
	
	SRvclDeviceStatus rvclds;
	
	rvclds.dwSize = sizeof(rvclds);
	rvclds.dwMask =	RVCL_STATUS_RESOLUTION;
	
	if(RvclGetDeviceStatus(nDevice,&rvclds))
	{
		m_sizeMax = rvclds.sizeMaxResolution;
		m_sizeMin = rvclds.sizeMinResolution;
		m_sizeAlignment = rvclds.sizeResolutionAlignment;
	}

	LeaveCriticalSection(&m_cs);

	// Check window size
	CRect rect;
	GetClientRect(&rect);
	
	if((rect.Size().cx < m_sizeMin.cx) || (rect.Size().cy < m_sizeMin.cy))
		SetSize(m_sizeMin);

	if((rect.Size().cx > m_sizeMax.cx) || (rect.Size().cy > m_sizeMax.cy))
		SetSize(m_sizeMax);

	return true;
}

/////////////////////////////////////////////////////////////////////////////

// Video capture thread
DWORD WINAPI CVideoView::CaptureThread(LPVOID pParam)
{
	CVideoView*	pThis = (CVideoView*)pParam;
	
	// Handles to wait:
	// 0 - stop event,
	// from 1 to REQUESTS_NUMBER - OVERLAPPED events,
	// from (REQUESTS_NUMBER+1) to (2*REQUESTS_NUMBER) - waitable timers
	
	HANDLE hEvents[2*REQUESTS_NUMBER+1];

	// Stop event
	hEvents[0] = pThis->m_hStopCaptureEvent;

	for(int i=0; i<REQUESTS_NUMBER; i++)
	{
		// Add OVELAPPED event
		hEvents[i+1] = pThis->m_aRequests[i].overlapped.hEvent;

		// Add waitable timer
		hEvents[i+REQUESTS_NUMBER+1] = pThis->m_aRequests[i].hTimer;

		// Set the state of the OVERLAPPED event object to signaled
		// (to start capture process immediately).
		pThis->m_aRequests[i].dwError = ERROR_SUCCESS;
		SetEvent(pThis->m_aRequests[i].overlapped.hEvent);
	}

	while(true)
	{
		DWORD dwWait = WaitForMultipleObjects(2*REQUESTS_NUMBER+1,hEvents,FALSE,INFINITE);

		if(dwWait == WAIT_OBJECT_0)
		{
			// This is stop event
			break;
		}
		
		// Request index in CVideoView::m_aRequests array
		int nRequest = -1;

		if(dwWait <= (WAIT_OBJECT_0+REQUESTS_NUMBER))
		{
			// This is OVERLAPPED event
			nRequest = dwWait - WAIT_OBJECT_0 - 1;
			SRequest* pRequest = &pThis->m_aRequests[nRequest];
			
			if(pRequest->dwError == ERROR_IO_PENDING)
			{
				ASSERT(HasOverlappedIoCompleted(&pRequest->overlapped));
				
				DWORD dwBytesWritten;
			
				// Get capture result
				if(!RvclGetGrabFrameResult(
					pRequest->nDevice,&pRequest->overlapped,&dwBytesWritten,FALSE))
				{
					pRequest->dwError = GetLastError();
				}
				else
				{
					pThis->PostCaptureFiltering(&pRequest->rvclf);
					pRequest->dwError = ERROR_SUCCESS;
				}

				// Show image or error
				ASSERT(::IsWindow(pThis->GetSafeHwnd()));
				::SendMessage(pThis->GetSafeHwnd(),WM_SHOW_IMAGE,nRequest,0);

				// If capture fails, wait a while
				if(pRequest->dwError != ERROR_SUCCESS)
					Sleep(20);
			}
		}
		else
			if(dwWait <= (WAIT_OBJECT_0+2*REQUESTS_NUMBER))
			{
				// This is waitable timer - operation timeout.
				nRequest = dwWait - REQUESTS_NUMBER - WAIT_OBJECT_0 - 1;
				
				// Cancel all pending operations
				RvclCancelIo(pThis->m_aRequests[nRequest].nDevice);

				// Wait for OVELAPPED event(s)
				continue;
			}


		ASSERT(nRequest != -1);
		SRequest* pRequest = &pThis->m_aRequests[nRequest];

		CRect rect;

		// Get appropriate image size
		pThis->GetClientRect(&rect);

		pRequest->rvclf.dwSize = sizeof(pRequest->rvclf);
		
		// Current flags
		pRequest->rvclf.dwFlags = pThis->m_dwFlags;
		
		EnterCriticalSection(&pThis->m_cs);

		// Current device #
		pRequest->nDevice = pThis->m_nDevice;
		
		// Current color format
		pRequest->rvclf.dwColorFormat = pThis->m_dwColorFormat;

		rect.right -= rect.right % pThis->m_sizeAlignment.cx;
		rect.bottom -= rect.bottom % pThis->m_sizeAlignment.cy;

		if((rect.right > pThis->m_sizeMax.cx))
			rect.right = pThis->m_sizeMax.cx;
		else
			if(rect.right < pThis->m_sizeMin.cx)
				rect.right = pThis->m_sizeMin.cx;

		if(rect.bottom > pThis->m_sizeMax.cy)
			rect.bottom = pThis->m_sizeMax.cy;
		else
			if(rect.bottom < pThis->m_sizeMin.cy)
				rect.bottom = pThis->m_sizeMin.cy;

		LeaveCriticalSection(&pThis->m_cs);
				
		pRequest->rvclf.sizeResolution.cx = rect.right;
		pRequest->rvclf.sizeResolution.cy = rect.bottom;

		// Start grab operation
		if(RvclGrabFrame(pRequest->nDevice,&pRequest->rvclf,&pRequest->overlapped))
		{
			pRequest->dwError = ERROR_SUCCESS;
						
			ASSERT(::IsWindow(pThis->GetSafeHwnd()));
			::SendMessage(pThis->GetSafeHwnd(),WM_SHOW_IMAGE,nRequest,0);

			SetEvent(pRequest->overlapped.hEvent);
		}
		else
		{
			pRequest->dwError = GetLastError();

			if(pRequest->dwError == ERROR_IO_PENDING)
			{
				// Pending operation - set up waitable timer.
				// Frame capture cannot be longer than 1 second.
				LARGE_INTEGER nDueTime;
				nDueTime.QuadPart = Int32x32To64(-10000,1000);
				
				SetWaitableTimer(pRequest->hTimer,&nDueTime,0,NULL,NULL,FALSE);
				continue;
			}
			
			// Other error
			ASSERT(::IsWindow(pThis->GetSafeHwnd()));
			::SendMessage(pThis->GetSafeHwnd(),WM_SHOW_IMAGE,nRequest,0);
			
			SetEvent(pRequest->overlapped.hEvent);
			Sleep(20);
		}
	}	

	// Cancel all pending requests
	for(i=0; i<REQUESTS_NUMBER; i++)
		if(pThis->m_aRequests[i].dwError == ERROR_IO_PENDING)
		{
			RvclCancelIo(pThis->m_aRequests[i].nDevice);
			CancelWaitableTimer(pThis->m_aRequests[i].hTimer);

			WaitForSingleObject(pThis->m_aRequests[i].overlapped.hEvent,1000);
		}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////

LONG CVideoView::OnShowImage(WPARAM wParam,LPARAM lParam)
{
	ASSERT(wParam < REQUESTS_NUMBER);

	m_nReadyRequest = wParam;
	Invalidate(false);
	UpdateWindow();

	if(m_aRequests[m_nReadyRequest].dwError == ERROR_SUCCESS)
		m_dwFrameCount++;

	return 0;
}

/////////////////////////////////////////////////////////////////////////////

void CVideoView::OnTimer(UINT nIDEvent) 
{
	CMainFrame*	pFrame = static_cast<CMainFrame*>(AfxGetApp()->m_pMainWnd);

	static DWORD dwLastTickCount = GetTickCount() - FPS_UPDATE_TIMEOUT;
	static DWORD dwLastFrameCount = 0;

	DWORD	dwTickCount = GetTickCount();
	double	dFps;
	UINT	nFps;

	// Calculate FPS
	dFps =	1000.0 * double(m_dwFrameCount - dwLastFrameCount) /
			double(dwTickCount - dwLastTickCount);

	if((ceil(dFps)-dFps) < (dFps-floor(dFps)))
		nFps = UINT(ceil(dFps));
	else
		nFps = UINT(floor(dFps));

	if(pFrame && ::IsWindow(pFrame->GetSafeHwnd()))
		pFrame->ShowFps(nFps);

	dwLastTickCount = dwTickCount;
	dwLastFrameCount = m_dwFrameCount;
	
	CView::OnTimer(nIDEvent);
}

/////////////////////////////////////////////////////////////////////////////

void CVideoView::OnImageTopDown() 
{
	m_dwFlags &= ~RVCL_GRAB_BOTTOM_UP;
}

/////////////////////////////////////////////////////////////////////////////

void CVideoView::OnImageBottomUp() 
{
	m_dwFlags |= RVCL_GRAB_BOTTOM_UP;
}

/////////////////////////////////////////////////////////////////////////////

void CVideoView::OnUpdateImageTopDown(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(!(m_dwFlags & RVCL_GRAB_BOTTOM_UP));	
}

/////////////////////////////////////////////////////////////////////////////

void CVideoView::OnUpdateImageBottomUp(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(m_dwFlags & RVCL_GRAB_BOTTOM_UP);	
}

/////////////////////////////////////////////////////////////////////////////

void CVideoView::OnWindowDeinterlacingfilter()
{
	m_fDeinterlacing = !m_fDeinterlacing;
}
/////////////////////////////////////////////////////////////////////////////
void CVideoView::OnUpdateWindowDeinterlacingfilter(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_fDeinterlacing);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CVideoView::PostCaptureFiltering(SRvclFrame* rvclf)
{
    if(m_fDeinterlacing)
	{
		/*
		*  Only two fields have interlacing effect.
		*/
		if((m_sizeMax.cy / 2) < rvclf->sizeResolution.cy)
		{
			BOOL fRet = RvclDeinterlaceFilter(rvclf->pBufferPtr, rvclf->sizeResolution,
				rvclf->dwColorFormat);
			if(!fRet)
			{
				switch(GetLastError())
				{
				case ERROR_DLL_INIT_FAILED:
					AfxMessageBox(_T("RvclInitializeEx function was not called!"), MB_ICONSTOP);
					break;
				case ERROR_ACCESS_DENIED:
					AfxMessageBox(_T("You have no access to RvclDeinterlaceFilter!\n You should have a right hasp licence."), MB_ICONSTOP);
					break;
				case ERROR_NOT_SUPPORTED:
					AfxMessageBox(_T("RvclDeinterlaceFilter: This color format is not supported."), MB_ICONSTOP);
					break;
				case ERROR_INVALID_PARAMETER:
					AfxMessageBox(_T("RvclDeinterlaceFilter: Invalid parameter"), MB_ICONSTOP);
					break;
				default:
					AfxMessageBox(_T("RvclDeinterlaceFilter: Error!"), MB_ICONSTOP);
					break;
				}
				m_fDeinterlacing = FALSE;
			}
			
		}
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////