// ControlPanelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RvcViewer.h"
#include "ControlPanelDlg.h"

#include "VideoDoc.h"
#include "VideoView.h"

#include "GeneralPage.h"
#include "GpioPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CControlPanelDlg dialog

CControlPanelDlg::CControlPanelDlg() 
{
	//{{AFX_DATA_INIT(CControlPanelDlg)
	//}}AFX_DATA_INIT
	m_pView = NULL;
}

/////////////////////////////////////////////////////////////////////////////

CControlPanelDlg::~CControlPanelDlg() 
{
	for(int i=0; i<m_aPages.GetSize(); i++)
		delete m_aPages[i];
}

/////////////////////////////////////////////////////////////////////////////

BOOL CControlPanelDlg::Create(CVideoView* pParentWnd)
{
	ASSERT(pParentWnd != NULL);
	m_pView = pParentWnd;
	return CDialog::Create(CControlPanelDlg::IDD,pParentWnd);
}

/////////////////////////////////////////////////////////////////////////////

void CControlPanelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CControlPanelDlg)
	DDX_Control(pDX, IDC_TAB_PAGES, m_PagesTabCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CControlPanelDlg, CDialog)
	//{{AFX_MSG_MAP(CControlPanelDlg)
	ON_CBN_SELCHANGE(IDC_COMBO_DEVICES, OnSelChange_ComboDevices)
	ON_NOTIFY(TCN_SELCHANGING, IDC_TAB_PAGES, OnSelChanging_TabPages)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_PAGES, OnSelChange_TabPages)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControlPanelDlg message handlers

void CControlPanelDlg::OnCancel() 
{
}

void CControlPanelDlg::OnOK() 
{
}

/////////////////////////////////////////////////////////////////////////////

