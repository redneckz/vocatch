// MultiScreenView.cpp : implementation of the CMultiScreenView class
//

#include "stdafx.h"
#include "MultiScreen.h"
#include "MultiScreenView.h"
#include <math.h>

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMultiScreenView

#define ID_VIEW_CF_FIRST	ID_VIEW_CF_RGB32
#define ID_VIEW_CF_LAST		ID_VIEW_CF_Y8

#define ID_VIEW_VF_FIRST	ID_VIEW_VF_NTSCM
#define ID_VIEW_VF_LAST		ID_VIEW_VF_SECAM

#define ID_VIEW_CSM_FIRST	ID_VIEW_CSM_WHOLEFRAME
#define ID_VIEW_CSM_LAST	ID_VIEW_CSM_FITTOWND

#define RDN_CAPTURE_SIZE_FITTOWND		1
#define RDN_CAPTURE_SIZE_WHOLEFRAME		2
#define RDN_CAPTURE_SIZE_WHOLEFIELD		3
#define RDN_CAPTURE_SIZE_CIF			4

// FPS update period (ms)
#define FPS_UPDATE_TIMEOUT		2000

CMultiScreenView::SFormat CMultiScreenView::s_arrCaptureSizeModes[] = 
{
	{ RDN_CAPTURE_SIZE_FITTOWND,		ID_VIEW_CSM_FITTOWND},
	{ RDN_CAPTURE_SIZE_WHOLEFRAME,		ID_VIEW_CSM_WHOLEFRAME},
	{ RDN_CAPTURE_SIZE_WHOLEFIELD,		ID_VIEW_CSM_WHOLEFIELD},
	{ RDN_CAPTURE_SIZE_CIF,				ID_VIEW_CSM_CIF},
};

CMultiScreenView::SFormat CMultiScreenView::s_arrColorFormats[] =
{
	{ RDN_CF_RGB32,						ID_VIEW_CF_RGB32 },
	{ RDN_CF_RGB24,						ID_VIEW_CF_RGB24 },
	{ RDN_CF_RGB15,						ID_VIEW_CF_RGB15 },
	{ RDN_CF_YUV422_PACKED,				ID_VIEW_CF_YUV422_PACKED },
	{ RDN_CF_YUV411_PACKED,				ID_VIEW_CF_YUV411_PACKED },
	{ RDN_CF_YUV422_PLANAR,				ID_VIEW_CF_YUV422_PLANAR },
	{ RDN_CF_YUV411_PLANAR,				ID_VIEW_CF_YUV411_PLANAR },
	{ RDN_CF_Y8,						ID_VIEW_CF_Y8 }
};

CMultiScreenView::SFormat CMultiScreenView::s_arrVideoFormats[] =
{
	{ RVCL_VF_NTSCM,					ID_VIEW_VF_NTSCM },
	{ RVCL_VF_NTSCJ,					ID_VIEW_VF_NTSCJ },
	{ RVCL_VF_PALBDGHI,					ID_VIEW_VF_PALBDGHI },
	{ RVCL_VF_PALM,						ID_VIEW_VF_PALM },
	{ RVCL_VF_PALN,						ID_VIEW_VF_PALN },
	{ RVCL_VF_SECAM,					ID_VIEW_VF_SECAM }
};

CMultiScreenView::CMultiScreenView()
{
	m_heStop = 0;
	m_heNewImage = 0;
	m_htDrawThread = 0;
	InitializeCriticalSection(&m_csReadyImages);

	m_dwVideoFormat = RVCL_VF_PALBDGHI;
	m_dwColorFormat = RDN_CF_RGB24;
	m_dwCaptureSizeMode = RDN_CAPTURE_SIZE_FITTOWND;

	m_sizeMinResolution = CSize(48,36);
	m_sizeMaxResolution = CSize(768,576);
	m_sizeAlignment = CSize(16,1);

	m_sizeChannelRect = CSize(0, 0);
	m_sizeImageRect = m_sizeChannelRect;
	m_sizeRowColCount = CSize(0, 0);

	m_dwFlags = RVCL_GRAB_FIELD_EVEN;
	
	m_fMultiplexingMode = false;
	m_f43Ratio = true;
	m_fShowVideo = true;
	
	m_pBMI = 0;
	m_pFont = 0;
	m_pbYuvBuffer = 0;
	m_nFpsTimer = 0;

	InitializeCriticalSection(&m_csChannelsFps);
}

CMultiScreenView::~CMultiScreenView()
{
	DeleteCriticalSection(&m_csReadyImages);
	DeleteCriticalSection(&m_csChannelsFps);
	delete [] m_pbYuvBuffer;
}


