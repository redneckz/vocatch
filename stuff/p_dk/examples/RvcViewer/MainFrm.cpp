// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "RvcViewer.h"

#include "MainFrm.h"
#include "VideoDoc.h"
#include "VideoView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define PANE_MAIN	0
#define PANE_SIZE	1
#define PANE_FPS	2

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,	// status line indicator
	ID_SEPARATOR,	// preview size
	ID_SEPARATOR	// FPS
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

/////////////////////////////////////////////////////////////////////////////

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style &= ~FWS_ADDTOTITLE; 
	
	if(!CFrameWnd::PreCreateWindow(cs))
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	CVideoView* pView = static_cast<CVideoView*>(GetActiveView());

	if(pView && ::IsWindow(pView->GetSafeHwnd()))
	{
		CSize	sizeMin,sizeMax;
		RECT	rectMine,rectView;
		int		nFrameWidth,nFrameHeight;
		
		// Get min & max view size
		pView->GetMinMaxSize(sizeMin,sizeMax);

		// Get current view rect
		pView->GetClientRect(&rectView);

		// Get frame window rect
		GetWindowRect(&rectMine);
		
		nFrameWidth = rectMine.right - rectMine.left - rectView.right;
		nFrameHeight = rectMine.bottom - rectMine.top - rectView.bottom;

		// задать размеры
		if((nFrameWidth > 0) && (nFrameHeight > 0))
		{
			lpMMI->ptMinTrackSize.x = sizeMin.cx + nFrameWidth;
			lpMMI->ptMinTrackSize.y = sizeMin.cy + nFrameHeight;
			lpMMI->ptMaxTrackSize.x = sizeMax.cx + nFrameWidth;
			lpMMI->ptMaxTrackSize.y = sizeMax.cy + nFrameHeight;
			lpMMI->ptMaxSize.x = sizeMax.cx + nFrameWidth;
			lpMMI->ptMaxSize.y = sizeMax.cy + nFrameHeight;
		}
	}

	CFrameWnd::OnGetMinMaxInfo(lpMMI);
}

/////////////////////////////////////////////////////////////////////////////

void CMainFrame::ShowPreviewSize(CSize &size)
{
	CString	str;

	str.Format(_T("%u : %u"),size.cx,size.cy);
	m_wndStatusBar.SetPaneText(PANE_SIZE,LPCTSTR(str));
}

/////////////////////////////////////////////////////////////////////////////

void CMainFrame::ShowFps(UINT nFps)
{
	CString	str;

	str.Format(_T("%u FPS"),nFps);
	m_wndStatusBar.SetPaneText(PANE_FPS,LPCTSTR(str));
}

/////////////////////////////////////////////////////////////////////////////

void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	int		nWidth,nCount = sizeof(indicators)/sizeof(UINT);
	UINT	nID,nStyle;

	// Resize status bar panes
	if(IsWindow(m_wndStatusBar))
		for(int i=0; i<nCount; i++)
		{
			m_wndStatusBar.GetPaneInfo(i,nID,nStyle,nWidth);
			m_wndStatusBar.SetPaneInfo(i,nID,nStyle,cx/nCount);
		}
	
	CFrameWnd::OnSize(nType, cx, cy);
}
