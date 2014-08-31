// VideoView.h : interface of the CVideoView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIDEOVIEW_H__BA79E24F_9907_11D3_9584_00A0244E460B__INCLUDED_)
#define AFX_VIDEOVIEW_H__BA79E24F_9907_11D3_9584_00A0244E460B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ControlPanelDlg.h"

// Maximum number of pending requests
#define REQUESTS_NUMBER		2

// "Show image" message
#define WM_SHOW_IMAGE		(WM_USER + 20)

/////////////////////////////////////////////////////////////////////////////

class CVideoView : public CView
{
private:
	// Video capture request
	struct SRequest
	{
		int			nDevice;	// zero-based device index
		DWORD		dwError;	// Win32 API error
		SRvclFrame	rvclf;		// RVCL frame settings
		OVERLAPPED	overlapped;
		HANDLE		hTimer;		// waitable timer
	};

protected: // create from serialization only
	CVideoView();
	DECLARE_DYNCREATE(CVideoView)

// Attributes
public:
	CVideoDoc* GetDocument();
	
	void	GetMinMaxSize(CSize& sizeMin,CSize& sizeMax);
				
// Operations
public:
	bool	Update(int nDevice,DWORD dwColorFormat);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVideoView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CVideoView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	// Child windows
	CControlPanelDlg*	m_pControlPanel;
	
	// Image settings
	int			m_nDevice;			// current device index
	DWORD		m_dwColorFormat;	// current color format
	CSize		m_sizeMin,			// minimum image resolution
				m_sizeMax,			// maximum image resolution
				m_sizeAlignment;
	DWORD		m_dwFlags;

	CRITICAL_SECTION	m_cs;
	
	// Threads
	HANDLE		m_hCaptureThread;	// capture thread
	
	// Events
	HANDLE		m_hStopCaptureEvent;

	// Statistics
	DWORD		m_dwFrameCount;		// total frame counter
	UINT		m_nTimer;			// timer for FPS update

	// Requests
	SRequest	m_aRequests[REQUESTS_NUMBER];
	
	// Paint data
	UINT		m_nReadyRequest;	// request to show
	BITMAPINFO*	m_pBMI;
	CFont*		m_pFont;			// "NO SIGNAL" font

	BOOL		m_fDeinterlacing;
	
private:
	void	SetSize(CSize& size);

	static DWORD WINAPI CaptureThread(LPVOID pParam);
	BOOL PostCaptureFiltering(SRvclFrame* rvclf);

// Generated message map functions
protected:
	//{{AFX_MSG(CVideoView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnWindowFullFrame();
	afx_msg void OnWindowQcif();
	afx_msg void OnWindowCif();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LONG OnShowImage(WPARAM wParam,LPARAM lParam);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnImageTopDown();
	afx_msg void OnImageBottomUp();
	afx_msg void OnUpdateImageTopDown(CCmdUI* pCmdUI);
	afx_msg void OnUpdateImageBottomUp(CCmdUI* pCmdUI);
	afx_msg void OnWindowDeinterlacingfilter();
	afx_msg void OnUpdateWindowDeinterlacingfilter(CCmdUI *pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in VideoView.cpp
inline CVideoDoc* CVideoView::GetDocument()
   { return (CVideoDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
// Inline functions

inline void CVideoView::GetMinMaxSize(CSize& sizeMin,CSize& sizeMax)
{
	sizeMin = m_sizeMin;
	sizeMax = m_sizeMax;
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIDEOVIEW_H__BA79E24F_9907_11D3_9584_00A0244E460B__INCLUDED_)