BEGIN_MESSAGE_MAP(CMultiScreenView,CWnd )
	//{{AFX_MSG_MAP(CMultiScreenView)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_COMMAND_RANGE(ID_VIEW_CF_FIRST, ID_VIEW_CF_LAST, OnViewColorFormat)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_CF_FIRST, ID_VIEW_CF_LAST, OnUpdateViewColorFormat)
	ON_COMMAND_RANGE(ID_VIEW_VF_FIRST, ID_VIEW_VF_LAST, OnViewVideoFormat)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_VF_FIRST, ID_VIEW_VF_LAST, OnUpdateViewVideoFormat)
	ON_COMMAND_RANGE(ID_VIEW_CSM_FIRST, ID_VIEW_CSM_LAST, OnViewCaptureSizeMode)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_CSM_FIRST, ID_VIEW_CSM_LAST, OnUpdateViewCaptureSizeMode)
	ON_COMMAND(ID_VIEW_MULTIPLEXING, OnViewMultiplexing)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MULTIPLEXING, OnUpdateViewMultiplexing)
	ON_COMMAND(ID_VIEW_43RATIO, OnView43Ratio)
	ON_UPDATE_COMMAND_UI(ID_VIEW_43RATIO, OnUpdateView43Ratio)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_COMMAND(ID_VIEW_SHOWVIDEO, OnViewShowVideo)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHOWVIDEO, OnUpdateViewShowVideo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMultiScreenView message handlers

BOOL CMultiScreenView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if(!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), HBRUSH(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CMultiScreenView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CWnd::OnPaint() for painting messages
}

void CMultiScreenView::DrawImage(CImage* pImage)
{
	ASSERT(pImage);

	EnterCriticalSection(&m_csReadyImages);

	m_lstReadyImages.AddTail(pImage);
	SetEvent(m_heNewImage);

	LeaveCriticalSection(&m_csReadyImages);
}

// static 
DWORD WINAPI CMultiScreenView::DrawThread(LPVOID lpParameter)
{
	ASSERT(lpParameter);
	return static_cast<CMultiScreenView*>(lpParameter)->DrawEngine();
}

DWORD CMultiScreenView::DrawEngine()
{
	HANDLE	hHandles[] = {m_heStop,m_heNewImage};
	DWORD	dwWait;
	CImage*	pImage;

	while(true)
	{
		dwWait = WaitForMultipleObjects(sizeof(hHandles)/sizeof(HANDLE),hHandles,false,INFINITE);

		if(dwWait == WAIT_OBJECT_0)
			break;

		while(true)
		{
			EnterCriticalSection(&m_csReadyImages);

			if(m_lstReadyImages.IsEmpty())
			{
				LeaveCriticalSection(&m_csReadyImages);
				break;
			}

			pImage = m_lstReadyImages.RemoveHead();
			LeaveCriticalSection(&m_csReadyImages);

			InternalDrawImage(pImage);
		}
	}

	return 0;
}
	
