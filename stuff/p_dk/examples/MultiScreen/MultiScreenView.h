// MultiScreenView.h : interface of the CMultiScreenView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MULTISCREENVIEW_H__337E6DCB_5344_4195_8E73_FF3D95821DE2__INCLUDED_)
#define AFX_MULTISCREENVIEW_H__337E6DCB_5344_4195_8E73_FF3D95821DE2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
#include "CaptureDevice.h"

/////////////////////////////////////////////////////////////////////////////
// CMultiScreenView window

class CMultiScreenView : public CWnd
{
// Construction
public:
	CMultiScreenView();

// Attributes
public:
	struct SFormat
	{
		DWORD	dwRvclFormat;
		UINT	nResourceID;
	};

	class CChannel
	{
	public:
		// FPS
		unsigned m_nFps;
		unsigned m_nFramesCount;
		unsigned m_nLastFramesCount;

		CChannel()
		{
			m_nFps = 0;
			m_nFramesCount = 0;
			m_nLastFramesCount = 0;
		}
	};
	

// Operations
public:
	void DrawImage(CImage* pImage);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMultiScreenView)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMultiScreenView();

private:
	static DWORD WINAPI DrawThread(LPVOID lpParameter);
	
	DWORD DrawEngine();
	void InternalDrawImage(CImage* pImage);
	void Packed422ToPlanar(const BYTE* pbPackedImage,BYTE* pbPlanarImage,SIZE sizeResolution) const;
	void Packed411ToPlanar(const BYTE* pbPackedImage,BYTE* pbPlanarImage,SIZE sizeResolution) const;

	void CalcRowColCount();
	void CalcChannelSize();
	void SetCaptureImageResolution();
	void GetChannelRect(DWORD dwIndex, CRect *prectImage);
	unsigned GetExpectedFps();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMultiScreenView)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnViewColorFormat(UINT nID);
	afx_msg void OnUpdateViewColorFormat(CCmdUI* pCmdUI);
	afx_msg void OnViewVideoFormat(UINT nID);
	afx_msg void OnUpdateViewVideoFormat(CCmdUI* pCmdUI);
	afx_msg void OnViewCaptureSizeMode(UINT nID);
	afx_msg void OnUpdateViewCaptureSizeMode(CCmdUI* pCmdUI);
	afx_msg void OnViewMultiplexing();
	afx_msg void OnUpdateViewMultiplexing(CCmdUI* pCmdUI);
	afx_msg void OnView43Ratio();
	afx_msg void OnUpdateView43Ratio(CCmdUI* pCmdUI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnViewShowVideo();
	afx_msg void OnUpdateViewShowVideo(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	DWORD		m_dwColorFormat;			// current color format
	DWORD		m_dwVideoFormat;			// current video format
	DWORD		m_dwCaptureSizeMode;
	CSize		m_sizeMinResolution,		// minimum image resolution
				m_sizeMaxResolution,		// maximum image resolution
				m_sizeAlignment;
	DWORD		m_dwFlags;
	
	bool		m_fMultiplexingMode;		// multiplexing mode
	bool		m_f43Ratio;					// 4:3 image ratio
	bool		m_fShowVideo;				// show video streams

	CSize		m_sizeChannelRect;
	CSize		m_sizeImageRect;
	CSize		m_sizeRowColCount;

	BITMAPINFO*	m_pBMI;
	CFont*		m_pFont;					// "NO SIGNAL" font

	CArray<CCaptureDevice*,CCaptureDevice*> m_arrDevices;
	CArray<CChannel*,CChannel*> m_arrChannels;

	HANDLE		m_heStop;
	HANDLE		m_heNewImage;
	HANDLE		m_htDrawThread;

	CRITICAL_SECTION		m_csReadyImages;
	CList<CImage*,CImage*>	m_lstReadyImages;

	// Temporary YUV buffer for packed YUV images
	LPBYTE		m_pbYuvBuffer;

	UINT		m_nFpsTimer;				// timer for FPS update

	CRITICAL_SECTION m_csChannelsFps;

	static SFormat s_arrColorFormats[];
	static SFormat s_arrVideoFormats[];
	static SFormat s_arrCaptureSizeModes[];
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MULTISCREENVIEW_H__337E6DCB_5344_4195_8E73_FF3D95821DE2__INCLUDED_)
