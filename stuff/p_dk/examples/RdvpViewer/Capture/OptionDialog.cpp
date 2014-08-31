// OptionDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Capture.h"
#include "OptionDialog.h"

#include "Grabber.h"

#include "..\..\..\include\rvcl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Video formats
const COptionDialog::SFormat COptionDialog::s_aVideoFormats[] = 
{	
	{RVCL_VF_NTSCM,		IDS_VF_NTSC_M},
	{RVCL_VF_NTSCJ,		IDS_VF_NTSC_J},
	{RVCL_VF_PALBDGHI,	IDS_VF_PAL_BDGHI},
	{RVCL_VF_PALM,		IDS_VF_PAL_M},
	{RVCL_VF_PALN,		IDS_VF_PAL_N},
	{RVCL_VF_SECAM,		IDS_VF_SECAM}
};


/////////////////////////////////////////////////////////////////////////////
// COptionDialog dialog


COptionDialog::COptionDialog(CWnd* pParent /*=NULL*/)
	: CDialog(COptionDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionDialog)
	//}}AFX_DATA_INIT
}

COptionDialog::COptionDialog(CCaptureApp* pParent /*=NULL*/)
	: CDialog(COptionDialog::IDD, NULL)
{
	//{{AFX_DATA_INIT(COptionDialog)
	//}}AFX_DATA_INIT

	m_pParent = pParent;

	m_nDevice = m_pParent->m_nDevice;
	m_nInput =  m_pParent->m_nInput;
	m_dwFormat = m_pParent->m_dwFormat;


}


void COptionDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionDialog)
	DDX_Control(pDX, IDC_COMBO_FORMAT, m_cmbFormat);
	DDX_Control(pDX, IDC_COMBO_INPUT, m_cmbInput);
	DDX_Control(pDX, IDC_COMBO_DEVICE, m_cmbDevice);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionDialog, CDialog)
	//{{AFX_MSG_MAP(COptionDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_DEVICE, OnSelchange_ComboDevice)
	ON_CBN_SELCHANGE(IDC_COMBO_INPUT, OnSelchange_ComboInput)
	ON_CBN_SELCHANGE(IDC_COMBO_FORMAT, OnSelchange_ComboFormat)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionDialog message handlers

BOOL COptionDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	CString	strMsg,strCap;
	int nDevices;

	// Get video capture devices count
	if(!RvclGetDevicesCount(&nDevices) || (nDevices == 0))
	{
	
		strCap.LoadString(IDS_ERROR);
		strMsg.LoadString(IDS_ERR_NO_DEVICES);
		MessageBox(strMsg,strCap,MB_OK|MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}


	int j;
	for(int i=0; i<nDevices; i++)
	{
		// Get device static info
		SRvclDeviceInfo rvcldi;
		rvcldi.dwSize = sizeof(rvcldi);

		if(!RvclGetDeviceInfo(i,&rvcldi))
		{
			strCap.LoadString(IDS_ERROR);
			strMsg.LoadString(IDS_ERR_CANT_GET_DEVICE_INFO);

			MessageBox(strMsg,strCap,MB_OK|MB_ICONSTOP|MB_TOPMOST);
		}
		else
		{
			// User-readable name = device index + PCI ID + PCI bus + PCI slot
			strMsg.Format(
				IDS_FULL_DEVICE_NAME,
				i,rvcldi.szDeviceDesc,rvcldi.dwPciBusNumber,rvcldi.dwPciSlotNumber);

			j = m_cmbDevice.AddString(strMsg);
	
			// Items's DWORD data = device index
			m_cmbDevice.SetItemData(j,i);
		}
	}

	m_cmbDevice.SetCurSel(m_nDevice);

	for(i=0; i<4; i++)
	{
		strMsg.Format(IDS_VIDEO_SOURCE,i);
		j = m_cmbInput.AddString(strMsg);
		
		// application-defined item data = source number
		m_cmbInput.SetItemData(j,i);
	}

	m_cmbInput.SetCurSel(m_nInput);

	for(i=0; i<sizeof(s_aVideoFormats)/sizeof(SFormat); i++)
	{
		// Load format name from resource
		strMsg.LoadString(s_aVideoFormats[i].lStringID);
		j = m_cmbFormat.AddString(strMsg);
	
		// application-defined item data = RVCL video format flag
		m_cmbFormat.SetItemData(j,s_aVideoFormats[i].dwRvclFlag);

		if(s_aVideoFormats[i].dwRvclFlag == m_dwFormat) m_cmbFormat.SetCurSel(i);

	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionDialog::OnSelchange_ComboFormat() 
{
	// TODO: Add your control notification handler code here
	int i = m_cmbFormat.GetCurSel();

	if(i == CB_ERR)
		return;

	// Video format = item's DWORD data
	m_dwFormat = m_cmbFormat.GetItemData(i);

	m_pParent->SetVideoOptions(m_nDevice,m_nInput,m_dwFormat);

}

void COptionDialog::OnSelchange_ComboDevice() 
{
	// TODO: Add your control notification handler code here
	int i = m_cmbDevice.GetCurSel();

	if(i == CB_ERR)
		return;

	// Device = item's DWORD data
	m_nDevice = m_cmbDevice.GetItemData(i);

	m_pParent->SetVideoOptions(m_nDevice,m_nInput,m_dwFormat);
	
}

void COptionDialog::OnSelchange_ComboInput() 
{
	// TODO: Add your control notification handler code here
	int i = m_cmbInput.GetCurSel();

	if(i == CB_ERR)
		return;

	// Input = item's DWORD data
	m_nInput = m_cmbInput.GetItemData(i);

	m_pParent->SetVideoOptions(m_nDevice,m_nInput,m_dwFormat);


}

void COptionDialog::OnOK() 
{
	// TODO: Add extra validation here

	m_pParent->SetVideoOptions(m_nDevice,m_nInput,m_dwFormat);

	CDialog::OnOK();
}