void CMultiScreenView::InternalDrawImage(CImage* pImage)
{
	ASSERT(pImage);
	
	CRect rectClient, rectChannel;
	
	GetClientRect(&rectClient);

	int			nRelativeWidth = 1;
	int			nRelativeHeight = 1;
	bool		fVUnder	= false;
	unsigned	nChannel = 0;
	
	if(m_fMultiplexingMode)
		nChannel = pImage->m_nDevice * 4 + pImage->m_rvclf.dwVideoSource;
	else
		nChannel = pImage->m_nDevice;

	GetChannelRect(nChannel, &rectChannel);

	CDC* pDC = GetDC();

	if(!pDC)
		return;

	switch(pImage->m_dwError)
	{
		case ERROR_SUCCESS:
		{
			if(m_fShowVideo)
			{
				ASSERT(pImage->m_rvclf.pBufferPtr);
				ASSERT(m_pBMI);

				// Top-Down DIB - set negative height
				m_pBMI->bmiHeader.biWidth = pImage->m_rvclf.sizeResolution.cx;
				m_pBMI->bmiHeader.biHeight = -pImage->m_rvclf.sizeResolution.cy;

				bool	fShowChroma = false;
				int		nChromaWidth,nChromaHeight;
				LPBYTE	pbImageBuffer = (LPBYTE)pImage->m_rvclf.pBufferPtr;

				switch(pImage->m_rvclf.dwColorFormat)
				{
					case RDN_CF_YUV422_PACKED:
							// Convert into YUV 4:2:2 planar 
							Packed422ToPlanar(pbImageBuffer,m_pbYuvBuffer,pImage->m_rvclf.sizeResolution);
							pbImageBuffer = m_pbYuvBuffer;

					case RDN_CF_YUV422_PLANAR:
							m_pBMI->bmiHeader.biBitCount = 8;
							fShowChroma = true;
							nChromaWidth = pImage->m_rvclf.sizeResolution.cx/2;
							nChromaHeight = pImage->m_rvclf.sizeResolution.cy;
							//rectChannel.right = rectChannel.left + 
							//	rectChannel.Width() / 2;
							nRelativeWidth = 2;
							nRelativeHeight = 1;
							break;

					case RDN_CF_YUV411_PACKED:
							// Convert into YUV 4:1:1 planar 
							Packed411ToPlanar(pbImageBuffer,m_pbYuvBuffer,pImage->m_rvclf.sizeResolution);
							pbImageBuffer = m_pbYuvBuffer;

					case RDN_CF_YUV411_PLANAR:
							m_pBMI->bmiHeader.biBitCount = 8;
							fShowChroma = true;
							nChromaWidth = pImage->m_rvclf.sizeResolution.cx/4;
							nChromaHeight = pImage->m_rvclf.sizeResolution.cy;
							//rectChannel.right = rectChannel.left + 
							//	rectChannel.Width() / 3 * 2;
							nRelativeWidth = 2;
							nRelativeHeight = 1;
							break;

					case RDN_CF_RGB32:	
							m_pBMI->bmiHeader.biBitCount = 32;
							break;

					case RDN_CF_RGB24:
							m_pBMI->bmiHeader.biBitCount = 24;
							break;

					case RDN_CF_RGB15:
							m_pBMI->bmiHeader.biBitCount = 16;	// 15 is GDI's 16
							break;

					case RDN_CF_Y8:
							m_pBMI->bmiHeader.biBitCount = 8;
							break;

					default:
							ASSERT(false);
				}

				m_pBMI->bmiHeader.biWidth = pImage->m_rvclf.sizeResolution.cx;
				m_pBMI->bmiHeader.biHeight = -(int)pImage->m_rvclf.sizeResolution.cy;

				SetStretchBltMode(pDC->GetSafeHdc(), COLORONCOLOR);//HALFTONE);
			  				
				StretchDIBits(
					pDC->GetSafeHdc(),
					rectChannel.left,
					rectChannel.top,
					m_sizeImageRect.cx,
					m_sizeImageRect.cy,
					0,
					0,
					pImage->m_rvclf.sizeResolution.cx,
					pImage->m_rvclf.sizeResolution.cy,
					pbImageBuffer,
					m_pBMI,
					DIB_RGB_COLORS,
					SRCCOPY);


				if(fShowChroma)
				{
					LPBYTE pChromaData = 
							pbImageBuffer + 
							pImage->m_rvclf.sizeResolution.cx * pImage->m_rvclf.sizeResolution.cy;

					// U
					m_pBMI->bmiHeader.biWidth = nChromaWidth;
					m_pBMI->bmiHeader.biHeight = -(int)nChromaHeight;
					
					CRect rect = rectChannel;
					rect.right -= m_sizeImageRect.cx;
					rect.OffsetRect(m_sizeImageRect.cx, 0);

					StretchDIBits(
						pDC->GetSafeHdc(),
						rect.left,
						rect.top,
						rect.Width() / nRelativeWidth,
						rect.Height() / nRelativeHeight,
						0,
						0,
						nChromaWidth,
						nChromaHeight,
						pChromaData,
						(LPBITMAPINFO)m_pBMI,
						DIB_RGB_COLORS,
						SRCCOPY);

					pChromaData += nChromaWidth * nChromaHeight;

					// V

					if(fVUnder)
						rect.OffsetRect(0, rect.Height() / nRelativeHeight);
					else
						rect.OffsetRect(rect.Width() / nRelativeWidth, 0);

					StretchDIBits(
						pDC->GetSafeHdc(),
						rect.left,
						rect.top,
						rect.Width() / nRelativeWidth,
						rect.Height() / nRelativeHeight,
						0,
						0,
						nChromaWidth,
						nChromaHeight,
						pChromaData, 
						(LPBITMAPINFO)m_pBMI,
						DIB_RGB_COLORS,
						SRCCOPY);
				}
			}
			else
			{
				pDC->FillSolidRect(&rectChannel, RGB(0,0,0));
				pDC->Draw3dRect(&rectChannel, RGB(0xff,0xff,0xff), RGB(0xff,0xff,0xff));
			}
			
			CString		strOutText;
			unsigned	nFps;
			bool		fShowFps;
			DWORD		dwTextColor;

			EnterCriticalSection(&m_csChannelsFps);
			
			// Update frames count & read current FPS
			m_arrChannels[nChannel]->m_nFramesCount++;
			nFps = m_arrChannels[nChannel]->m_nFps;
			fShowFps = (m_arrChannels[nChannel]->m_nLastFramesCount != 0);
			
			LeaveCriticalSection(&m_csChannelsFps);

			strOutText.Format(
				_T("Device %u\nInput %u"),
				pImage->m_nDevice, pImage->m_rvclf.dwVideoSource);

			// Show text using green color
			dwTextColor = RGB(0, 0x9f, 0);

			if(fShowFps)
			{
				CString str;
				str.Format(_T("\n%u FPS"), nFps);
				strOutText += str;

				// Show reduced FPS in real-time mode
				// usinf RED color
				if(!m_fMultiplexingMode && (nFps < GetExpectedFps()))
					dwTextColor = RGB(0xff, 0, 0);
			}

			CRect rectTextOut = rectChannel;
			rectTextOut.DeflateRect(5, 5, 5, 5);

			pDC->SetTextColor(dwTextColor);
			pDC->SetBkMode(TRANSPARENT);
			
			pDC->DrawText(strOutText, &rectTextOut, 0);

			break;
		}
		case ERROR_IO_PENDING:
		{
			break;
		}
		default:
		{
			// Error
			CString		str;
			COLORREF	colorText,colorBack;
			CFont*		pOldFont = NULL;

			if(pImage->m_dwError == ERROR_NOT_READY)
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
				str = _com_error(HRESULT_FROM_WIN32(pImage->m_dwError)).ErrorMessage();
			}

			if(m_pFont)
				pOldFont = pDC->SelectObject(m_pFont);

			CSize size = pDC->GetTextExtent(str);
			CRect rect;

			// The code below is "smart" DrawText
			//pDC->FillSolidRect(&rectChannel, colorBack);
			pDC->FillSolidRect(&rectChannel, colorBack);
			pDC->Draw3dRect(&rectChannel, colorText, colorText);

			colorText = pDC->SetTextColor(colorText);
			colorBack = pDC->SetBkColor(colorBack);
			
			int nBkMode = pDC->SetBkMode(OPAQUE);
			
			pDC->DrawText(str, &rectChannel, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

			// Return old GDI settings
			pDC->SetTextColor(colorText);
			pDC->SetBkColor(colorBack);
			pDC->SetBkMode(nBkMode);
			
			if(pOldFont)
				pDC->SelectObject(pOldFont);

			break;
		}
	} // switch

	ReleaseDC(pDC);
	SetEvent(pImage->m_heReady);
}

