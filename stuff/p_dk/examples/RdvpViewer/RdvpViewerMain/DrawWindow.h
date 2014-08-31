#if !defined(AFX_CDrawWindow_H__525DCC7B_995C_4E8C_B2C0_6CB68B4DAF1A__INCLUDED_)
#define AFX_CDrawWindow_H__525DCC7B_995C_4E8C_B2C0_6CB68B4DAF1A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CDrawWindow.h : header file
//



struct SImageProcessing
{
	DWORD	dwSize;				// Size of this structure (in bytes)
	DWORD	dwFlags;			// Flags
	DWORD	dwPrivateFlags;		// Private flags - filter depending (see below)
	
	SIZE	sizeSrcResolution;	// Source resoulution
	DWORD	dwSrcPitch;			// Source pitch
	DWORD	dwSrcColorFormat;	// Source color format
	LPVOID	pSrcDataPtr;		// Source image pointer
	DWORD	dwSrcDataSize;		// Source image data (buffer) size

	SIZE	sizeDestResolution;	// Destination resolution
	DWORD	dwDestPitch;		// Destination pitch
	DWORD	dwDestColorFormat;	// Destination color format
	LPVOID	pDestDataPtr;		// Destination image pointer
	DWORD	dwDestDataSize;		// Destination data (buffer) size

	LPVOID	pParametersPtr;		// Parameters data pointer
	DWORD	dwParametersSize;	// Parameters data size
};

/////////////////////////////////////////////////////////////////////////////
// CDrawWindow window

class CDrawWindow : public CWnd
{
// Construction
public:
	CDrawWindow();

	void SetParams(int iW,int iH,DWORD	dwColorFormat, LPBYTE	pBuffer);

// Attributes
public:

// Operations
public:


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDrawWindow)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDrawWindow();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDrawWindow)
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void YUV411PackToYUV411Disp(LPBYTE	pSource,LPBYTE	pDest,DWORD dwPitch,DWORD dwH);
	void YUV411PlanarToYUV411Disp(LPBYTE	pSource,LPBYTE	pDest,DWORD dwPitch,DWORD dwH);
private:
	BITMAPINFO* m_pBMI;
	HPALETTE m_hPalette;

	INT		m_iW;				//width of the image
	INT		m_iH;				//height if the image
	INT		m_iBitCount;		//bit count (24 - for true color, 8 for grayscale)
	LPBYTE	m_pBuffer;			//pointer to draw buffer

	LPBYTE	m_pConvertBuffer;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CDrawWindow_H__525DCC7B_995C_4E8C_B2C0_6CB68B4DAF1A__INCLUDED_)
