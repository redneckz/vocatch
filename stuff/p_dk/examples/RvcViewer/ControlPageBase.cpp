// ControlPageBase.cpp : implementation file
//

#include "stdafx.h"
#include "RvcViewer.h"
#include "ControlPageBase.h"
#include "ControlPanelDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CControlPageBase

CControlPageBase::CControlPageBase(CControlPanelDlg* pControlPanel, UINT nStatusUpdateInterval)
	: CDialog()
{
	ASSERT(pControlPanel);
	m_pControlPanel = pControlPanel;
	m_nStatusUpdateInterval = nStatusUpdateInterval;
	m_nStatusTimer = 0;
}

CControlPageBase::~CControlPageBase()
{
}


BEGIN_MESSAGE_MAP(CControlPageBase, CDialog)
	//{{AFX_MSG_MAP(CControlPageBase)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControlPageBase message handlers

int CControlPageBase::GetCurrentDevice() const
{
	ASSERT(m_pControlPanel);
	return m_pControlPanel->GetCurrentDevice();
}

CString CControlPageBase::GetCurrentDeviceName() const
{
	ASSERT(m_pControlPanel);
	return m_pControlPanel->GetCurrentDeviceName();
}

void CControlPageBase::UpdateView() const
{
	ASSERT(m_pControlPanel);
	m_pControlPanel->UpdateView();
}

/////////////////////////////////////////////////////////////////////////////

BOOL CControlPageBase::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Create timer for status update
	m_nStatusTimer = SetTimer(1,m_nStatusUpdateInterval,NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////

void CControlPageBase::OnTimer(UINT nIDEvent) 
{
	ShowCurrentState();
	CDialog::OnTimer(nIDEvent);
}