int CMultiScreenView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if(CWnd ::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Create BITMAP
	int nSize = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*256;
	
	m_pBMI = (BITMAPINFO*) new BYTE[nSize];
	ASSERT(m_pBMI);

	m_pBMI->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_pBMI->bmiHeader.biWidth = 0;
	m_pBMI->bmiHeader.biHeight = 0;
	m_pBMI->bmiHeader.biPlanes = 1;
	m_pBMI->bmiHeader.biBitCount = 0;
	m_pBMI->bmiHeader.biCompression = BI_RGB;
	m_pBMI->bmiHeader.biSizeImage   = 0;
	m_pBMI->bmiHeader.biXPelsPerMeter = 0;
	m_pBMI->bmiHeader.biYPelsPerMeter = 0;
	m_pBMI->bmiHeader.biClrUsed	= 0;
	m_pBMI->bmiHeader.biClrImportant = 0;

	// Grayscale palette
	for(int i=0; i<256; i++) 
	{
		m_pBMI->bmiColors[i].rgbRed = i ;
		m_pBMI->bmiColors[i].rgbGreen = i ;
		m_pBMI->bmiColors[i].rgbBlue = i ;
		m_pBMI->bmiColors[i].rgbReserved = 0;
	}

	// Allocate temporary YUV buffer for packed YUV images
	const unsigned nMaxLumaBufferSize = 768*576;

	m_pbYuvBuffer = new BYTE[nMaxLumaBufferSize * 2];
	ASSERT(m_pbYuvBuffer);


	// Create "stop" manual-reset event
	m_heStop = CreateEvent(NULL,true,false,NULL);
	ASSERT(m_heStop != NULL);

	// Create "new image" auto-reset event
	m_heNewImage = CreateEvent(NULL,false,false,NULL);
	ASSERT(m_heNewImage != NULL);

	// Create capture thread (in a suspended state)
	m_htDrawThread = CreateThread(NULL,0,DrawThread,this,0,NULL);
	ASSERT(m_htDrawThread != NULL);	
	
	// Create images
	int nDevicesCount;

	RvclGetDevicesCount(&nDevicesCount);

	for(i=0; i<nDevicesCount; i++)
	{
		CCaptureDevice* pDevice = new CCaptureDevice(i,this);
		ASSERT(pDevice);

		pDevice->Create();
		pDevice->SetMultiplexingMode(m_fMultiplexingMode);

		m_arrDevices.Add(pDevice);

		for(int j=0; j<4; j++)
			m_arrChannels.Add(new CChannel);
	}

	CalcRowColCount();

	for(i=0; i<m_arrDevices.GetSize(); i++)
	{
		m_arrDevices[i]->Start();
	}

	// Set up FPS timer
	m_nFpsTimer = SetTimer(1,FPS_UPDATE_TIMEOUT,NULL);

	return 0;
}

