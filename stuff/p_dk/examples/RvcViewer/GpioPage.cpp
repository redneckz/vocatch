// GpioPage.cpp : implementation file
//

#include "stdafx.h"
#include "RvcViewer.h"
#include "GpioPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGpioPage dialog


CGpioPage::CGpioPage(CControlPanelDlg* pControlPanel)
	: CControlPageBase(pControlPanel, 300)
{
	//{{AFX_DATA_INIT(CGpioPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CGpioPage::~CGpioPage()
{
}

void CGpioPage::DoDataExchange(CDataExchange* pDX)
{
	CControlPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGpioPage)
	DDX_Control(pDX, IDC_LIST_GPIO, m_listGpio);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGpioPage, CControlPageBase)
	//{{AFX_MSG_MAP(CGpioPage)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_GPIO, OnItemChanged_ListGpio)
	ON_BN_CLICKED(IDC_BUTTON_SET, OnButton_Set)
	ON_BN_CLICKED(IDC_BUTTON_RESET, OnButton_Reset)
	ON_BN_CLICKED(IDC_BUTTON_INPUT, OnButton_Input)
	ON_BN_CLICKED(IDC_BUTTON_OUTPUT, OnButton_Output)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGpioPage message handlers

void CGpioPage::OnCancel() 
{
}

void CGpioPage::OnOK() 
{
}

/////////////////////////////////////////////////////////////////////////////

BOOL CGpioPage::OnInitDialog() 
{
	CControlPageBase::OnInitDialog();
	
	m_listGpio.SetExtendedStyle(
		m_listGpio.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	CString	str;
	str.LoadString(IDS_GPIO_PIN); m_listGpio.InsertColumn(0,str);
	str.LoadString(IDS_GPIO_MODE); m_listGpio.InsertColumn(1,str);
	str.LoadString(IDS_GPIO_DATA); m_listGpio.InsertColumn(2,str);

	for(int i=0; i<24; i++)
	{
		str.Format(_T("%u"),i);
		m_listGpio.InsertItem(i,str);
	}

	m_listGpio.SetColumnWidth(0,LVSCW_AUTOSIZE_USEHEADER);
	m_listGpio.SetColumnWidth(1,LVSCW_AUTOSIZE_USEHEADER);
	m_listGpio.SetColumnWidth(2,LVSCW_AUTOSIZE_USEHEADER);

	m_listGpio.SetItemState(0,LVIS_SELECTED,LVIS_SELECTED);
	UpdateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////

void CGpioPage::OnItemChanged_ListGpio(NMHDR* pNMHDR, LRESULT* pResult) 
{
	//LPNMLISTVIEW pnmlv = (LPNMLISTVIEW)pNMHDR;
	
	UpdateControls();
	*pResult = 0;
}

/////////////////////////////////////////////////////////////////////////////

void CGpioPage::ShowCurrentState()
{
	SRvclGpioControl gpio;

	// Get GPIO output enable & data
	gpio.dwSize = sizeof(gpio);
	gpio.dwMask = RVCL_GPIO_GET_OUTPUT_ENABLE|RVCL_GPIO_GET_DATA;

	if(RvclGpioControl(GetCurrentDevice(),&gpio))
	{
		CString	strInput,strOutput,str;
		int		i,nCount = m_listGpio.GetItemCount();
		DWORD	dw;
		
		strInput.LoadString(IDS_INPUT);
		strOutput.LoadString(IDS_OUTPUT);

		for(i=0,dw=1; i<nCount; i++,dw<<=1)
		{
			// Update "mode" column
			str = (gpio.dwOutputEnable & dw) ? strOutput : strInput;
			
			if(m_listGpio.GetItemText(i,1) != str)
				m_listGpio.SetItemText(i,1,str);

			// Update "data" column
			str = (gpio.dwData & dw) ? _T("1") : _T("0");

			if(m_listGpio.GetItemText(i,2) != str)
				m_listGpio.SetItemText(i,2,str);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////

void CGpioPage::OnButton_Set() 
{
	SetData(m_listGpio.GetNextItem(-1,LVNI_SELECTED),true);
}

/////////////////////////////////////////////////////////////////////////////

void CGpioPage::OnButton_Reset() 
{
	SetData(m_listGpio.GetNextItem(-1,LVNI_SELECTED),false);
}

/////////////////////////////////////////////////////////////////////////////

void CGpioPage::SetData(int iItem, bool bSet)
{
	if(iItem < 0 || iItem >= m_listGpio.GetItemCount())
		return;

	SRvclGpioControl gpio;

	// Set new GPIO data (with mask)
	gpio.dwSize = sizeof(gpio);
	gpio.dwMask = RVCL_GPIO_SET_DATA;
	gpio.dwDataMask = DWORD(1) << iItem;
	gpio.dwData = bSet ? gpio.dwDataMask : 0;
	
	RvclGpioControl(GetCurrentDevice(),&gpio);
	ShowCurrentState();
}

/////////////////////////////////////////////////////////////////////////////

void CGpioPage::OnButton_Input() 
{
	SetOutputEnable(m_listGpio.GetNextItem(-1,LVNI_SELECTED),false);
	UpdateControls();
}

void CGpioPage::OnButton_Output() 
{
	SetOutputEnable(m_listGpio.GetNextItem(-1,LVNI_SELECTED),true);
	UpdateControls();
}

/////////////////////////////////////////////////////////////////////////////

void CGpioPage::SetOutputEnable(int iItem, bool bOutput)
{
	if(iItem < 0 || iItem >= m_listGpio.GetItemCount())
		return;

	SRvclGpioControl gpio;

	// Set new GPIO output enable (with mask)
	gpio.dwSize = sizeof(gpio);
	gpio.dwMask = RVCL_GPIO_SET_OUTPUT_ENABLE;
	gpio.dwOutputEnableMask = DWORD(1) << iItem;
	gpio.dwOutputEnable = bOutput ? gpio.dwOutputEnableMask : 0;

	RvclGpioControl(GetCurrentDevice(),&gpio);
	ShowCurrentState();
}

/////////////////////////////////////////////////////////////////////////////

void CGpioPage::UpdateControls()
{
	BOOL	bOutputSelected = false;
	int		iItem = m_listGpio.GetNextItem(-1,LVNI_SELECTED);

	if(iItem != -1)
	{
		SRvclGpioControl gpio;

		// Get GPIO output enable
		gpio.dwSize = sizeof(gpio);
		gpio.dwMask = RVCL_GPIO_GET_OUTPUT_ENABLE;

		RvclGpioControl(GetCurrentDevice(),&gpio);

		// Test selected item pin
		bOutputSelected = gpio.dwOutputEnable & (DWORD(1) << iItem);
	}

	GetDlgItem(IDC_BUTTON_SET)->EnableWindow(bOutputSelected);
	GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(bOutputSelected);
}

/////////////////////////////////////////////////////////////////////////////
