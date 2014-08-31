// CDrawWindow.cpp : implementation file
//

#include "stdafx.h"
#include "MainApp.h"
#include "DrawWindow.h"
#include "..\..\..\include\rdvp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CDrawWindow

CDrawWindow::CDrawWindow()
{
	int nSize = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*256;
	
	m_pBMI = (BITMAPINFO*) new char[nSize];
	memset (m_pBMI, 0, nSize);

	m_pBMI->bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
	m_pBMI->bmiHeader.biPlanes		= 1 ;
	m_pBMI->bmiHeader.biCompression = BI_RGB ;
	m_pBMI->bmiHeader.biClrUsed		= 0;
	
	for(int i = 0; i < 256; i++)
	{
		m_pBMI->bmiColors[i].rgbRed		= i ;
		m_pBMI->bmiColors[i].rgbGreen	= i ;
		m_pBMI->bmiColors[i].rgbBlue	= i ;
		m_pBMI->bmiColors[i].rgbReserved= 0 ;
	}

	m_pConvertBuffer = new BYTE[768*576*4];

	m_iW = 192;
	m_iH = 144;
	m_iBitCount = 8;
	m_pBuffer = 0;
}

CDrawWindow::~CDrawWindow()
{
	if(m_pBMI)
		delete []m_pBMI;

	delete []m_pConvertBuffer;
}


BEGIN_MESSAGE_MAP(CDrawWindow, CWnd)
	//{{AFX_MSG_MAP(CDrawWindow)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDrawWindow message handlers

void CDrawWindow::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	m_pBMI->bmiHeader.biWidth		= m_iW;
	m_pBMI->bmiHeader.biHeight		= -m_iH;
	m_pBMI->bmiHeader.biBitCount	= m_iBitCount;


	// TODO: Add your message handler code here
	dc.SetStretchBltMode(COLORONCOLOR);

	if(m_pBuffer)
	StretchDIBits(dc.m_hDC,
		0,
		0,
		m_iW, 
		m_iH,
		0,
		0,
		m_iW, 
		m_iH,
		m_pBuffer,
		(LPBITMAPINFO)m_pBMI,
		DIB_RGB_COLORS,
		SRCCOPY);
	
	// Do not call CWnd::OnPaint() for painting messages
}



void CDrawWindow::SetParams(int iW,int iH,DWORD	dwColorFormat, LPBYTE	pBuffer)
{
	if( (dwColorFormat== RDN_CF_YUV411_PACKED) || (dwColorFormat== RDN_CF_YUV411_PLANAR) ) 
		iW=iW*3/2;

	if(iW!=m_iW || iH!=m_iH)
		SetWindowPos(NULL,0,0,iW,iH+20,SWP_NOMOVE | SWP_SHOWWINDOW);

	//set internal parameters (size, draw buffer and color format)
	m_iW = iW;						//set width of draw image
	m_iH = iH;						//set height
	m_pBuffer = pBuffer;			//set draw buffer


	switch(dwColorFormat)
	{
	case(RDN_CF_RGB24):m_iBitCount = 24;break;
	case(RDN_CF_RGB32):m_iBitCount = 32;break;
	case(RDN_CF_Y8):m_iBitCount = 8;break;
	case(RDN_CF_YUV411_PLANAR): 
		{
			m_iBitCount = 8;

			m_pBuffer = m_pConvertBuffer;

			YUV411PlanarToYUV411Disp(pBuffer,m_pConvertBuffer,iW,iH);
			break;
		}
	case(RDN_CF_YUV411_PACKED):
		{
			m_iBitCount = 8;

			m_pBuffer = m_pConvertBuffer;

			YUV411PackToYUV411Disp(pBuffer,m_pConvertBuffer,iW,iH);
			

			break;
		}
	}

	Invalidate(FALSE);

	UpdateWindow();
}

////////////////////////////////////////////////////////////////////////
// Destroy window
void CDrawWindow::OnDestroy() 
{
	CWnd::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

////////////////////////////////////////////////////////////////////////
// Convert packed format to planar format
void CDrawWindow::YUV411PackToYUV411Disp(LPBYTE	pSource,LPBYTE	pDest,DWORD dwPitch,DWORD dwH)
{

	DWORD	i,j;
	DWORD	iW = dwPitch*2/3;				//in the yuv411 packed format pixel contain 12 bit

	LPBYTE	pYUVPack = pSource;

	LPBYTE	pY = pDest;						//here we put Y component
	LPBYTE	pCr = pDest + iW;				//here we put Chroma component (Cr)
	LPBYTE	pCb = pCr + iW/4;				//here we put Chroma component (Cb)

	DWORD	dwLumaDiff = iW/2;
	DWORD	dwChromaDiff = iW/4+iW;

	//the yuv411 packed format consist of
	//Cb0	Y0	Cr0	Y1	Cb4	Y2	Cr4	Y3	Y4	Y5	Y6	Y7

	//here we unpack it to the
	//Y0 Y1 Y2 Y3 Y4 Y5 Y6 Y7......Cb0 Cb4.....Cr0 Cr4

	for(i = 0; i< dwH; i++)
	{
		for(j = 0; j < dwPitch; j += 12)
		{
			*pCb++ = pYUVPack[j];
			*pCb++ = pYUVPack[j+4];

			*pCr++ = pYUVPack[j+2];
			*pCr++ = pYUVPack[j+6];

			*pY++ = pYUVPack[j+1];
			*pY++ = pYUVPack[j+3];
			*pY++ = pYUVPack[j+5];
			*pY++ = pYUVPack[j+7];

			*pY++ = pYUVPack[j+8];
			*pY++ = pYUVPack[j+9];
			*pY++ = pYUVPack[j+10];
			*pY++ = pYUVPack[j+11];
		}

		pY  += dwLumaDiff;			//go to the next line of Y
		pCb += dwChromaDiff;		//next line of Cb
		pCr += dwChromaDiff;		//next line of Cr

		pYUVPack += dwPitch;		//next line of Source Packed Data
	}

}

void CDrawWindow::YUV411PlanarToYUV411Disp(LPBYTE	pSource,LPBYTE	pDest,DWORD dwPitch,DWORD dwH)
{

	DWORD	i,j;
	DWORD	iW = dwPitch*2/3;				//in the yuv411 packed format pixel contain 12 bit
	DWORD	dwSize = iW*dwH;

	LPBYTE	pY0 = pSource;
	LPBYTE	pU0 = pSource + dwSize;
	LPBYTE	pV0 = pU0 + dwSize/4;


	LPBYTE	pY = pDest;						//here we put Y component
	LPBYTE	pCb = pDest + iW;				//here we put Chroma component (Cr)
	LPBYTE	pCr = pCb + iW/4;				//here we put Chroma component (Cb)

	//the yuv411 packed format consist of
	//Cb0	Y0	Cr0	Y1	Cb4	Y2	Cr4	Y3	Y4	Y5	Y6	Y7

	//here we unpack it to the
	//Y0 Y1 Y2 Y3 Y4 Y5 Y6 Y7......Cb0 Cb4.....Cr0 Cr4

	for(i = 0; i< dwH; i++)
	{
		memcpy(pY, pY0, iW);	// copy y
		memcpy(pCr, pU0, iW/4);
		memcpy(pCb, pV0, iW/4);

		pY0 += iW;
		pU0 += iW/4;
		pV0 += iW/4;

		pY += dwPitch;
		pCr += dwPitch;
		pCb += dwPitch;
	}

}