void CMultiScreenView::OnDestroy() 
{
	ASSERT(m_htDrawThread);
	ASSERT(m_heStop);
	ASSERT(m_heNewImage);

	// Kill FPS timer
	if(m_nFpsTimer)
		KillTimer(m_nFpsTimer), m_nFpsTimer = 0;

	for(int i=0; i<m_arrDevices.GetSize(); i++)
	{
		m_arrDevices[i]->Stop();
	}

	SetEvent(m_heStop);
	WaitForSingleObject(m_htDrawThread,30000);

	CloseHandle(m_heStop), m_heStop = 0;
	CloseHandle(m_htDrawThread), m_htDrawThread = 0;

	m_lstReadyImages.RemoveAll();
	CloseHandle(m_heNewImage), m_heNewImage = 0;

	for(i=0; i<m_arrDevices.GetSize(); i++)
	{
		m_arrDevices[i]->Destroy();
		delete m_arrDevices[i];
	}

	m_arrDevices.RemoveAll();

	for(i=0; i<m_arrChannels.GetSize(); i++)
	{
		delete m_arrChannels[i];
	}

	m_arrChannels.RemoveAll();
	
	delete m_pBMI, m_pBMI = 0;
	delete m_pFont, m_pFont = 0;

	CWnd ::OnDestroy();
}

BOOL CMultiScreenView::OnEraseBkgnd(CDC* pDC) 
{
	int		nCountImages = 0;
	CRect	rectImage;

	if(m_fMultiplexingMode)
		nCountImages = m_arrDevices.GetSize() * 4;
	else
		nCountImages = m_arrDevices.GetSize();

	for(int i=0; i<nCountImages; i++)
	{
		GetChannelRect( i, &rectImage);
		pDC->ExcludeClipRect(rectImage);
	}

	return CWnd::OnEraseBkgnd(pDC);
}

void CMultiScreenView::OnViewCaptureSizeMode(UINT nID)
{
	for(int i=0; i<sizeof(s_arrCaptureSizeModes)/sizeof(s_arrCaptureSizeModes[0]); i++)
	{
		if(s_arrCaptureSizeModes[i].nResourceID == nID)
		{
			m_dwCaptureSizeMode = s_arrCaptureSizeModes[i].dwRvclFormat;
			break;
		}
	}
	
	SetCaptureImageResolution();
}

void CMultiScreenView::OnUpdateViewCaptureSizeMode(CCmdUI* pCmdUI)
{
	for(int i=0; i<sizeof(s_arrCaptureSizeModes)/sizeof(s_arrCaptureSizeModes[0]); i++)
	{
		if(s_arrCaptureSizeModes[i].nResourceID == pCmdUI->m_nID)
		{
			pCmdUI->SetRadio(m_dwCaptureSizeMode == s_arrCaptureSizeModes[i].dwRvclFormat);
			break;
		}
	}
}

void CMultiScreenView::OnViewColorFormat(UINT nID) 
{
	for(int i=0; i<sizeof(s_arrColorFormats)/sizeof(s_arrColorFormats[0]); i++)
	{
		if(s_arrColorFormats[i].nResourceID == nID)
		{
			for(int j=0; j<m_arrDevices.GetSize(); j++)
			{
				m_arrDevices[j]->SetColorFormat(s_arrColorFormats[i].dwRvclFormat);
			}

			break;
		}
	}

	m_dwColorFormat = s_arrColorFormats[i].dwRvclFormat;

	CalcChannelSize();
	SetCaptureImageResolution();
	Invalidate();
}