BOOL CControlPanelDlg::OnInitDialog() 
{
	int		i,j,nDevices;
	CString	strMsg,strCap,str;
		
	CDialog::OnInitDialog();
	
	// Get video capture devices count
	if(!RvclGetDevicesCount(&nDevices) || (nDevices == 0))
	{
		strCap.LoadString(IDS_ERROR);
		strMsg.LoadString(IDS_ERR_NO_DEVICES);
		MessageBox(strMsg,strCap,MB_OK|MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	// Create devices combo
	CComboBox* pDevicesCombo = (CComboBox*)GetDlgItem(IDC_COMBO_DEVICES);
	ASSERT(pDevicesCombo && ::IsWindow(pDevicesCombo->GetSafeHwnd()));

	// Load defaults: source #0, PAL format, default adjustments
	for(i = 0; i < nDevices; i++)
	{
		RvclSetVideoSource(i, 0);
		RvclSetVideoFormat(i, RVCL_VF_PALBDGHI);

		// Get device static info
		SRvclDeviceInfo rvcldi;
		rvcldi.dwSize = sizeof(rvcldi);

		if(!RvclGetDeviceInfo(i, &rvcldi))
		{
			DWORD dwError = GetLastError();

			strCap.LoadString(IDS_ERROR);
			strMsg.Format(IDS_ERR_CANT_GET_DEVICE_INFO,ERROR_MESSAGE(dwError), dwError);

			MessageBox(strMsg, strCap, MB_OK | MB_ICONSTOP | MB_TOPMOST);
		}
		else
		{
			// User-readable name = device index + PCI ID + PCI bus + PCI slot
			str.Format(IDS_FULL_DEVICE_NAME, i, rvcldi.szDeviceDesc, rvcldi.dwPciBusNumber, rvcldi.dwPciSlotNumber);

			j = pDevicesCombo->AddString(str);
	
			// Items's DWORD data = device index
			pDevicesCombo->SetItemData(j,i);

			SRvclVideoAdjustments rvclva;
		
			rvclva.dwSize = sizeof(rvclva);
			rvclva.dwMask =	RVCL_VADJUST_BRIGHTNESS |
							RVCL_VADJUST_CONTRAST |
							RVCL_VADJUST_SATURATION |
							RVCL_VADJUST_HUE |
							RVCL_VADJUST_COMBRANGE;

			rvclva.dwBrightness = CGeneralPage::s_aVideoAdjustmentsDefault[rvcldi.dwDeviceType].dwBrightness;
			rvclva.dwContrast   = CGeneralPage::s_aVideoAdjustmentsDefault[rvcldi.dwDeviceType].dwContrast;
			rvclva.dwSaturation = CGeneralPage::s_aVideoAdjustmentsDefault[rvcldi.dwDeviceType].dwSaturation;
			rvclva.dwHue        = CGeneralPage::s_aVideoAdjustmentsDefault[rvcldi.dwDeviceType].dwHue;
			rvclva.dwCombRange  = CGeneralPage::s_aVideoAdjustmentsDefault[rvcldi.dwDeviceType].dwCombRange;
			
			RvclSetVideoAdjustments(i, &rvclva);
		}
	}

	// Create Tab Control pages
	CControlPageBase* pPage;

	pPage = new CGeneralPage(this);
	pPage->Create(CGeneralPage::IDD, &m_PagesTabCtrl);
	m_aPages.Add(pPage);
	str.LoadString(IDS_PAGE_GENERAL);
	m_PagesTabCtrl.InsertItem(0,str);

	pPage = new CGpioPage(this);
	pPage->Create(UINT(CGpioPage::IDD), &m_PagesTabCtrl);
	m_aPages.Add(pPage);
	str.LoadString(IDS_PAGE_GPIO);
	m_PagesTabCtrl.InsertItem(1,str);

	RECT rect;

	for(i=0; i<m_aPages.GetSize(); i++)
	{
		m_PagesTabCtrl.GetItemRect(i,&rect);
		if(rect.left > 0)
			break;
	}

	int xEdge = GetSystemMetrics(SM_CXEDGE);
	int yEdge = GetSystemMetrics(SM_CYEDGE);

	for(i=0; i<m_aPages.GetSize(); i++)
	{
		m_aPages[i]->SetWindowPos(
			NULL,rect.left+xEdge,rect.bottom+yEdge,0,0,SWP_NOZORDER|SWP_NOSIZE);
	}

	m_PagesTabCtrl.SetCurSel(0);
	m_aPages[0]->ShowWindow(SW_SHOW);
	m_aPages[0]->SetFocus();

	// Select first device
	pDevicesCombo->SetCurSel(0);
	OnSelChange_ComboDevices();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////

// Updates view settings
void CControlPanelDlg::UpdateView() const
{
	ASSERT(m_pView);
	ASSERT((m_aPages.GetSize() >= 1) && m_aPages[0]);

	m_pView->Update(
		GetCurrentDevice(),
		((CGeneralPage*)m_aPages[0])->GetColorFormat());
}

/////////////////////////////////////////////////////////////////////////////

// Updates current device status & video adjustments
void CControlPanelDlg::ShowCurrentState()
{
	for(int i=0; i<m_aPages.GetSize(); i++)
		m_aPages[i]->ShowCurrentState();
}

/////////////////////////////////////////////////////////////////////////////

// CBN_SELCHANGE for devices combo
void CControlPanelDlg::OnSelChange_ComboDevices() 
{
	// Show current device status & update view settings
	ShowCurrentState();
	UpdateView();
}

/////////////////////////////////////////////////////////////////////////////

void CControlPanelDlg::OnSelChanging_TabPages(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Call GetCurSel() here will return the index of the page that
	// will be hidden soon.  This is NOT the newly selected page when 
	// OnTabSelChange() is called.
	CTabCtrl* pTabCtrl = (CTabCtrl*)GetDlgItem(IDC_TAB_PAGES);

	int	nSelectedItem = pTabCtrl->GetCurSel();

	if(nSelectedItem != -1)
		m_aPages[nSelectedItem]->ShowWindow(SW_HIDE);

	*pResult = FALSE;	// return FALSE to allow seletion to change
}

/////////////////////////////////////////////////////////////////////////////

void CControlPanelDlg::OnSelChange_TabPages(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Call GetCurSel() will return the index of the page that is newly
	// selected.  This page will be visible soon.
	CTabCtrl* pTabCtrl = (CTabCtrl*)GetDlgItem(IDC_TAB_PAGES);
	
	int nSelectedItem = pTabCtrl->GetCurSel();
	
	// Set input focus to this page.
	if(nSelectedItem != -1)
	{
		m_aPages[nSelectedItem]->ShowWindow(SW_SHOW);
		m_aPages[nSelectedItem]->SetFocus();
	}
	
	*pResult = 0;
}

/////////////////////////////////////////////////////////////////////////////