void CMultiScreenView::OnUpdateViewColorFormat(CCmdUI* pCmdUI) 
{
	for(int i=0; i<sizeof(s_arrColorFormats)/sizeof(s_arrColorFormats[0]); i++)
	{
		if(s_arrColorFormats[i].nResourceID == pCmdUI->m_nID)
		{
			pCmdUI->SetRadio(m_dwColorFormat == s_arrColorFormats[i].dwRvclFormat);
			break;
		}
	}
}

void CMultiScreenView::OnViewVideoFormat(UINT nID) 
{
	for(int i=0; i<sizeof(s_arrVideoFormats)/sizeof(s_arrVideoFormats[0]); i++)
	{
		if(s_arrVideoFormats[i].nResourceID == nID)
		{
			for(int j=0; j<m_arrDevices.GetSize(); j++)
			{
				m_arrDevices[j]->SetVideoFormat(s_arrVideoFormats[i].dwRvclFormat);
			}

			break;
		}
	}

	m_dwVideoFormat = s_arrVideoFormats[i].dwRvclFormat;

	CalcChannelSize();
	SetCaptureImageResolution();
	Invalidate();
}

void CMultiScreenView::OnUpdateViewVideoFormat(CCmdUI* pCmdUI) 
{
	for(int i=0; i<sizeof(s_arrVideoFormats)/sizeof(s_arrVideoFormats[0]); i++)
	{
		if(s_arrVideoFormats[i].nResourceID == pCmdUI->m_nID)
		{
			pCmdUI->SetRadio(m_dwVideoFormat == s_arrVideoFormats[i].dwRvclFormat);
			break;
		}
	}
}

// Converts YUV 4:2:2 packed image into YUV 4:2:2 planar
void CMultiScreenView::Packed422ToPlanar(const BYTE* pbPackedImage,BYTE* pbPlanarImage,SIZE sizeResolution) const
{
	int		i,
			nLumaBufferSize = sizeResolution.cx * sizeResolution.cy,
			nQuads = nLumaBufferSize * 2 / 4;

	LPBYTE	pbBufferY = pbPlanarImage,
			pbBufferU = pbBufferY + nLumaBufferSize,
			pbBufferV = pbBufferU + nLumaBufferSize/2;

	for(i=0; i<nQuads; i++)
	{
		*pbBufferY++ = *pbPackedImage++;
		*pbBufferU++ = *pbPackedImage++;
		*pbBufferY++ = *pbPackedImage++;
		*pbBufferV++ = *pbPackedImage++;
	}
}

// Converts YUV 4:1:1 packed image into YUV 4:1:1 planar
void CMultiScreenView::Packed411ToPlanar(const BYTE* pbPackedImage,BYTE* pbPlanarImage,SIZE sizeResolution) const
{
	int		i,
			nLumaBufferSize = sizeResolution.cx * sizeResolution.cy,
			nDozens = nLumaBufferSize * 3 / 24;

	LPBYTE	pbBufferY = pbPlanarImage,
			pbBufferU = pbBufferY + nLumaBufferSize,
			pbBufferV = pbBufferU + nLumaBufferSize/4;

	for(i=0; i<nDozens; i++)
	{
		*pbBufferU++ = *pbPackedImage++;
		*pbBufferY++ = *pbPackedImage++;
		*pbBufferV++ = *pbPackedImage++;
		*pbBufferY++ = *pbPackedImage++;
		
		*pbBufferU++ = *pbPackedImage++;
		*pbBufferY++ = *pbPackedImage++;
		*pbBufferV++ = *pbPackedImage++;
		*pbBufferY++ = *pbPackedImage++;
		
		*pbBufferY++ = *pbPackedImage++;
		*pbBufferY++ = *pbPackedImage++;
		*pbBufferY++ = *pbPackedImage++;
		*pbBufferY++ = *pbPackedImage++;
	}
}

void CMultiScreenView::OnView43Ratio()
{
	m_f43Ratio = !m_f43Ratio;

	CalcChannelSize();
	SetCaptureImageResolution();
	Invalidate();
}

void CMultiScreenView::OnUpdateView43Ratio(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_f43Ratio ? 1 : 0);
}


void CMultiScreenView::OnViewMultiplexing() 
{
	m_fMultiplexingMode = !m_fMultiplexingMode;
	
	CalcRowColCount();

	for(int i=0; i<m_arrDevices.GetSize(); i++)
		m_arrDevices[i]->SetMultiplexingMode(m_fMultiplexingMode);

	EnterCriticalSection(&m_csChannelsFps);

	for(i=0; i<m_arrChannels.GetSize(); i++)
	{
		CChannel* pChannel = m_arrChannels[i];
		pChannel->m_nFps = 0;
		pChannel->m_nFramesCount = 0;
		pChannel->m_nLastFramesCount = 0;
	}

	LeaveCriticalSection(&m_csChannelsFps);

	CalcChannelSize();
	SetCaptureImageResolution();
	Invalidate();
}

void CMultiScreenView::OnUpdateViewMultiplexing(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_fMultiplexingMode ? 1 : 0);
}

void CMultiScreenView::CalcChannelSize()
{
	CRect rectClient;
	GetClientRect(&rectClient);
	
	int		nRelativeWidthDef = 4,
			nRelativeWidth = 4,
			nRelativeHeight = 3,
			nWidthAlignment = 4;
	bool	fShowChroma	= false;

	switch(m_dwColorFormat)
	{
		case RDN_CF_YUV422_PACKED:
		case RDN_CF_YUV422_PLANAR:
			nRelativeWidth = 8;
			nWidthAlignment = 16;
			fShowChroma = true;
			break;
			
		case RDN_CF_YUV411_PACKED:
		case RDN_CF_YUV411_PLANAR:
			nRelativeWidth = 6;
			nWidthAlignment = 16;
			fShowChroma = true;
			break;
			
		case RDN_CF_RGB32:	
		case RDN_CF_RGB24:
		case RDN_CF_RGB16:
		case RDN_CF_RGB15:
		case RDN_CF_Y8:
			nWidthAlignment = 4;
			break;
		
		default:
			ASSERT(false);
		
	}

	int nCX = rectClient.Width() / m_sizeRowColCount.cx;
	int nCY = rectClient.Height() / m_sizeRowColCount.cy;

	nCX -= nCX % nWidthAlignment;

	if(!m_f43Ratio)
	{
		m_sizeChannelRect.cx = nCX;
		m_sizeChannelRect.cy = nCY;
		
		m_sizeImageRect = m_sizeChannelRect;

		if(fShowChroma)
		{
			m_sizeImageRect.cx = 
				m_sizeChannelRect.cx * nRelativeWidthDef / nRelativeWidth;

			m_sizeImageRect.cx -= 
				m_sizeImageRect.cx % nWidthAlignment;
		}

	}
	else
	{				
		if((nCX * nRelativeHeight / nRelativeWidth) < nCY)
		{
			m_sizeChannelRect.cx = nCX;
		}
		else
		{
			m_sizeChannelRect.cx = nCY * nRelativeWidth / nRelativeHeight;
			m_sizeChannelRect.cx -= m_sizeChannelRect.cx % nWidthAlignment;
		}
		
		if(fShowChroma)
		{
			m_sizeImageRect.cx = 
				m_sizeChannelRect.cx * nRelativeWidthDef / nRelativeWidth;
		}
		else
		{
			m_sizeImageRect.cx = 
				m_sizeChannelRect.cx;
		}

		m_sizeImageRect.cx -= m_sizeImageRect.cx % nWidthAlignment;
		m_sizeChannelRect.cy = m_sizeImageRect.cx * nRelativeHeight / nRelativeWidthDef;

		m_sizeImageRect.cy = m_sizeChannelRect.cy;
	}
}

void CMultiScreenView::CalcRowColCount()
{
	double dblDeviceCount = (double)m_arrDevices.GetSize();

	if(m_fMultiplexingMode)
		dblDeviceCount *= 4.0;

	double dblX = ((double)4.0) * sqrt( dblDeviceCount / (double)12 );
	double dblY = ((double)3.0) * sqrt( dblDeviceCount / (double)12 );
	
	double dblFractionX, dblFractionY, dblInteger;
	
	dblFractionX = modf(dblX, &dblInteger);	
	m_sizeRowColCount.cx = DWORD(dblFractionX > 0.5 ? dblInteger+=1.0 : dblInteger);
	
	dblFractionY = modf(dblY, &dblInteger);	
	m_sizeRowColCount.cy = DWORD(dblFractionY > 0.5 ? dblInteger+=1.0 : dblInteger);

	m_sizeRowColCount.cy += 
		m_sizeRowColCount.cx * m_sizeRowColCount.cy < dblDeviceCount ? 1 : 0;

	if(m_sizeRowColCount.cx * m_sizeRowColCount.cy - dblDeviceCount - 1 >= 
		m_sizeRowColCount.cy - 1)
		m_sizeRowColCount.cx--;

	CalcChannelSize();
	SetCaptureImageResolution();
}

void CMultiScreenView::GetChannelRect(DWORD dwIndex, CRect *prectImage)
{
	ASSERT(prectImage);

	prectImage->SetRectEmpty();

	prectImage->right = m_sizeChannelRect.cx;
	prectImage->bottom = m_sizeChannelRect.cy;
	
	prectImage->OffsetRect(
		prectImage->Width() * (dwIndex % m_sizeRowColCount.cx),
		prectImage->Height() * (dwIndex / m_sizeRowColCount.cx));

}

void CMultiScreenView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	CalcChannelSize();
	SetCaptureImageResolution();
}

void CMultiScreenView::SetCaptureImageResolution()
{
	CSize sizeImageRect;
	
	SRvclDeviceStatus rvclds;

	rvclds.dwSize = sizeof(rvclds);
	rvclds.dwMask = RVCL_STATUS_RESOLUTION;

	switch(m_dwCaptureSizeMode)
	{
		case RDN_CAPTURE_SIZE_FITTOWND:

			sizeImageRect = m_sizeImageRect;
			break;
		
		case RDN_CAPTURE_SIZE_CIF:
			
			if(!RvclGetDeviceStatus(0, &rvclds))
				return;

			sizeImageRect.cx = rvclds.sizeMaxResolution.cx / 2;
			sizeImageRect.cy = rvclds.sizeMaxResolution.cy / 2;
			break;

		case RDN_CAPTURE_SIZE_WHOLEFRAME:

			if(!RvclGetDeviceStatus(0, &rvclds))
				return;

			sizeImageRect = rvclds.sizeMaxResolution;
			break;

		case RDN_CAPTURE_SIZE_WHOLEFIELD:

			if(!RvclGetDeviceStatus(0, &rvclds))
				return;

			sizeImageRect.cx = rvclds.sizeMaxResolution.cx;
			sizeImageRect.cy = rvclds.sizeMaxResolution.cy / 2;
			break;
	}

	for(int i=0; i<m_arrDevices.GetSize(); i++)
		m_arrDevices[i]->SetImageResolution(sizeImageRect);

	CMainFrame*	pFrame = static_cast<CMainFrame*>(AfxGetApp()->m_pMainWnd);
	
	if(pFrame && ::IsWindow(pFrame->GetSafeHwnd()))
		pFrame->ShowPreviewSize(sizeImageRect, m_sizeImageRect);

}

void CMultiScreenView::OnTimer(UINT nIDEvent) 
{
	static DWORD dwLastTickCount = GetTickCount() - FPS_UPDATE_TIMEOUT;

	DWORD		dwTickCount = GetTickCount();
	double		dFps;
	unsigned	nFps;

	EnterCriticalSection(&m_csChannelsFps);

	for(int i=0; i<m_arrChannels.GetSize(); i++)
	{
		CChannel* pChannel = m_arrChannels[i];

		// Calculate FPS
		dFps =	1000.0 * double(pChannel->m_nFramesCount - pChannel->m_nLastFramesCount) /
				double(dwTickCount - dwLastTickCount);

		if((ceil(dFps)-dFps) < (dFps-floor(dFps)))
			nFps = UINT(ceil(dFps));
		else
			nFps = UINT(floor(dFps));

		pChannel->m_nFps = nFps;
		pChannel->m_nLastFramesCount = pChannel->m_nFramesCount;
	}
	
	LeaveCriticalSection(&m_csChannelsFps);

	dwLastTickCount = dwTickCount;
	CWnd ::OnTimer(nIDEvent);
}

// Returns expected FPS for current video format 
// (real-time mode only)
unsigned CMultiScreenView::GetExpectedFps()
{
	ASSERT(!m_fMultiplexingMode);
	
	unsigned nFps = 0;
	
	switch(m_dwVideoFormat)
	{
		case RVCL_VF_NTSCM:
		case RVCL_VF_NTSCJ:
		case RVCL_VF_PALM:
				nFps = 30;
				break;

		case RVCL_VF_PALBDGHI:
		case RVCL_VF_PALN:
		case RVCL_VF_SECAM:
				nFps = 25;
				break;

		default:
				ASSERT(false);
	}

	return nFps;
}

void CMultiScreenView::OnViewShowVideo() 
{
	m_fShowVideo = !m_fShowVideo;
}

void CMultiScreenView::OnUpdateViewShowVideo(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_fShowVideo);
}
